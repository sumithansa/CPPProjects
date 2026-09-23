#include <sk/string.hpp>

#include <gtest/gtest.h>

#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

constexpr const char* kLong = "this string is longer than the inline buffer";

TEST(StringTest, DefaultIsEmptyAndNullTerminated) {
    sk::String s;
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.length(), 0u);
    EXPECT_STREQ(s.c_str(), "");
    EXPECT_TRUE(s.is_small());
}

TEST(StringTest, ShortStringsStayInline) {
    sk::String s("Hello");
    EXPECT_EQ(s.length(), 5u);
    EXPECT_TRUE(s.is_small());
    EXPECT_EQ(s, "Hello");
}

TEST(StringTest, BoundaryBetweenInlineAndHeap) {
    sk::String fits(std::string(sk::String::kSsoCapacity, 'x'));
    sk::String spills(std::string(sk::String::kSsoCapacity + 1, 'x'));
    EXPECT_TRUE(fits.is_small());
    EXPECT_FALSE(spills.is_small());
}

TEST(StringTest, CopyIsDeepAndIndependent) {
    sk::String a(kLong);
    sk::String b(a);
    EXPECT_EQ(a, b);
    EXPECT_NE(a.c_str(), b.c_str());
    b[0] = 'T';
    EXPECT_EQ(a[0], 't');
}

TEST(StringTest, MoveTransfersHeapBufferAndEmptiesSource) {
    sk::String a(kLong);
    const char* buffer = a.c_str();
    sk::String b(std::move(a));
    EXPECT_EQ(b.c_str(), buffer);
    EXPECT_TRUE(a.empty()); // NOLINT(bugprone-use-after-move)
    EXPECT_STREQ(a.c_str(), "");
}

TEST(StringTest, MoveOfInlineStringCopiesBytes) {
    sk::String a("short");
    sk::String b(std::move(a));
    EXPECT_EQ(b, "short");
    EXPECT_TRUE(b.is_small());
}

TEST(StringTest, AssignmentHandlesEveryStorageCombination) {
    sk::String small("abc");
    sk::String large(kLong);

    sk::String s;
    s = large;
    EXPECT_EQ(s, kLong);
    s = small; // reuses existing heap capacity
    EXPECT_EQ(s, "abc");
    s = std::move(large);
    EXPECT_EQ(s, kLong);

    auto& self = s;
    s = self;
    EXPECT_EQ(s, kLong);
}

TEST(StringTest, AssignFromOwnSubstringIsSafe) {
    sk::String s(kLong);
    s.assign(s.view().substr(5, 6));
    EXPECT_EQ(s, "string");
}

TEST(StringTest, AppendGrowsAndHandlesSelfAliasing) {
    sk::String s("ab");
    for (int i = 0; i < 5; ++i) {
        s.append(s); // doubles; the argument points into the buffer being replaced
    }
    EXPECT_EQ(s.size(), 64u);
    EXPECT_EQ(s.substr(0, 4), "abab");
}

TEST(StringTest, ConcatenationOperators) {
    sk::String s = sk::String("Hello") + ", " + "World";
    s += "!";
    s.push_back('?');
    EXPECT_EQ(s, "Hello, World!?");
}

TEST(StringTest, SubstrClampsCountAndValidatesPosition) {
    const sk::String s("Hello, World!");
    EXPECT_EQ(s.substr(7, 5), "World");
    EXPECT_EQ(s.substr(7), "World!");
    EXPECT_EQ(s.substr(13), "");
    EXPECT_THROW((void)s.substr(14), std::out_of_range);
}

TEST(StringTest, ComparisonAndOrdering) {
    const sk::String apple("apple");
    const sk::String banana("banana");
    EXPECT_LT(apple, banana);
    EXPECT_GT(banana, "apple");
    EXPECT_EQ(apple.compare("apple"), 0);
    EXPECT_NE(apple, banana);
}

TEST(StringTest, IteratorsAndFind) {
    sk::String s("Hello");
    EXPECT_EQ(*s.begin(), 'H');
    EXPECT_EQ(*s.end(), '\0'); // terminator is always present
    EXPECT_EQ(s.find("llo"), 2u);
    EXPECT_EQ(s.find("xyz"), sk::String::npos);
}

TEST(StringTest, ClearKeepsCapacity) {
    sk::String s(kLong);
    const auto capacity = s.capacity();
    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.capacity(), capacity);
}

TEST(StringTest, SwapMixedStorage) {
    sk::String a("tiny");
    sk::String b(kLong);
    swap(a, b);
    EXPECT_EQ(a, kLong);
    EXPECT_EQ(b, "tiny");
}

TEST(StringTest, StreamsLikeStdString) {
    std::ostringstream os;
    os << sk::String("streamed");
    EXPECT_EQ(os.str(), "streamed");
}

TEST(StringTest, AtChecksBounds) {
    sk::String s("abc");
    EXPECT_EQ(s.at(2), 'c');
    EXPECT_THROW((void)s.at(3), std::out_of_range);
}

} // namespace
