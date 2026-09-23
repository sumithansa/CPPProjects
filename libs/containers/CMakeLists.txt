add_library(sk_containers src/string.cpp)
add_library(sk::containers ALIAS sk_containers)
target_include_directories(sk_containers PUBLIC include)
target_compile_features(sk_containers PUBLIC cxx_std_20)
sk_set_warnings(sk_containers)

if(SK_BUILD_TESTS)
    sk_add_test(containers_tests
        SOURCES tests/vector_test.cpp tests/string_test.cpp
        LIBS sk::containers)
endif()
