cmake_minimum_required(VERSION 3.25)

project(ModernCppSystems
    VERSION 1.0.0
    DESCRIPTION "Modern C++20 containers, concurrency primitives and smart pointers, built from scratch"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(SK_BUILD_TESTS          "Build unit tests"                      ON)
option(SK_BUILD_EXAMPLES       "Build example programs"                ON)
option(SK_BUILD_BENCHMARKS     "Build Google Benchmark suites"         OFF)
option(SK_WARNINGS_AS_ERRORS   "Treat compiler warnings as errors"     OFF)
set(SK_SANITIZERS "" CACHE STRING "Sanitizers to enable, e.g. 'address;undefined' or 'thread'")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Sanitizers)

# Must run before dependencies are added so third-party code is instrumented too.
sk_enable_sanitizers("${SK_SANITIZERS}")

include(Dependencies)
find_package(Threads REQUIRED)

if(SK_BUILD_TESTS)
    enable_testing()
    include(GoogleTest)
endif()

add_subdirectory(libs/containers)
add_subdirectory(libs/memory)
add_subdirectory(libs/concurrency)
add_subdirectory(libs/patterns)
add_subdirectory(algorithms)

if(SK_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(SK_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
