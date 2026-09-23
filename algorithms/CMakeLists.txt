add_library(sk_algorithms src/problems.cpp)
add_library(sk::algorithms ALIAS sk_algorithms)
target_include_directories(sk_algorithms PUBLIC include)
target_compile_features(sk_algorithms PUBLIC cxx_std_20)
sk_set_warnings(sk_algorithms)

if(SK_BUILD_TESTS)
    sk_add_test(algorithms_tests
        SOURCES tests/sorting_test.cpp tests/linked_list_test.cpp tests/problems_test.cpp
        LIBS sk::algorithms)
endif()
