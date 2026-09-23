# Each .cpp in this directory is a small, self-contained program illustrating one language feature.
file(GLOB example_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(source IN LISTS example_sources)
    get_filename_component(name ${source} NAME_WE)
    add_executable(example_${name} ${source})
    target_link_libraries(example_${name} PRIVATE sk::containers sk::memory sk::patterns)
    sk_set_warnings(example_${name})
    if(MSVC)
        # The demos use portable C stdio (tmpfile), not the MSVC-only *_s variants.
        target_compile_definitions(example_${name} PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
endforeach()
