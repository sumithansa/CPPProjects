// Producer -> consumer throughput: lock-free SPSC ring buffer vs a mutex-protected queue.
//
// Both queues get the same capacity and the same spin-wait loop (with a CPU
// pause hint), so the only difference is the synchronisation strategy.
#include <sk/cpu_relax.hpp>
#include <sk/spsc_queue.hpp>

#include <benchmark/benchmark.h>

#include <cstdint>
#include <deque>
#include <mutex>
#include <optional>
#include <thread>

namespace {

constexpr std::uint64_t kItems = 10'000'000;

// Baseline: the straightforward thread-safe queue most people write first.
class MutexQueue {
public:
    explicit MutexQueue(std::size_t capacity) : capacity_(capacity) {}

    bool try_push(std::uint64_t value) {
        std::lock_guard lock(mutex_);
        if (items_.size() == capacity_) {
            return false;
        }
        items_.push_back(value);
        return true;
    }

    std::optional<std::uint64_t> try_pop() {
        std::lock_guard lock(mutex_);
        if (items_.empty()) {
            return std::nullopt;
        }
        const auto value = items_.front();
        items_.pop_front();
        return value;
    }

private:
    std::size_t capacity_;
    std::mutex mutex_;
    std::deque<std::uint64_t> items_;
};

template <typename Queue>
void run_transfer(Queue& queue) {
    std::thread producer([&queue] {
        for (std::uint64_t i = 0; i < kItems; ++i) {
            while (!queue.try_push(i)) {
                sk::cpu_relax();
            }
        }
    });

    std::uint64_t received = 0;
    std::uint64_t checksum = 0;
    while (received < kItems) {
        if (auto v = queue.try_pop()) {
            checksum += *v;
            ++received;
        } else {
            sk::cpu_relax();
        }
    }
    producer.join();
    benchmark::DoNotOptimize(checksum);
}

template <typename Queue>
void BM_Transfer(benchmark::State& state) {
    for (auto _ : state) {
        Queue queue(static_cast<std::size_t>(state.range(0)));
        run_transfer(queue);
    }
    state.SetItemsProcessed(state.iterations() * static_cast<std::int64_t>(kItems));
}

BENCHMARK_TEMPLATE(BM_Transfer, sk::SpscQueue<std::uint64_t>)
    ->Arg(1024)
    ->Arg(65536)
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_Transfer, MutexQueue)
    ->Arg(1024)
    ->Arg(65536)
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();

} // namespace
