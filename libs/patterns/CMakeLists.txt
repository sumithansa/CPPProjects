add_library(sk_patterns INTERFACE)
add_library(sk::patterns ALIAS sk_patterns)
target_include_directories(sk_patterns INTERFACE include)
target_compile_features(sk_patterns INTERFACE cxx_std_20)

if(SK_BUILD_TESTS)
    sk_add_test(patterns_tests
        SOURCES tests/singleton_test.cpp tests/factory_test.cpp
        LIBS sk::patterns Threads::Threads)
endif()
