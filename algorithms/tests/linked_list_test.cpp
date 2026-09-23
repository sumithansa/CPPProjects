#include <sk/algo/linked_list.hpp>

#include <gtest/gtest.h>

#include <vector>

namespace {

template <typename T>
std::vector<T> to_vector(const sk::algo::SinglyLinkedList<T>& list) {
    return {list.begin(), list.end()};
}

TEST(LinkedListTest, PushFrontAndBack) {
    sk::algo::SinglyLinkedList<int> list;
    list.push_back(2);
    list.push_front(1);
    list.push_back(3);
    EXPECT_EQ(to_vector(list), (std::vector<int>{1, 2, 3}));
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.back(), 3);
    EXPECT_EQ(list.size(), 3u);
}

TEST(LinkedListTest, PopFrontUpdatesTailWhenEmptied) {
    sk::algo::SinglyLinkedList<int> list{1};
    list.pop_front();
    EXPECT_TRUE(list.empty());
    list.push_back(7); // would crash if tail_ still pointed at the freed node
    EXPECT_EQ(list.front(), 7);
    EXPECT_EQ(list.back(), 7);
}

TEST(LinkedListTest, ReverseUpdatesHeadAndTail) {
    sk::algo::SinglyLinkedList<int> list{10, 2, 4, 3, 13};
    list.reverse();
    EXPECT_EQ(to_vector(list), (std::vector<int>{13, 3, 4, 2, 10}));
    EXPECT_EQ(list.back(), 10);
    list.push_back(99);
    EXPECT_EQ(list.back(), 99);

    sk::algo::SinglyLinkedList<int> empty;
    empty.reverse();
    EXPECT_TRUE(empty.empty());
}

TEST(LinkedListTest, MiddleElement) {
    EXPECT_EQ(sk::algo::SinglyLinkedList<int>{}.middle(), nullptr);
    EXPECT_EQ(*(sk::algo::SinglyLinkedList<int>{1}.middle()), 1);
    EXPECT_EQ(*(sk::algo::SinglyLinkedList<int>{1, 2, 3}.middle()), 2);
    EXPECT_EQ(*(sk::algo::SinglyLinkedList<int>{1, 2, 3, 4}.middle()), 3);
}

TEST(LinkedListTest, CopyIsDeepMoveIsCheap) {
    sk::algo::SinglyLinkedList<int> a{1, 2, 3};
    auto b = a;
    b.push_back(4);
    EXPECT_EQ(a.size(), 3u);

    auto c = std::move(b);
    EXPECT_EQ(c.size(), 4u);
    EXPECT_TRUE(b.empty()); // NOLINT(bugprone-use-after-move)
}

TEST(LinkedListTest, DestroyingAVeryLongListDoesNotOverflowTheStack) {
    sk::algo::SinglyLinkedList<int> list;
    for (int i = 0; i < 1'000'000; ++i) {
        list.push_front(i);
    }
    EXPECT_EQ(list.size(), 1'000'000u);
}

} // namespace
