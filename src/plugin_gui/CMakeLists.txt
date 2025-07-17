if (PLUGIN_VST3)
    set(PROJECT_NAME plugin_gui_vst3)
else()
    set(PROJECT_NAME plugin_gui)
endif()

project(${PROJECT_NAME})

set(PUGL_SOURCES
    ../pugl/src/common.c
    ../pugl/src/internal.c

    ../pugl/src/win.c
    ../pugl/src/win_gl.c
    ../pugl/src/win_stub.c
)

add_library(${PROJECT_NAME} STATIC
    gui.cpp
    lib_impl.cpp
    platform.cpp
    plugin_controller.cpp
    ${PUGL_SOURCES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE PUGL_STATIC)

if(PLUGIN_VST3)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PLUGIN_VST3)
endif()

# choose graphics backend
if (WIN32)
    # d3d11
    target_compile_definitions(${PROJECT_NAME} PRIVATE GFX_D3D11)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    LANGUAGE CXX
)
target_include_directories(${PROJECT_NAME}
    PRIVATE ../sokol ../imgui ../gui_libs ../pugl/include
    PUBLIC include
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE imgui
)