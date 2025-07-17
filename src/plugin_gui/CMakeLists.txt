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

if (WIN32)
    set(PUGL_SOURCES ${PUGL_SOURCES}
        ../pugl/src/win.c
        ../pugl/src/win_stub.c
    )

    if (${GRAPHICS_BACKEND} STREQUAL "gl")
        set(PUGL_SOURCES ${PUGL_SOURCES} ../pugl/src/win_gl.c)
    endif()

elseif(APPLE)
    set(PUGL_SOURCES ${PUGL_SOURCES}
        ../pugl/src/mac.m
        ../pugl/src/mac_stub.m
    )

    if (${GRAPHICS_BACKEND} STREQUAL "gl")
        set(PUGL_SOURCES ${PUGL_SOURCES} ../pugl/src/mac_gl.m)
    endif()

else()
    set(PUGL_SOURCES ${PUGL_SOURCES}
        ../pugl/src/x11.c
        ../pugl/src/x11_stub.c
    )

    if (${GRAPHICS_BACKEND} STREQUAL "gl")
        set(PUGL_SOURCES ${PUGL_SOURCES} ../pugl/src/x11_gl.c)
    endif()

endif()

add_library(${PROJECT_NAME} STATIC
    gui.cpp
    lib_impl.cpp
    platform.cpp
    plugin_controller.cpp
    ${PUGL_SOURCES}
)

# graphics backend implementations
if (${GRAPHICS_BACKEND} STREQUAL "d3d11")
    target_compile_definitions(${PROJECT_NAME} PRIVATE GFX_D3D11)
    target_sources(${PROJECT_NAME} PRIVATE gfx_d3d11.cpp)
elseif (${GRAPHICS_BACKEND} STREQUAL "gl")
    target_compile_definitions(${PROJECT_NAME} PRIVATE GFX_OPENGL)
    target_sources(${PROJECT_NAME} PRIVATE gfx_gl.cpp)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE PUGL_STATIC)

if(PLUGIN_VST3)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PLUGIN_VST3)
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