#pragma once

#include <cstddef>
#include <memory>
#include <new>
#include <utility>

namespace sk {

/// Fixed-size block allocator backed by one contiguous arena.
///
/// Free blocks form an intrusive singly linked list: the first bytes of each
/// free block hold the pointer to the next one, so bookkeeping costs no extra
/// memory. allocate() and deallocate() are O(1) pointer swaps with no system
/// calls, which is why pools are common in games, networking and drivers where
/// many same-sized objects churn.
///
/// Not thread-safe: use one pool per thread, or guard it externally.
class FixedBlockPool {
public:
    FixedBlockPool(std::size_t block_size, std::size_t block_count)
        : block_size_(round_up(block_size < sizeof(FreeNode) ? sizeof(FreeNode) : block_size)),
          block_count_(block_count),
          arena_(std::make_unique<std::byte[]>(block_size_ * block_count)) {
        // operator new[] returns memory aligned for any fundamental type, and
        // every block size is a multiple of that alignment.
        for (std::size_t i = block_count_; i-- > 0;) {
            push(arena_.get() + i * block_size_);
        }
    }

    FixedBlockPool(const FixedBlockPool&) = delete;
    FixedBlockPool& operator=(const FixedBlockPool&) = delete;

    /// Returns a block, or nullptr when the pool is exhausted.
    [[nodiscard]] void* allocate() noexcept {
        if (head_ == nullptr) {
            return nullptr;
        }
        FreeNode* node = head_;
        head_ = node->next;
        --free_count_;
        return node;
    }

    void deallocate(void* block) noexcept {
        if (block != nullptr) {
            push(static_cast<std::byte*>(block));
        }
    }

    bool owns(const void* block) const noexcept {
        const auto* p = static_cast<const std::byte*>(block);
        return p >= arena_.get() && p < arena_.get() + block_size_ * block_count_;
    }

    std::size_t block_size() const noexcept { return block_size_; }
    std::size_t free_blocks() const noexcept { return free_count_; }
    std::size_t capacity() const noexcept { return block_count_; }

private:
    struct FreeNode {
        FreeNode* next;
    };

    static constexpr std::size_t round_up(std::size_t size) noexcept {
        constexpr std::size_t align = alignof(std::max_align_t);
        return (size + align - 1) & ~(align - 1);
    }

    void push(std::byte* block) noexcept {
        auto* node = ::new (block) FreeNode{head_};
        head_ = node;
        ++free_count_;
    }

    std::size_t block_size_;
    std::size_t block_count_;
    std::unique_ptr<std::byte[]> arena_;
    FreeNode* head_ = nullptr;
    std::size_t free_count_ = 0;
};

/// Typed front-end: constructs T objects inside a FixedBlockPool.
template <typename T>
class ObjectPool {
public:
    static_assert(alignof(T) <= alignof(std::max_align_t), "over-aligned types are not supported");

    explicit ObjectPool(std::size_t capacity) : pool_(sizeof(T), capacity) {}

    template <typename... Args>
    [[nodiscard]] T* create(Args&&... args) {
        void* block = pool_.allocate();
        if (block == nullptr) {
            throw std::bad_alloc();
        }
        try {
            return ::new (block) T(std::forward<Args>(args)...);
        } catch (...) {
            pool_.deallocate(block);
            throw;
        }
    }

    void destroy(T* object) noexcept {
        if (object != nullptr) {
            object->~T();
            pool_.deallocate(object);
        }
    }

    std::size_t available() const noexcept { return pool_.free_blocks(); }

private:
    FixedBlockPool pool_;
};

} // namespace sk
