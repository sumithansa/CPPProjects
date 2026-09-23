#include <sk/singleton.hpp>

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <type_traits>
#include <vector>

namespace {

class SlowToBuild : public sk::Singleton<SlowToBuild> {
public:
    static inline std::atomic<int> constructions{0};
    int value = 0;

private:
    friend class sk::Singleton<SlowToBuild>;
    SlowToBuild() {
        // Widen the window in which a broken implementation would construct twice.
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        constructions.fetch_add(1);
        value = 42;
    }
};

TEST(SingletonTest, ConcurrentFirstAccessConstructsExactlyOnce) {
    constexpr int kThreads = 16;
    std::vector<SlowToBuild*> seen(kThreads);
    std::vector<std::thread> threads;
    for (int i = 0; i < kThreads; ++i) {
        threads.emplace_back(
            [&seen, i] { seen[static_cast<std::size_t>(i)] = &SlowToBuild::instance(); });
    }
    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(SlowToBuild::constructions.load(), 1);
    for (SlowToBuild* p : seen) {
        EXPECT_EQ(p, seen.front());
        EXPECT_EQ(p->value, 42); // fully constructed object is visible to every thread
    }
}

TEST(SingletonTest, IsNeitherCopyableNorMovable) {
    static_assert(!std::is_copy_constructible_v<SlowToBuild>);
    static_assert(!std::is_move_constructible_v<SlowToBuild>);
    static_assert(!std::is_default_constructible_v<SlowToBuild>);
}

} // namespace
