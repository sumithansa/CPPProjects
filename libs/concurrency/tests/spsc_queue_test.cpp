#include <sk/spsc_queue.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <string>
#include <thread>

namespace {

TEST(SpscQueueTest, CapacityRoundsUpToPowerOfTwo) {
    EXPECT_EQ(sk::SpscQueue<int>(5).capacity(), 8u);
    EXPECT_EQ(sk::SpscQueue<int>(8).capacity(), 8u);
    EXPECT_EQ(sk::SpscQueue<int>(0).capacity(), 2u);
}

TEST(SpscQueueTest, FifoOrderAndFullEmptyDetection) {
    sk::SpscQueue<int> q(4);
    EXPECT_FALSE(q.try_pop().has_value());
    for (int i = 0; i < 4; ++i) {
        EXPECT_TRUE(q.try_push(i));
    }
    EXPECT_FALSE(q.try_push(99)) << "queue should be full";
    for (int i = 0; i < 4; ++i) {
        auto v = q.try_pop();
        ASSERT_TRUE(v.has_value());
        EXPECT_EQ(*v, i);
    }
    EXPECT_TRUE(q.empty());
}

TEST(SpscQueueTest, WrapsAroundManyTimes) {
    sk::SpscQueue<std::string> q(2);
    for (int i = 0; i < 1000; ++i) {
        ASSERT_TRUE(q.try_emplace(std::to_string(i)));
        ASSERT_EQ(q.try_pop().value(), std::to_string(i));
    }
}

TEST(SpscQueueTest, DestructorDestroysRemainingElements) {
    auto tracker = std::make_shared<int>(0);
    {
        sk::SpscQueue<std::shared_ptr<int>> q(4);
        q.try_push(tracker);
        q.try_push(tracker);
        EXPECT_EQ(tracker.use_count(), 3);
    }
    EXPECT_EQ(tracker.use_count(), 1);
}

TEST(SpscQueueTest, ProducerConsumerTransferEveryItemInOrder) {
    // Run under ThreadSanitizer in CI: proves the acquire/release pairs are sufficient.
    constexpr std::uint64_t kItems = 1'000'000;
    sk::SpscQueue<std::uint64_t> q(1024);

    std::thread producer([&] {
        for (std::uint64_t i = 0; i < kItems; ++i) {
            while (!q.try_push(i)) {
                std::this_thread::yield();
            }
        }
    });

    std::uint64_t expected = 0;
    std::uint64_t sum = 0;
    while (expected < kItems) {
        if (auto v = q.try_pop()) {
            ASSERT_EQ(*v, expected);
            sum += *v;
            ++expected;
        } else {
            std::this_thread::yield();
        }
    }
    producer.join();
    EXPECT_EQ(sum, kItems * (kItems - 1) / 2);
}

} // namespace
