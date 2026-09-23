#include <sk/shared_ptr.hpp>

#include <gtest/gtest.h>

#include <thread>
#include <utility>
#include <vector>

namespace {

struct Counted {
    static inline int live = 0;
    int value;
    explicit Counted(int v = 0) : value(v) { ++live; }
    virtual ~Counted() { --live; }
};

struct Derived : Counted {
    using Counted::Counted;
};

class SharedPtrTest : public ::testing::Test {
protected:
    void SetUp() override { Counted::live = 0; }
    void TearDown() override { EXPECT_EQ(Counted::live, 0); }
};

TEST_F(SharedPtrTest, CopiesShareOwnership) {
    auto a = sk::make_shared<Counted>(7);
    EXPECT_EQ(a.use_count(), 1);
    {
        sk::SharedPtr<Counted> b = a;
        sk::SharedPtr<Counted> c(b);
        EXPECT_EQ(a.use_count(), 3);
        EXPECT_EQ(c->value, 7);
    }
    EXPECT_EQ(a.use_count(), 1);
    EXPECT_EQ(Counted::live, 1);
}

TEST_F(SharedPtrTest, AssignmentReleasesPreviousObject) {
    // Regression: the original operator= overwrote the pointer without
    // releasing the old reference, leaking the previous object.
    auto a = sk::make_shared<Counted>(1);
    auto b = sk::make_shared<Counted>(2);
    b = a;
    EXPECT_EQ(Counted::live, 1);
    EXPECT_EQ(a.use_count(), 2);

    auto& self = b;
    b = self; // self-assignment must not drop the count to zero
    EXPECT_EQ(a.use_count(), 2);
}

TEST_F(SharedPtrTest, MoveDoesNotTouchCount) {
    sk::SharedPtr<Counted> a(new Counted(1));
    sk::SharedPtr<Counted> b(std::move(a));
    EXPECT_FALSE(a); // NOLINT(bugprone-use-after-move)
    EXPECT_EQ(b.use_count(), 1);
}

TEST_F(SharedPtrTest, ResetAndNull) {
    auto p = sk::make_shared<Counted>(1);
    p.reset(new Counted(2));
    EXPECT_EQ(p->value, 2);
    EXPECT_EQ(Counted::live, 1);
    p.reset();
    EXPECT_EQ(p.use_count(), 0);
    EXPECT_EQ(Counted::live, 0);
}

TEST_F(SharedPtrTest, DerivedConvertsToBase) {
    sk::SharedPtr<Counted> base = sk::make_shared<Derived>(9);
    EXPECT_EQ(base->value, 9);
    sk::SharedPtr<Counted> adopted(new Derived(3));
    EXPECT_EQ(adopted->value, 3);
}

TEST_F(SharedPtrTest, CustomDeleter) {
    bool deleted = false;
    {
        sk::SharedPtr<int> p(new int(1), [&deleted](int* ptr) {
            deleted = true;
            delete ptr;
        });
    }
    EXPECT_TRUE(deleted);
}

TEST_F(SharedPtrTest, WeakPtrObservesWithoutOwning) {
    sk::WeakPtr<Counted> weak;
    {
        auto strong = sk::make_shared<Counted>(4);
        weak = strong;
        EXPECT_EQ(weak.use_count(), 1);
        auto locked = weak.lock();
        ASSERT_TRUE(locked);
        EXPECT_EQ(locked->value, 4);
    }
    EXPECT_TRUE(weak.expired());
    EXPECT_FALSE(weak.lock());
    EXPECT_EQ(Counted::live, 0);
}

TEST_F(SharedPtrTest, WeakPtrBreaksReferenceCycle) {
    struct Node {
        sk::SharedPtr<Node> next;
        sk::WeakPtr<Node> prev;
        Counted tracker{0};
    };
    {
        auto first = sk::make_shared<Node>();
        auto second = sk::make_shared<Node>();
        first->next = second;
        second->prev = first; // a SharedPtr here would leak both nodes
    }
    EXPECT_EQ(Counted::live, 0);
}

TEST_F(SharedPtrTest, ConcurrentCopiesKeepCountConsistent) {
    auto shared = sk::make_shared<Counted>(1);
    constexpr int kThreads = 8;
    constexpr int kIterations = 10'000;

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; ++t) {
        threads.emplace_back([shared] { // each thread owns a copy
            for (int i = 0; i < kIterations; ++i) {
                sk::SharedPtr<Counted> local = shared;
                sk::WeakPtr<Counted> weak = local;
                (void)weak.lock();
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    EXPECT_EQ(shared.use_count(), 1);
}

} // namespace
