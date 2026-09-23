#include <sk/unique_ptr.hpp>

#include <gtest/gtest.h>

#include <utility>

namespace {

struct Counted {
    static inline int live = 0;
    int value;
    explicit Counted(int v = 0) : value(v) { ++live; }
    Counted(const Counted&) = delete;
    Counted& operator=(const Counted&) = delete;
    virtual ~Counted() { --live; }
};

struct DerivedCounted : Counted {
    using Counted::Counted;
};

class UniquePtrTest : public ::testing::Test {
protected:
    void SetUp() override { Counted::live = 0; }
    void TearDown() override { EXPECT_EQ(Counted::live, 0); }
};

TEST_F(UniquePtrTest, HasZeroOverheadWithStatelessDeleter) {
    static_assert(sizeof(sk::UniquePtr<int>) == sizeof(int*));
    static_assert(!std::is_copy_constructible_v<sk::UniquePtr<int>>);
    static_assert(std::is_nothrow_move_constructible_v<sk::UniquePtr<int>>);
}

TEST_F(UniquePtrTest, DestroysObjectAtEndOfScope) {
    {
        auto p = sk::make_unique<Counted>(42);
        EXPECT_EQ(p->value, 42);
        EXPECT_EQ((*p).value, 42);
        EXPECT_EQ(Counted::live, 1);
    }
    EXPECT_EQ(Counted::live, 0);
}

TEST_F(UniquePtrTest, MoveTransfersOwnership) {
    auto a = sk::make_unique<Counted>(1);
    Counted* raw = a.get();
    sk::UniquePtr<Counted> b(std::move(a));
    EXPECT_EQ(a, nullptr); // NOLINT(bugprone-use-after-move)
    EXPECT_EQ(b.get(), raw);

    auto c = sk::make_unique<Counted>(2);
    c = std::move(b); // old object in c is destroyed
    EXPECT_EQ(c.get(), raw);
    EXPECT_EQ(Counted::live, 1);
}

TEST_F(UniquePtrTest, ReleaseGivesUpOwnership) {
    auto p = sk::make_unique<Counted>(5);
    Counted* raw = p.release();
    EXPECT_FALSE(p);
    EXPECT_EQ(Counted::live, 1);
    delete raw;
}

TEST_F(UniquePtrTest, ResetReplacesManagedObject) {
    auto p = sk::make_unique<Counted>(1);
    p.reset(new Counted(2));
    EXPECT_EQ(p->value, 2);
    EXPECT_EQ(Counted::live, 1);
    p = nullptr;
    EXPECT_EQ(Counted::live, 0);
}

TEST_F(UniquePtrTest, ConvertsDerivedToBase) {
    sk::UniquePtr<Counted> base = sk::make_unique<DerivedCounted>(3);
    EXPECT_EQ(base->value, 3);
}

TEST_F(UniquePtrTest, CustomDeleterIsInvoked) {
    int calls = 0;
    auto deleter = [&calls](int* p) {
        ++calls;
        delete p;
    };
    {
        sk::UniquePtr<int, decltype(deleter)> p(new int(1), deleter);
    }
    EXPECT_EQ(calls, 1);
}

} // namespace
