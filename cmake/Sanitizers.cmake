# Enables sanitizers for every target in the build, including third-party code
# (TSan in particular needs the whole program instrumented to avoid false positives).
function(sk_enable_sanitizers sanitizers)
    if(NOT sanitizers)
        return()
    endif()

    if(MSVC)
        if("address" IN_LIST sanitizers)
            add_compile_options(/fsanitize=address)
        endif()
        return()
    endif()

    if("thread" IN_LIST sanitizers AND "address" IN_LIST sanitizers)
        message(FATAL_ERROR "ThreadSanitizer cannot be combined with AddressSanitizer")
    endif()

    list(JOIN sanitizers "," joined)
    message(STATUS "Sanitizers enabled: ${joined}")
    add_compile_options(-fsanitize=${joined} -fno-omit-frame-pointer -fno-sanitize-recover=all)
    add_link_options(-fsanitize=${joined})
endfunction()
