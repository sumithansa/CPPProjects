#pragma once

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <utility>

namespace sk::algo {

/// Singly linked list with O(1) push_front, push_back and pop_front.
///
/// Nodes are owned through std::unique_ptr, so there is no manual delete.
/// The destructor unlinks nodes iteratively: letting the unique_ptr chain
/// destroy itself would recurse once per node and overflow the stack on
/// long lists.
template <typename T>
class SinglyLinkedList {
    struct Node {
        T value;
        std::unique_ptr<Node> next;
    };

public:
    class ConstIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        ConstIterator() = default;
        explicit ConstIterator(const Node* node) : node_(node) {}

        reference operator*() const { return node_->value; }
        pointer operator->() const { return &node_->value; }
        ConstIterator& operator++() {
            node_ = node_->next.get();
            return *this;
        }
        ConstIterator operator++(int) {
            auto copy = *this;
            ++*this;
            return copy;
        }
        bool operator==(const ConstIterator&) const = default;

    private:
        const Node* node_ = nullptr;
    };

    SinglyLinkedList() = default;
    SinglyLinkedList(std::initializer_list<T> values) {
        for (const auto& v : values) {
            push_back(v);
        }
    }

    SinglyLinkedList(const SinglyLinkedList& other) : SinglyLinkedList() {
        for (const auto& v : other) {
            push_back(v);
        }
    }
    SinglyLinkedList(SinglyLinkedList&& other) noexcept
        : head_(std::move(other.head_)), tail_(std::exchange(other.tail_, nullptr)),
          size_(std::exchange(other.size_, 0)) {}
    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        if (this != &other) {
            SinglyLinkedList copy(other);
            swap(copy);
        }
        return *this;
    }

    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
        SinglyLinkedList moved(std::move(other));
        swap(moved);
        return *this;
    }

    ~SinglyLinkedList() { clear(); }

    void push_front(T value) {
        auto node = std::make_unique<Node>(Node{std::move(value), std::move(head_)});
        head_ = std::move(node);
        if (tail_ == nullptr) {
            tail_ = head_.get();
        }
        ++size_;
    }

    void push_back(T value) {
        auto node = std::make_unique<Node>(Node{std::move(value), nullptr});
        Node* raw = node.get();
        if (tail_ == nullptr) {
            head_ = std::move(node);
        } else {
            tail_->next = std::move(node);
        }
        tail_ = raw;
        ++size_;
    }

    void pop_front() {
        head_ = std::move(head_->next);
        if (head_ == nullptr) {
            tail_ = nullptr;
        }
        --size_;
    }

    void clear() noexcept {
        while (head_ != nullptr) {
            head_ = std::move(head_->next);
        }
        tail_ = nullptr;
        size_ = 0;
    }

    /// Reverses the list in place in O(n) time and O(1) extra space.
    void reverse() noexcept {
        std::unique_ptr<Node> reversed;
        tail_ = head_.get();
        while (head_ != nullptr) {
            auto next = std::move(head_->next);
            head_->next = std::move(reversed);
            reversed = std::move(head_);
            head_ = std::move(next);
        }
        head_ = std::move(reversed);
    }

    /// Returns the middle element (the second of the two middles for even
    /// sizes) using the fast/slow pointer technique, or nullptr if empty.
    const T* middle() const noexcept {
        const Node* slow = head_.get();
        const Node* fast = head_.get();
        while (fast != nullptr && fast->next != nullptr) {
            slow = slow->next.get();
            fast = fast->next->next.get();
        }
        return slow != nullptr ? &slow->value : nullptr;
    }

    const T& front() const { return head_->value; }
    const T& back() const { return tail_->value; }
    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    ConstIterator begin() const { return ConstIterator(head_.get()); }
    ConstIterator end() const { return ConstIterator(); }

    void swap(SinglyLinkedList& other) noexcept {
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
    }

private:
    std::unique_ptr<Node> head_;
    Node* tail_ = nullptr; // non-owning
    std::size_t size_ = 0;
};

} // namespace sk::algo
