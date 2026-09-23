// sk::Vector vs std::vector, and the cost of skipping reserve().
#include <sk/vector.hpp>

#include <benchmark/benchmark.h>

#include <string>
#include <vector>

namespace {

template <typename Vec>
void BM_PushBackInts(benchmark::State& state) {
    const auto n = static_cast<int>(state.range(0));
    for (auto _ : state) {
        Vec v;
        for (int i = 0; i < n; ++i) {
            v.push_back(i);
        }
        benchmark::DoNotOptimize(v.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * n);
}
BENCHMARK_TEMPLATE(BM_PushBackInts, std::vector<int>)->Arg(1'000)->Arg(100'000);
BENCHMARK_TEMPLATE(BM_PushBackInts, sk::Vector<int>)->Arg(1'000)->Arg(100'000);

template <typename Vec>
void BM_PushBackStrings(benchmark::State& state) {
    const auto n = static_cast<int>(state.range(0));
    const std::string payload(32, 'x'); // beyond SSO: exercises move-on-reallocate
    for (auto _ : state) {
        Vec v;
        for (int i = 0; i < n; ++i) {
            v.push_back(payload);
        }
        benchmark::DoNotOptimize(v.data());
    }
    state.SetItemsProcessed(state.iterations() * n);
}
BENCHMARK_TEMPLATE(BM_PushBackStrings, std::vector<std::string>)->Arg(10'000);
BENCHMARK_TEMPLATE(BM_PushBackStrings, sk::Vector<std::string>)->Arg(10'000);

void BM_EmplaceWithReserve(benchmark::State& state) {
    const auto n = static_cast<int>(state.range(0));
    for (auto _ : state) {
        sk::Vector<std::string> v;
        v.reserve(static_cast<std::size_t>(n));
        for (int i = 0; i < n; ++i) {
            v.emplace_back(32, 'x');
        }
        benchmark::DoNotOptimize(v.data());
    }
    state.SetItemsProcessed(state.iterations() * n);
}
BENCHMARK(BM_EmplaceWithReserve)->Arg(10'000);

} // namespace
