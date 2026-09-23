#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace sk {

/// Fixed-size pool of worker threads that execute submitted tasks in FIFO order.
///
///  - submit() returns a std::future, so results and exceptions propagate to
///    the caller.
///  - Destruction is graceful: already-queued tasks are drained before the
///    workers are joined. Submitting after shutdown has begun throws.
class ThreadPool {
public:
    explicit ThreadPool(std::size_t thread_count = default_thread_count());

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool();

    /// Arguments are decay-copied into the task and passed as rvalues, the
    /// same semantics as std::async / std::thread.
    template <typename F, typename... Args>
    [[nodiscard]] auto submit(F&& func, Args&&... args)
        -> std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>> {
        using Result = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

        // packaged_task is move-only but std::function needs copyable
        // callables, so the task lives behind a shared_ptr.
        auto task = std::make_shared<std::packaged_task<Result()>>(
            [f = std::forward<F>(func), ... a = std::forward<Args>(args)]() mutable {
                return std::invoke(std::move(f), std::move(a)...);
            });
        std::future<Result> result = task->get_future();
        enqueue([task] { (*task)(); });
        return result;
    }

    /// Blocks until every task submitted so far has finished.
    void wait_idle();

    std::size_t thread_count() const noexcept { return workers_.size(); }

    static std::size_t default_thread_count() noexcept;

private:
    void enqueue(std::function<void()> job);
    void worker_loop();

    std::mutex mutex_;
    std::condition_variable work_available_;
    std::condition_variable idle_;
    std::queue<std::function<void()>> jobs_;
    std::size_t active_ = 0;
    bool stopping_ = false;
    std::vector<std::thread> workers_;
};

} // namespace sk
