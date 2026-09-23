add_library(sk_concurrency src/thread_pool.cpp)
add_library(sk::concurrency ALIAS sk_concurrency)
target_include_directories(sk_concurrency PUBLIC include)
target_compile_features(sk_concurrency PUBLIC cxx_std_20)
target_link_libraries(sk_concurrency PUBLIC Threads::Threads)
sk_set_warnings(sk_concurrency)

if(SK_BUILD_TESTS)
    sk_add_test(concurrency_tests
        SOURCES tests/spsc_queue_test.cpp tests/thread_pool_test.cpp
        LIBS sk::concurrency)
endif()
