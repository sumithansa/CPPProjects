#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>

/// Solutions to common interview problems, each with its complexity stated.
namespace sk::algo {

/// Reverses the decimal digits of x, returning 0 if the result would overflow
/// int32_t. Overflow is detected *before* it happens, without 64-bit math.
/// O(log10 |x|) time, O(1) space.
std::int32_t reverse_integer(std::int32_t x) noexcept;

/// True if x reads the same backwards. Reverses only half the digits, so it
/// cannot overflow. O(log10 x) time, O(1) space.
bool is_palindrome(std::int32_t x) noexcept;

/// Length of the longest substring without repeating characters.
/// Sliding window over the last-seen index of each byte: O(n) time, O(1) space.
std::size_t length_of_longest_unique_substring(std::string_view s) noexcept;

/// Longest palindromic substring (first one found on ties).
/// Expand-around-centre: O(n^2) time, O(1) extra space.
std::string longest_palindrome(std::string_view s);

/// Any element present in both inputs, or std::nullopt.
/// Hash set of the smaller input: O(n + m) expected time.
std::optional<int> first_common_element(std::span<const int> a, std::span<const int> b);

/// Minimal LeetCode-style list node (raw pointers, to match the problem statement).
struct ListNode {
    int val = 0;
    ListNode* next = nullptr;
};

/// Adds two non-negative numbers stored as reversed digit lists.
/// Digit-by-digit with carry, so lists of any length work (converting to an
/// int first overflows beyond 10 digits). O(max(n, m)) time.
/// The caller owns the returned list (see free_list).
ListNode* add_two_numbers(const ListNode* a, const ListNode* b);

/// Floyd's tortoise-and-hare cycle detection: O(n) time, O(1) space.
bool has_cycle(const ListNode* head) noexcept;

/// Deletes an acyclic list built with new.
void free_list(ListNode* head) noexcept;

} // namespace sk::algo
