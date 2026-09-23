# Third-party dependencies are fetched at configure time; nothing is vendored.
# FIND_PACKAGE_ARGS lets a system-installed copy take precedence when available.
include(FetchContent)

if(SK_BUILD_TESTS)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        SYSTEM
        FIND_PACKAGE_ARGS NAMES GTest)
    FetchContent_MakeAvailable(googletest)
endif()

if(SK_BUILD_BENCHMARKS)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(benchmark
        URL https://github.com/google/benchmark/archive/refs/tags/v1.9.1.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        SYSTEM
        FIND_PACKAGE_ARGS NAMES benchmark)
    FetchContent_MakeAvailable(benchmark)
endif()

# Registers a GoogleTest executable built from the given sources.
function(sk_add_test name)
    cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
    add_executable(${name} ${ARG_SOURCES})
    target_link_libraries(${name} PRIVATE ${ARG_LIBS} GTest::gtest_main)
    sk_set_warnings(${name})
    gtest_discover_tests(${name} DISCOVERY_MODE PRE_TEST)
endfunction()
