#include <sk/algo/problems.hpp>

#include <gtest/gtest.h>

#include <initializer_list>
#include <limits>
#include <vector>

namespace {

using sk::algo::ListNode;

ListNode* make_list(std::initializer_list<int> digits) {
    ListNode dummy;
    ListNode* tail = &dummy;
    for (int d : digits) {
        tail->next = new ListNode{d, nullptr};
        tail = tail->next;
    }
    return dummy.next;
}

std::vector<int> to_vector(const ListNode* node) {
    std::vector<int> out;
    for (; node != nullptr; node = node->next) {
        out.push_back(node->val);
    }
    return out;
}

TEST(ReverseIntegerTest, ReversesDigitsKeepingSign) {
    EXPECT_EQ(sk::algo::reverse_integer(123), 321);
    EXPECT_EQ(sk::algo::reverse_integer(-123), -321);
    EXPECT_EQ(sk::algo::reverse_integer(120), 21);
    EXPECT_EQ(sk::algo::reverse_integer(0), 0);
}

TEST(ReverseIntegerTest, ReturnsZeroOnOverflow) {
    constexpr auto kMax = std::numeric_limits<std::int32_t>::max();
    constexpr auto kMin = std::numeric_limits<std::int32_t>::min();
    EXPECT_EQ(sk::algo::reverse_integer(kMax), 0);
    EXPECT_EQ(sk::algo::reverse_integer(kMin), 0); // -kMin is not representable
    EXPECT_EQ(sk::algo::reverse_integer(1'534'236'469), 0);
    EXPECT_EQ(sk::algo::reverse_integer(1'463'847'412), 2'147'483'641); // just fits
    EXPECT_EQ(sk::algo::reverse_integer(-1'463'847'412), -2'147'483'641);
}

TEST(PalindromeNumberTest, Cases) {
    EXPECT_TRUE(sk::algo::is_palindrome(0));
    EXPECT_TRUE(sk::algo::is_palindrome(1221));
    EXPECT_TRUE(sk::algo::is_palindrome(12321));
    EXPECT_FALSE(sk::algo::is_palindrome(-121));
    EXPECT_FALSE(sk::algo::is_palindrome(10));
    EXPECT_FALSE(sk::algo::is_palindrome(123));
    EXPECT_FALSE(sk::algo::is_palindrome(std::numeric_limits<std::int32_t>::max()));
}

TEST(LongestUniqueSubstringTest, Cases) {
    EXPECT_EQ(sk::algo::length_of_longest_unique_substring(""), 0u);
    EXPECT_EQ(sk::algo::length_of_longest_unique_substring("bbbbb"), 1u);
    EXPECT_EQ(sk::algo::length_of_longest_unique_substring("abcabcbb"), 3u);
    EXPECT_EQ(sk::algo::length_of_longest_unique_substring("pwwkew"), 3u);
    EXPECT_EQ(sk::algo::length_of_longest_unique_substring("abba"), 2u);
    EXPECT_EQ(sk::algo::length_of_longest_unique_substring("dvdf"), 3u);
}

TEST(LongestPalindromeTest, Cases) {
    EXPECT_EQ(sk::algo::longest_palindrome(""), "");
    EXPECT_EQ(sk::algo::longest_palindrome("a"), "a");
    EXPECT_EQ(sk::algo::longest_palindrome("babad"), "bab");
    EXPECT_EQ(sk::algo::longest_palindrome("cbbd"), "bb");
    EXPECT_EQ(sk::algo::longest_palindrome("ac"), "a");
    EXPECT_EQ(sk::algo::longest_palindrome("abcccccc"), "cccccc");
    EXPECT_EQ(sk::algo::longest_palindrome("forgeeksskeegfor"), "geeksskeeg");
}

TEST(CommonElementTest, Cases) {
    const std::vector<int> a{2, 4, 5};
    const std::vector<int> b{1, 3, 5};
    const std::vector<int> c{7, 8};
    EXPECT_EQ(sk::algo::first_common_element(a, b), 5);
    EXPECT_EQ(sk::algo::first_common_element(a, c), std::nullopt);
    // Regression: the original returned 0 for "not found", which is ambiguous
    // when 0 is a legitimate common value.
    const std::vector<int> zero{0};
    EXPECT_EQ(sk::algo::first_common_element(zero, zero), 0);
}

TEST(AddTwoNumbersTest, CarriesAcrossDifferentLengths) {
    ListNode* a = make_list({9, 9, 9, 9});
    ListNode* b = make_list({9, 9, 9, 9, 9, 9, 9});
    ListNode* sum = sk::algo::add_two_numbers(a, b);
    EXPECT_EQ(to_vector(sum), (std::vector<int>{8, 9, 9, 9, 0, 0, 0, 1}));
    sk::algo::free_list(a);
    sk::algo::free_list(b);
    sk::algo::free_list(sum);
}

TEST(AddTwoNumbersTest, NumbersLongerThanAnyIntegerType) {
    // 30 digits: the original "convert to int, add, convert back" approach overflows here.
    ListNode* a = make_list(
        {9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9});
    ListNode* b = make_list({1});
    ListNode* sum = sk::algo::add_two_numbers(a, b);
    auto digits = to_vector(sum);
    ASSERT_EQ(digits.size(), 31u);
    EXPECT_EQ(digits.back(), 1);
    sk::algo::free_list(a);
    sk::algo::free_list(b);
    sk::algo::free_list(sum);
}

TEST(HasCycleTest, DetectsCycles) {
    std::vector<ListNode> nodes(5);
    for (std::size_t i = 0; i + 1 < nodes.size(); ++i) {
        nodes[i].next = &nodes[i + 1];
    }
    EXPECT_FALSE(sk::algo::has_cycle(nodes.data()));
    nodes.back().next = &nodes[1];
    EXPECT_TRUE(sk::algo::has_cycle(nodes.data()));

    ListNode self;
    self.next = &self;
    EXPECT_TRUE(sk::algo::has_cycle(&self));
    EXPECT_FALSE(sk::algo::has_cycle(nullptr));
}

} // namespace
