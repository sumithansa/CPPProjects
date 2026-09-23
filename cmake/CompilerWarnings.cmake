# Strict warnings for targets owned by this project (third-party code is left alone).
function(sk_set_warnings target)
    set(msvc_warnings
        /W4
        /permissive-
        /w14242 /w14254 /w14263 /w14265 /w14287 /w14296 /w14311 /w14826
        /wd4324) # "structure was padded due to alignment specifier": intended for cache-line isolation

    set(common_warnings
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wcast-align
        -Wdouble-promotion
        -Wimplicit-fallthrough)

    # -Wnull-dereference is clang-only: GCC's version depends on optimiser
    # inlining and produces false positives inside the standard library at -O3.
    set(clang_warnings ${common_warnings} -Wnull-dereference)
    set(gcc_warnings ${common_warnings} -Wmisleading-indentation -Wduplicated-cond -Wlogical-op -Wuseless-cast)

    if(SK_WARNINGS_AS_ERRORS)
        list(APPEND msvc_warnings /WX)
        list(APPEND clang_warnings -Werror)
        list(APPEND gcc_warnings -Werror)
    endif()

    get_target_property(type ${target} TYPE)
    if(type STREQUAL "INTERFACE_LIBRARY")
        set(scope INTERFACE)
    else()
        set(scope PRIVATE)
    endif()

    if(MSVC)
        target_compile_options(${target} ${scope} ${msvc_warnings})
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${target} ${scope} ${clang_warnings})
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${target} ${scope} ${gcc_warnings})
    endif()
endfunction()
