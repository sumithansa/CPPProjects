#include <sk/algo/problems.hpp>

#include <algorithm>
#include <array>
#include <limits>
#include <unordered_set>
#include <utility>

namespace sk::algo {

std::int32_t reverse_integer(std::int32_t x) noexcept {
    constexpr std::int32_t kMax = std::numeric_limits<std::int32_t>::max();
    constexpr std::int32_t kMin = std::numeric_limits<std::int32_t>::min();

    std::int32_t result = 0;
    while (x != 0) {
        const std::int32_t digit = x % 10; // same sign as x
        x /= 10;
        // result * 10 + digit must stay within [kMin, kMax].
        if (result > kMax / 10 || (result == kMax / 10 && digit > kMax % 10)) {
            return 0;
        }
        if (result < kMin / 10 || (result == kMin / 10 && digit < kMin % 10)) {
            return 0;
        }
        result = result * 10 + digit;
    }
    return result;
}

bool is_palindrome(std::int32_t x) noexcept {
    // Negatives, and numbers ending in 0 (other than 0 itself), can't be palindromes.
    if (x < 0 || (x % 10 == 0 && x != 0)) {
        return false;
    }
    std::int32_t reversed_half = 0;
    while (x > reversed_half) {
        reversed_half = reversed_half * 10 + x % 10;
        x /= 10;
    }
    // Odd digit counts leave the middle digit in reversed_half; drop it.
    return x == reversed_half || x == reversed_half / 10;
}

std::size_t length_of_longest_unique_substring(std::string_view s) noexcept {
    std::array<std::size_t, 256> next_start{}; // index just past each byte's last occurrence
    std::size_t window_start = 0;
    std::size_t best = 0;
    for (std::size_t i = 0; i < s.size(); ++i) {
        const auto byte = static_cast<unsigned char>(s[i]);
        // If this byte was already seen inside the window, move the window past it.
        window_start = std::max(window_start, next_start[byte]);
        next_start[byte] = i + 1;
        best = std::max(best, i + 1 - window_start);
    }
    return best;
}

std::string longest_palindrome(std::string_view s) {
    if (s.empty()) {
        return {};
    }
    std::size_t best_start = 0;
    std::size_t best_len = 1;

    // Grows a palindrome outwards from [left, right] and records it if it is the longest.
    const auto expand = [&](std::size_t left, std::size_t right) {
        while (right < s.size() && s[left] == s[right]) {
            if (right - left + 1 > best_len) {
                best_start = left;
                best_len = right - left + 1;
            }
            if (left == 0) {
                break;
            }
            --left;
            ++right;
        }
    };

    for (std::size_t centre = 0; centre < s.size(); ++centre) {
        expand(centre, centre);     // odd length
        expand(centre, centre + 1); // even length
    }
    return std::string(s.substr(best_start, best_len));
}

std::optional<int> first_common_element(std::span<const int> a, std::span<const int> b) {
    if (a.size() > b.size()) {
        std::swap(a, b);
    }
    const std::unordered_set<int> seen(a.begin(), a.end());
    for (int value : b) {
        if (seen.contains(value)) {
            return value;
        }
    }
    return std::nullopt;
}

ListNode* add_two_numbers(const ListNode* a, const ListNode* b) {
    ListNode dummy;
    ListNode* tail = &dummy;
    int carry = 0;
    try {
        while (a != nullptr || b != nullptr || carry != 0) {
            int sum = carry;
            if (a != nullptr) {
                sum += a->val;
                a = a->next;
            }
            if (b != nullptr) {
                sum += b->val;
                b = b->next;
            }
            carry = sum / 10;
            tail->next = new ListNode{sum % 10, nullptr};
            tail = tail->next;
        }
    } catch (...) {
        free_list(dummy.next);
        throw;
    }
    return dummy.next;
}

bool has_cycle(const ListNode* head) noexcept {
    const ListNode* slow = head;
    const ListNode* fast = head;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            return true;
        }
    }
    return false;
}

void free_list(ListNode* head) noexcept {
    while (head != nullptr) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
}

} // namespace sk::algo
