add_library(sk_memory INTERFACE)
add_library(sk::memory ALIAS sk_memory)
target_include_directories(sk_memory INTERFACE include)
target_compile_features(sk_memory INTERFACE cxx_std_20)

if(SK_BUILD_TESTS)
    sk_add_test(memory_tests
        SOURCES tests/unique_ptr_test.cpp tests/shared_ptr_test.cpp tests/pool_allocator_test.cpp
        LIBS sk::memory Threads::Threads)
endif()
