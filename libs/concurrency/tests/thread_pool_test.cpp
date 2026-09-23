#include <sk/thread_pool.hpp>

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace {

TEST(ThreadPoolTest, ReturnsResultsThroughFutures) {
    sk::ThreadPool pool(4);
    std::vector<std::future<int>> results;
    for (int i = 0; i < 100; ++i) {
        results.push_back(pool.submit([](int x) { return x * x; }, i));
    }
    int sum = 0;
    for (auto& f : results) {
        sum += f.get();
    }
    EXPECT_EQ(sum, 328'350); // sum of squares 0..99
}

TEST(ThreadPoolTest, PropagatesExceptions) {
    sk::ThreadPool pool(2);
    auto f = pool.submit([]() -> int { throw std::logic_error("boom"); });
    EXPECT_THROW(f.get(), std::logic_error);
}

TEST(ThreadPoolTest, AcceptsMoveOnlyArguments) {
    sk::ThreadPool pool(1);
    auto f = pool.submit([](std::unique_ptr<int> p) { return *p + 1; }, std::make_unique<int>(41));
    EXPECT_EQ(f.get(), 42);
}

TEST(ThreadPoolTest, DestructorDrainsQueuedTasks) {
    std::atomic<int> completed{0};
    {
        sk::ThreadPool pool(2);
        for (int i = 0; i < 50; ++i) {
            (void)pool.submit([&completed] {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                completed.fetch_add(1, std::memory_order_relaxed);
            });
        }
    }
    EXPECT_EQ(completed.load(), 50);
}

TEST(ThreadPoolTest, WaitIdleBlocksUntilAllWorkIsDone) {
    sk::ThreadPool pool(4);
    std::atomic<int> counter{0};
    for (int i = 0; i < 1000; ++i) {
        (void)pool.submit([&counter] { counter.fetch_add(1, std::memory_order_relaxed); });
    }
    pool.wait_idle();
    EXPECT_EQ(counter.load(), 1000);
}

TEST(ThreadPoolTest, ZeroThreadsIsClampedToOne) {
    sk::ThreadPool pool(0);
    EXPECT_EQ(pool.thread_count(), 1u);
    EXPECT_EQ(pool.submit([] { return 7; }).get(), 7);
}

} // namespace
