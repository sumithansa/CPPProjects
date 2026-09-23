#include <sk/thread_pool.hpp>

#include <algorithm>
#include <stdexcept>

namespace sk {

ThreadPool::ThreadPool(std::size_t thread_count) {
    thread_count = std::max<std::size_t>(thread_count, 1);
    workers_.reserve(thread_count);
    try {
        for (std::size_t i = 0; i < thread_count; ++i) {
            workers_.emplace_back([this] { worker_loop(); });
        }
    } catch (...) {
        // Thread creation failed part-way: stop the threads that did start.
        {
            std::scoped_lock lock(mutex_);
            stopping_ = true;
        }
        work_available_.notify_all();
        for (auto& worker : workers_) {
            worker.join();
        }
        throw;
    }
}

ThreadPool::~ThreadPool() {
    {
        std::scoped_lock lock(mutex_);
        stopping_ = true;
    }
    work_available_.notify_all();
    for (auto& worker : workers_) {
        worker.join();
    }
}

std::size_t ThreadPool::default_thread_count() noexcept {
    // hardware_concurrency() may legally return 0 when the value is unknown.
    return std::max(1u, std::thread::hardware_concurrency());
}

void ThreadPool::enqueue(std::function<void()> job) {
    {
        std::scoped_lock lock(mutex_);
        if (stopping_) {
            throw std::runtime_error("sk::ThreadPool: submit() called after shutdown");
        }
        jobs_.push(std::move(job));
    }
    work_available_.notify_one();
}

void ThreadPool::wait_idle() {
    std::unique_lock lock(mutex_);
    idle_.wait(lock, [this] { return jobs_.empty() && active_ == 0; });
}

void ThreadPool::worker_loop() {
    for (;;) {
        std::function<void()> job;
        {
            std::unique_lock lock(mutex_);
            // The predicate form handles spurious wake-ups.
            work_available_.wait(lock, [this] { return stopping_ || !jobs_.empty(); });
            if (jobs_.empty()) {
                return; // stopping_ and fully drained
            }
            job = std::move(jobs_.front());
            jobs_.pop();
            ++active_;
        }

        job(); // packaged_task captures exceptions into the future; never throws

        {
            std::scoped_lock lock(mutex_);
            --active_;
            if (jobs_.empty() && active_ == 0) {
                idle_.notify_all();
            }
        }
    }
}

} // namespace sk
