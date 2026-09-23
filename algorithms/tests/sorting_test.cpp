#include <sk/algo/sorting.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <list>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace {

using Sorter = std::function<void(std::vector<int>&)>;

struct NamedSorter {
    std::string name;
    Sorter sort;
};

const std::vector<NamedSorter>& all_sorters() {
    static const std::vector<NamedSorter> sorters{
        {"Bubble", [](auto& v) { sk::algo::bubble_sort(v.begin(), v.end()); }},
        {"Insertion", [](auto& v) { sk::algo::insertion_sort(v.begin(), v.end()); }},
        {"Selection", [](auto& v) { sk::algo::selection_sort(v.begin(), v.end()); }},
        {"Merge", [](auto& v) { sk::algo::merge_sort(v.begin(), v.end()); }},
        {"Quick", [](auto& v) { sk::algo::quick_sort(v.begin(), v.end()); }},
    };
    return sorters;
}

std::vector<int> random_vector(std::size_t n, int max_value, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(-max_value, max_value);
    std::vector<int> v(n);
    std::generate(v.begin(), v.end(), [&] { return dist(rng); });
    return v;
}

class SortingTest : public ::testing::TestWithParam<NamedSorter> {
protected:
    void expect_sorts(std::vector<int> input) const {
        auto expected = input;
        std::sort(expected.begin(), expected.end());
        GetParam().sort(input);
        EXPECT_EQ(input, expected);
    }
};

TEST_P(SortingTest, EmptyAndSingleElement) {
    expect_sorts({});
    expect_sorts({42});
}

TEST_P(SortingTest, AlreadySortedAndReversed) {
    expect_sorts({1, 2, 3, 4, 5, 6, 7, 8});
    expect_sorts({8, 7, 6, 5, 4, 3, 2, 1});
}

TEST_P(SortingTest, DuplicatesAndNegatives) {
    expect_sorts({3, -1, 3, 0, -1, 3, 3, -7});
    expect_sorts(std::vector<int>(100, 5));
}

TEST_P(SortingTest, RandomInputsOfManySizes) {
    for (std::size_t n : {2u, 3u, 17u, 100u, 1000u}) {
        expect_sorts(random_vector(n, 50, static_cast<unsigned>(n)));
    }
}

INSTANTIATE_TEST_SUITE_P(AllAlgorithms, SortingTest, ::testing::ValuesIn(all_sorters()),
                         [](const auto& info) { return info.param.name; });

TEST(SortingExtras, CustomComparatorSortsDescending) {
    std::vector<int> v{3, 1, 2};
    sk::algo::quick_sort(v.begin(), v.end(), std::greater<>{});
    EXPECT_EQ(v, (std::vector<int>{3, 2, 1}));
}

TEST(SortingExtras, StableSortsKeepEqualKeysInOrder) {
    using Item = std::pair<int, char>;
    const std::vector<Item> input{{1, 'a'}, {0, 'b'}, {1, 'c'}, {0, 'd'}};
    const std::vector<Item> expected{{0, 'b'}, {0, 'd'}, {1, 'a'}, {1, 'c'}};
    auto by_key = [](const Item& l, const Item& r) {
        return l.first < r.first;
    };

    auto v = input;
    sk::algo::insertion_sort(v.begin(), v.end(), by_key);
    EXPECT_EQ(v, expected);
    v = input;
    sk::algo::merge_sort(v.begin(), v.end(), by_key);
    EXPECT_EQ(v, expected);
    v = input;
    sk::algo::bubble_sort(v.begin(), v.end(), by_key);
    EXPECT_EQ(v, expected);
}

TEST(SortingExtras, BidirectionalContainers) {
    std::list<int> l{5, 2, 4, 1};
    sk::algo::insertion_sort(l.begin(), l.end());
    EXPECT_EQ(l, (std::list<int>{1, 2, 4, 5}));
}

} // namespace
