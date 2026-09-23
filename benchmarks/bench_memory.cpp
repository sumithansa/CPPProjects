// Allocation cost: fixed-block pool vs the general-purpose heap, and
// make_shared (one allocation) vs SharedPtr(new T) (two allocations).
#include <sk/pool_allocator.hpp>
#include <sk/shared_ptr.hpp>

#include <benchmark/benchmark.h>

#include <array>
#include <cstdint>
#include <vector>

namespace {

struct Particle {
    std::array<float, 12> state{};
};

constexpr std::size_t kBatch = 1024;

void BM_HeapNewDelete(benchmark::State& state) {
    std::vector<Particle*> live(kBatch);
    for (auto _ : state) {
        for (auto& p : live) {
            p = new Particle();
        }
        benchmark::DoNotOptimize(live.data());
        for (auto* p : live) {
            delete p;
        }
    }
    state.SetItemsProcessed(state.iterations() * static_cast<std::int64_t>(kBatch));
}
BENCHMARK(BM_HeapNewDelete);

void BM_ObjectPool(benchmark::State& state) {
    sk::ObjectPool<Particle> pool(kBatch);
    std::vector<Particle*> live(kBatch);
    for (auto _ : state) {
        for (auto& p : live) {
            p = pool.create();
        }
        benchmark::DoNotOptimize(live.data());
        for (auto* p : live) {
            pool.destroy(p);
        }
    }
    state.SetItemsProcessed(state.iterations() * static_cast<std::int64_t>(kBatch));
}
BENCHMARK(BM_ObjectPool);

void BM_SharedPtrFromNew(benchmark::State& state) {
    for (auto _ : state) {
        sk::SharedPtr<Particle> p(new Particle());
        benchmark::DoNotOptimize(p.get());
    }
}
BENCHMARK(BM_SharedPtrFromNew);

void BM_MakeShared(benchmark::State& state) {
    for (auto _ : state) {
        auto p = sk::make_shared<Particle>();
        benchmark::DoNotOptimize(p.get());
    }
}
BENCHMARK(BM_MakeShared);

} // namespace
