set(bench_targets bench_vector bench_queue bench_memory)

add_executable(bench_vector bench_vector.cpp)
target_link_libraries(bench_vector PRIVATE sk::containers benchmark::benchmark_main)

add_executable(bench_queue bench_queue.cpp)
target_link_libraries(bench_queue PRIVATE sk::concurrency benchmark::benchmark_main)

add_executable(bench_memory bench_memory.cpp)
target_link_libraries(bench_memory PRIVATE sk::memory benchmark::benchmark_main)

foreach(target IN LISTS bench_targets)
    sk_set_warnings(${target})
endforeach()

add_custom_target(benchmarks DEPENDS ${bench_targets})
