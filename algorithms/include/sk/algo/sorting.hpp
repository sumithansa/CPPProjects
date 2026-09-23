#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>

/// Classic comparison sorts written as generic, iterator-based algorithms in
/// the style of <algorithm>: they sort in place, accept any comparator and
/// work on any container with suitable iterators.
///
/// | Algorithm      | Best     | Average    | Worst      | Stable | Notes                     |
/// |----------------|----------|------------|------------|--------|---------------------------|
/// | bubble_sort    | O(n)     | O(n^2)     | O(n^2)     | yes    | early exit on sorted pass |
/// | insertion_sort | O(n)     | O(n^2)     | O(n^2)     | yes    | fast for small/nearly sorted
/// input | | selection_sort | O(n^2)   | O(n^2)     | O(n^2)     | no     | at most n-1 swaps | |
/// merge_sort     | O(n lg n)| O(n lg n)  | O(n lg n)  | yes    | uses std::inplace_merge   | |
/// quick_sort     | O(n lg n)| O(n lg n)  | O(n^2)     | no     | median-of-three pivot     |
namespace sk::algo {

template <std::random_access_iterator It, typename Compare = std::less<>>
void bubble_sort(It first, It last, Compare comp = {}) {
    for (auto end = last; end - first > 1; --end) {
        bool swapped = false;
        for (auto it = first; it + 1 != end; ++it) {
            if (comp(*(it + 1), *it)) {
                std::iter_swap(it, it + 1);
                swapped = true;
            }
        }
        if (!swapped) {
            return; // no swaps in a full pass: already sorted
        }
    }
}

template <std::bidirectional_iterator It, typename Compare = std::less<>>
void insertion_sort(It first, It last, Compare comp = {}) {
    if (first == last) {
        return;
    }
    for (auto it = std::next(first); it != last; ++it) {
        auto value = std::move(*it);
        auto hole = it;
        for (auto prev = std::prev(hole); comp(value, *prev); --prev) {
            *hole = std::move(*prev);
            hole = prev;
            if (prev == first) {
                break;
            }
        }
        *hole = std::move(value);
    }
}

template <std::forward_iterator It, typename Compare = std::less<>>
void selection_sort(It first, It last, Compare comp = {}) {
    for (; first != last; ++first) {
        auto smallest = std::min_element(first, last, comp);
        if (smallest != first) {
            std::iter_swap(first, smallest);
        }
    }
}

template <std::bidirectional_iterator It, typename Compare = std::less<>>
void merge_sort(It first, It last, Compare comp = {}) {
    const auto size = std::distance(first, last);
    if (size < 2) {
        return;
    }
    const auto middle = std::next(first, size / 2);
    merge_sort(first, middle, comp);
    merge_sort(middle, last, comp);
    std::inplace_merge(first, middle, last, comp);
}

template <std::random_access_iterator It, typename Compare = std::less<>>
void quick_sort(It first, It last, Compare comp = {}) {
    while (last - first > 16) {
        // Median-of-three avoids the O(n^2) worst case on already-sorted input.
        auto mid = first + (last - first) / 2;
        auto back = last - 1;
        if (comp(*mid, *first)) {
            std::iter_swap(mid, first);
        }
        if (comp(*back, *first)) {
            std::iter_swap(back, first);
        }
        if (comp(*back, *mid)) {
            std::iter_swap(back, mid);
        }
        const auto pivot = *mid;

        // Three-way partition: [< pivot][== pivot][> pivot] handles duplicates well.
        auto lower = std::partition(first, last, [&](const auto& x) { return comp(x, pivot); });
        auto upper = std::partition(lower, last, [&](const auto& x) { return !comp(pivot, x); });

        // Recurse into the smaller half, loop on the larger: O(log n) stack depth.
        if (lower - first < last - upper) {
            quick_sort(first, lower, comp);
            first = upper;
        } else {
            quick_sort(upper, last, comp);
            last = lower;
        }
    }
    insertion_sort(first, last, comp); // small ranges: insertion sort wins
}

} // namespace sk::algo
