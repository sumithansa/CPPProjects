#include <sk/vector.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

// Tracks live instances so tests can prove every constructed object is
// destroyed exactly once, and can be told to throw on the Nth copy.
struct Tracked {
    static inline int live = 0;
    static inline int copies_until_throw = -1;

    int value = 0;

    explicit Tracked(int v = 0) : value(v) { ++live; }
    Tracked(const Tracked& other) : value(other.value) {
        if (copies_until_throw == 0) {
            throw std::runtime_error("copy failed");
        }
        --copies_until_throw;
        ++live;
    }
    // Deliberately not noexcept: forces sk::Vector to copy on reallocation.
    // NOLINTNEXTLINE(performance-noexcept-move-constructor,cppcoreguidelines-noexcept-move-operations,performance-move-constructor-init)
    Tracked(Tracked&& other) : Tracked(std::as_const(other)) {}
    Tracked& operator=(const Tracked&) = default;
    Tracked& operator=(Tracked&&) = default;
    ~Tracked() { --live; }
};

class VectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        Tracked::live = 0;
        Tracked::copies_until_throw = -1;
    }
    void TearDown() override { EXPECT_EQ(Tracked::live, 0) << "leaked or double-destroyed"; }
};

TEST_F(VectorTest, DefaultConstructedIsEmptyAndDoesNotAllocate) {
    sk::Vector<int> v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0u);
    EXPECT_EQ(v.capacity(), 0u);
    EXPECT_EQ(v.data(), nullptr);
}

TEST_F(VectorTest, PushBackGrowsGeometrically) {
    sk::Vector<int> v;
    for (int i = 0; i < 100; ++i) {
        v.push_back(i);
    }
    ASSERT_EQ(v.size(), 100u);
    EXPECT_EQ(v.capacity(), 128u);
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(v[static_cast<std::size_t>(i)], i);
    }
}

TEST_F(VectorTest, CopyConstructorCopiesEveryElement) {
    // Regression: the original implementation copied only the first element.
    sk::Vector<std::string> a{"alpha", "beta", "gamma"};
    sk::Vector<std::string> b(a);
    EXPECT_EQ(a, b);
    b[1] = "changed";
    EXPECT_EQ(a[1], "beta");
}

TEST_F(VectorTest, MoveConstructorStealsBuffer) {
    sk::Vector<int> a{1, 2, 3};
    const int* buffer = a.data();
    sk::Vector<int> b(std::move(a));
    EXPECT_EQ(b.data(), buffer);
    EXPECT_EQ(b.size(), 3u);
    EXPECT_TRUE(a.empty()); // NOLINT(bugprone-use-after-move): moved-from state is specified
}

TEST_F(VectorTest, CopyAndMoveAssignment) {
    sk::Vector<int> a{1, 2, 3};
    sk::Vector<int> b{9};
    b = a;
    EXPECT_EQ(a, b);

    sk::Vector<int> c;
    c = std::move(b);
    EXPECT_EQ(c, a);

    auto& self = c;
    c = self; // self-assignment must be a no-op
    EXPECT_EQ(c, a);
}

TEST_F(VectorTest, PushBackOfOwnElementDuringReallocation) {
    sk::Vector<std::string> v{"a-long-string-that-defeats-sso"};
    ASSERT_EQ(v.size(), v.capacity());
    v.push_back(v[0]); // argument aliases the buffer being reallocated
    EXPECT_EQ(v[1], "a-long-string-that-defeats-sso");
}

TEST_F(VectorTest, ReallocationGivesStrongGuaranteeWhenCopyThrows) {
    sk::Vector<Tracked> v;
    v.reserve(2);
    v.emplace_back(1);
    v.emplace_back(2);

    Tracked::copies_until_throw = 1; // second element copy during growth fails
    EXPECT_THROW(v.emplace_back(3), std::runtime_error);

    ASSERT_EQ(v.size(), 2u);
    EXPECT_EQ(v.capacity(), 2u);
    EXPECT_EQ(v[0].value, 1);
    EXPECT_EQ(v[1].value, 2);
    Tracked::copies_until_throw = -1;
}

TEST_F(VectorTest, MoveOnlyTypesAreSupported) {
    sk::Vector<std::unique_ptr<int>> v;
    for (int i = 0; i < 10; ++i) {
        v.push_back(std::make_unique<int>(i));
    }
    EXPECT_EQ(*v[9], 9);
}

TEST_F(VectorTest, EraseShiftsElementsAndDestroysTail) {
    sk::Vector<Tracked> v;
    for (int i = 0; i < 5; ++i) {
        v.emplace_back(i);
    }
    auto* it = v.erase(v.begin() + 1, v.begin() + 3);
    EXPECT_EQ(it->value, 3);
    ASSERT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0].value, 0);
    EXPECT_EQ(v[2].value, 4);
    EXPECT_EQ(Tracked::live, 3);
}

TEST_F(VectorTest, ResizeValueInitialisesNewElements) {
    sk::Vector<int> v{7};
    v.resize(4);
    EXPECT_EQ(v, (sk::Vector<int>{7, 0, 0, 0}));
    v.resize(1);
    EXPECT_EQ(v, (sk::Vector<int>{7}));
}

TEST_F(VectorTest, ShrinkToFitReleasesSpareCapacity) {
    sk::Vector<int> v;
    v.reserve(64);
    v.push_back(1);
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 1u);
    v.clear();
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 0u);
}

TEST_F(VectorTest, AtThrowsOutOfRange) {
    const sk::Vector<int> v{1};
    EXPECT_EQ(v.at(0), 1);
    EXPECT_THROW((void)v.at(1), std::out_of_range);
}

TEST_F(VectorTest, WorksWithStandardAlgorithms) {
    sk::Vector<int> v{5, 3, 9, 1};
    std::sort(v.begin(), v.end());
    EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
    EXPECT_EQ(std::accumulate(v.begin(), v.end(), 0), 18);
    EXPECT_EQ(*v.rbegin(), 9);
    static_assert(std::contiguous_iterator<sk::Vector<int>::iterator>);
}

} // namespace
