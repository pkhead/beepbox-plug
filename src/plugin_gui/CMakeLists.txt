if (PLUGIN_VST3)
    set(PROJECT_NAME plugin_gui_vst3)
else()
    set(PROJECT_NAME plugin_gui)
endif()

project(${PROJECT_NAME} VERSION ${CMAKE_PROJECT_VERSION})

set(PUGL_SOURCES
    ../pugl/src/common.c
    ../pugl/src/internal.c
)

set(EXTRA_LIBS )

# include pugl backend implementations depending on
# graphics backend and platform
if (WIN32)
    set(PUGL_SOURCES ${PUGL_SOURCES}
        ../pugl/src/win.c
        ../pugl/src/win_stub.c
    )

    set(EXTRA_LIBS ${EXTRA_LIBS} dwmapi)

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

else() # not Win32 or Cocoa, so assume X11
    set(PUGL_SOURCES ${PUGL_SOURCES}
        ../pugl/src/x11.c
        ../pugl/src/x11_stub.c
    )

    if (${GRAPHICS_BACKEND} STREQUAL "gl")
        set(PUGL_SOURCES ${PUGL_SOURCES} ../pugl/src/x11_gl.c)
    endif()

    find_package(X11 REQUIRED)
    find_library(LIB_XCURSOR Xcursor)
    find_library(LIB_XRANDR Xrandr)

    set(EXTRA_LIBS ${EXTRA_LIBS} ${X11_LIBRARIES} ${LIB_XCURSOR} ${LIB_XRANDR})

endif()

# create the library target
add_library(${PROJECT_NAME} STATIC
    log.cpp
    gui.cpp
    lib_impl.cpp
    platform.cpp
    plugin_controller.cpp
    ${PUGL_SOURCES}
)

# choose correct gfx_*.cpp graphics backend
if (${GRAPHICS_BACKEND} STREQUAL "d3d11")
    target_compile_definitions(${PROJECT_NAME} PRIVATE GFX_D3D11)
    target_sources(${PROJECT_NAME} PRIVATE gfx_d3d11.cpp)
elseif (${GRAPHICS_BACKEND} STREQUAL "gl")
    target_compile_definitions(${PROJECT_NAME} PRIVATE GFX_OPENGL)
    target_sources(${PROJECT_NAME} PRIVATE gfx_gl.cpp)

    find_package(OpenGL REQUIRED)
    set(EXTRA_LIBS ${EXTRA_LIBS} ${OPENGL_LIBRARIES})
endif()

if(PLUGIN_VST3)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PLUGIN_VST3)
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE PUGL_STATIC PLUGIN_VERSION="${PROJECT_VERSION}")

set_target_properties(${PROJECT_NAME} PROPERTIES
    LANGUAGE CXX
)
target_include_directories(${PROJECT_NAME}
    PRIVATE ../imgui ../gui_libs ../pugl/include ../clap/include
    PUBLIC include
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE imgui ${EXTRA_LIBS}
)