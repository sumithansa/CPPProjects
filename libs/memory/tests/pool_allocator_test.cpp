#include <sk/pool_allocator.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <set>
#include <stdexcept>
#include <vector>

namespace {

TEST(FixedBlockPoolTest, HandsOutDistinctAlignedBlocksUntilExhausted) {
    sk::FixedBlockPool pool(24, 4);
    EXPECT_EQ(pool.block_size() % alignof(std::max_align_t), 0u);

    std::set<void*> blocks;
    for (int i = 0; i < 4; ++i) {
        void* block = pool.allocate();
        ASSERT_NE(block, nullptr);
        EXPECT_TRUE(pool.owns(block));
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(block) % alignof(std::max_align_t), 0u);
        blocks.insert(block);
    }
    EXPECT_EQ(blocks.size(), 4u);
    EXPECT_EQ(pool.allocate(), nullptr);
    EXPECT_EQ(pool.free_blocks(), 0u);

    for (void* block : blocks) {
        pool.deallocate(block);
    }
    EXPECT_EQ(pool.free_blocks(), 4u);
}

TEST(FixedBlockPoolTest, ReusesMostRecentlyFreedBlock) {
    sk::FixedBlockPool pool(16, 2);
    void* a = pool.allocate();
    pool.deallocate(a);
    EXPECT_EQ(pool.allocate(), a); // LIFO: the hot block is still in cache
}

TEST(FixedBlockPoolTest, TinyBlocksAreRoundedUpToHoldFreeListPointer) {
    sk::FixedBlockPool pool(1, 3);
    EXPECT_GE(pool.block_size(), sizeof(void*));
}

struct Widget {
    static inline int live = 0;
    int id;
    explicit Widget(int i) : id(i) {
        if (i < 0) {
            throw std::invalid_argument("negative id");
        }
        ++live;
    }
    ~Widget() { --live; }
};

TEST(ObjectPoolTest, ConstructsAndDestroysObjects) {
    sk::ObjectPool<Widget> pool(8);
    std::vector<Widget*> widgets;
    for (int i = 0; i < 8; ++i) {
        widgets.push_back(pool.create(i));
    }
    EXPECT_EQ(Widget::live, 8);
    EXPECT_EQ(pool.available(), 0u);
    EXPECT_THROW((void)pool.create(99), std::bad_alloc);

    for (Widget* w : widgets) {
        pool.destroy(w);
    }
    EXPECT_EQ(Widget::live, 0);
    EXPECT_EQ(pool.available(), 8u);
}

TEST(ObjectPoolTest, ReturnsBlockWhenConstructorThrows) {
    sk::ObjectPool<Widget> pool(1);
    EXPECT_THROW((void)pool.create(-1), std::invalid_argument);
    EXPECT_EQ(pool.available(), 1u);
}

} // namespace
