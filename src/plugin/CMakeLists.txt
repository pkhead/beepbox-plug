set(PROJECT_NAME plugin)
project(${PROJECT_NAME} VERSION ${CMAKE_PROJECT_VERSION})

add_library(${PROJECT_NAME} STATIC
    plugin.cpp
    plugin_ext_interface.cpp
    params.cpp
    device.cpp
    instr_device.cpp
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE PUGL_STATIC PLUGIN_VERSION="${PROJECT_VERSION}"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    LANGUAGE CXX
)
target_include_directories(${PROJECT_NAME}
    PRIVATE ../clap/include
    PUBLIC include
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE beepbox_synth_static
)

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    # standards-conforming preprocessor
    target_compile_options(${PROJECT_NAME} PRIVATE /Zc:preprocessor)
endif()