#pragma once

#include <atomic>
#include <bit>
#include <cstddef>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

namespace sk {

// Size of the unit the CPU keeps coherent between cores. Apple silicon uses
// 128-byte lines; x86 and most other ARM cores use 64. (We avoid
// std::hardware_destructive_interference_size because its value can differ
// between compilers and GCC warns when it is used in headers.)
#if defined(__APPLE__) && defined(__aarch64__)
inline constexpr std::size_t kCacheLineSize = 128;
#else
inline constexpr std::size_t kCacheLineSize = 64;
#endif

/// Bounded, lock-free, single-producer/single-consumer ring buffer.
///
/// Exactly one thread may call the push functions and exactly one (possibly
/// different) thread may call the pop functions.
///
/// How it avoids locks:
///  - head_ is written only by the consumer, tail_ only by the producer, so
///    no compare-and-swap is ever needed; plain loads and stores suffice.
///  - The producer publishes an element with a release store to tail_; the
///    consumer's acquire load of tail_ then guarantees it sees the fully
///    constructed element. The same pairing on head_ tells the producer a
///    slot has been vacated.
///  - Each side keeps a cached copy of the other side's index and re-reads
///    the shared atomic only when the cache says the queue looks full/empty.
///    This removes most cross-core cache-line traffic.
///  - Producer and consumer state sit on separate cache lines so the two
///    threads never invalidate each other's lines (no false sharing).
///
/// Indices grow monotonically and are masked into the buffer, so capacity
/// must be a power of two and "full" is simply tail - head == capacity.
template <typename T>
class SpscQueue {
    static_assert(std::is_nothrow_destructible_v<T>);

public:
    explicit SpscQueue(std::size_t capacity)
        : capacity_(std::bit_ceil(capacity < 2 ? std::size_t{2} : capacity)), mask_(capacity_ - 1),
          slots_(std::allocator<T>{}.allocate(capacity_)) {}

    SpscQueue(const SpscQueue&) = delete;
    SpscQueue& operator=(const SpscQueue&) = delete;

    ~SpscQueue() {
        const std::size_t tail = tail_.load(std::memory_order_relaxed);
        for (std::size_t i = head_.load(std::memory_order_relaxed); i != tail; ++i) {
            std::destroy_at(slots_ + (i & mask_));
        }
        std::allocator<T>{}.deallocate(slots_, capacity_);
    }

    /// Producer only. Returns false (and constructs nothing) if the queue is full.
    template <typename... Args>
    bool try_emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
        const std::size_t tail = tail_.load(std::memory_order_relaxed);
        if (tail - cached_head_ == capacity_) {
            cached_head_ = head_.load(std::memory_order_acquire);
            if (tail - cached_head_ == capacity_) {
                return false;
            }
        }
        std::construct_at(slots_ + (tail & mask_), std::forward<Args>(args)...);
        tail_.store(tail + 1, std::memory_order_release);
        return true;
    }

    bool try_push(const T& value) { return try_emplace(value); }
    bool try_push(T&& value) { return try_emplace(std::move(value)); }

    /// Consumer only. Returns std::nullopt if the queue is empty.
    std::optional<T> try_pop() noexcept(std::is_nothrow_move_constructible_v<T>) {
        const std::size_t head = head_.load(std::memory_order_relaxed);
        if (head == cached_tail_) {
            cached_tail_ = tail_.load(std::memory_order_acquire);
            if (head == cached_tail_) {
                return std::nullopt;
            }
        }
        T* slot = slots_ + (head & mask_);
        std::optional<T> value(std::move(*slot));
        std::destroy_at(slot);
        head_.store(head + 1, std::memory_order_release);
        return value;
    }

    /// Approximate when called concurrently; exact when the queue is quiescent.
    std::size_t size() const noexcept {
        return tail_.load(std::memory_order_acquire) - head_.load(std::memory_order_acquire);
    }
    bool empty() const noexcept { return size() == 0; }
    std::size_t capacity() const noexcept { return capacity_; }

private:
    const std::size_t capacity_;
    const std::size_t mask_;
    T* const slots_;

    // Consumer-owned cache line.
    alignas(kCacheLineSize) std::atomic<std::size_t> head_{0};
    std::size_t cached_tail_ = 0;

    // Producer-owned cache line.
    alignas(kCacheLineSize) std::atomic<std::size_t> tail_{0};
    std::size_t cached_head_ = 0;
    // No trailing padding needed: alignas makes sizeof(SpscQueue) a multiple
    // of kCacheLineSize, so nothing else can share the producer's line.
};

} // namespace sk
