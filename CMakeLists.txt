# This file is for building the example project ONLY. It's not meant to be a module included as a subdirectory.
# If you're interested using CMake to build your plugins, read the code here, then copy/paste and modify it in
# your own projects. It's easy!
cmake_minimum_required(VERSION 3.7)
project(beepbox VERSION 0.0.1)

set(CMAKE_C_STANDARD 11)

if (APPLE)
    enable_language(OBJC)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

# settings used across all formats
include_directories(CPLUG/src)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_STANDARD 20) # required for c++ clap build
    add_compile_options(
        /FI ${CMAKE_SOURCE_DIR}/plugin/config.h
    )
else()
    add_compile_options(
        -Werror=return-type
        -Werror=shadow
        -Wunused-function

        -Wno-deprecated
        -Wno-multichar
        -Wno-nullability-completeness
        -Wno-writable-strings

        -include${CMAKE_SOURCE_DIR}/plugin/config.h
    )
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-Werror=excess-initializers)
    endif()
endif()







set(BEEPBOX_SOURCES
    plugin/main.cpp
    plugin/sokol.cpp
    plugin/platform.cpp
)

set(BEEPBOX_INCLUDES ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/CPLUG/src )

add_subdirectory(synth)


# imgui
add_library(imgui STATIC
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui
    PUBLIC
        imgui
        imgui/backends
)

set_target_properties(imgui PROPERTIES POSITION_INDEPENDENT_CODE ON)

set(LIBRARIES imgui beepbox_synth)

# ██╗   ██╗███████╗████████╗██████╗ 
# ██║   ██║██╔════╝╚══██╔══╝╚════██╗
# ██║   ██║███████╗   ██║    █████╔╝
# ╚██╗ ██╔╝╚════██║   ██║    ╚═══██╗
#  ╚████╔╝ ███████║   ██║   ██████╔╝
#   ╚═══╝  ╚══════╝   ╚═╝   ╚═════╝ 

if (BUILD_VST3)
    if (WIN32)
        add_library(beepbox_vst3 MODULE
            ${BEEPBOX_SOURCES}
            CPLUG/src/cplug_vst3.c
        )
        target_include_directories(beepbox_vst3 PRIVATE ${BEEPBOX_INCLUDES})
        target_link_libraries(beepbox_vst3 ${LIBRARIES})

        # According to the docs you're meant to bundle your Windows VST3 using a bundle like folder structure:
        # https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical+Documentation/Locations+Format/Plugin+Format.html#for-the-windows-platform
        # However, placing the unbundled binary directly into your ...\Common Files\VST3 folder will work fine in every host.
        # I'm not sure anyone in the real world actually bundles Windows VST3s like this, but maybe I'm just ignorant.
        set_target_properties(beepbox_vst3 PROPERTIES
            BUNDLE True
            BUNDLE_EXTENSION vst3
            OUTPUT_NAME BeepBox                                             # out binary name, differs from target name
            CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/BeepBox.vst3 # out directory name, differs from target name
            SUFFIX .vst3
            PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/BeepBox.vst3
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/BeepBox.vst3/Contents/x86_64-win
        )
    elseif (APPLE)
        message(FATAL_ERROR "Apple not supported")
    endif()
endif()

#  ██████╗██╗      █████╗ ██████╗
# ██╔════╝██║     ██╔══██╗██╔══██╗
# ██║     ██║     ███████║██████╔╝
# ██║     ██║     ██╔══██║██╔═══╝
# ╚██████╗███████╗██║  ██║██║
#  ╚═════╝╚══════╝╚═╝  ╚═╝╚═╝

if (BUILD_CLAP)
    if (APPLE)
        message(FATAL_ERROR "Apple not supported")
    elseif(WIN32)
        add_library(beepbox_clap MODULE ${BEEPBOX_SOURCES} src/cplug_clap.c)
        target_include_directories(beepbox_clap PRIVATE ${BEEPBOX_INCLUDES})
        target_link_libraries(beepbox_vst3 ${LIBRARIES})
        set_target_properties(beepbox_clap PROPERTIES
            OUTPUT_NAME BeepBox
            SUFFIX .clap
            PDB_NAME beepbox_clap
            )
    endif()
endif()

# ███████╗████████╗ █████╗ ███╗   ██╗██████╗  █████╗ ██╗      ██████╗ ███╗   ██╗███████╗
# ██╔════╝╚══██╔══╝██╔══██╗████╗  ██║██╔══██╗██╔══██╗██║     ██╔═══██╗████╗  ██║██╔════╝
# ███████╗   ██║   ███████║██╔██╗ ██║██║  ██║███████║██║     ██║   ██║██╔██╗ ██║█████╗  
# ╚════██║   ██║   ██╔══██║██║╚██╗██║██║  ██║██╔══██║██║     ██║   ██║██║╚██╗██║██╔══╝  
# ███████║   ██║   ██║  ██║██║ ╚████║██████╔╝██║  ██║███████╗╚██████╔╝██║ ╚████║███████╗
# ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═══╝╚═════╝ ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝

if (BUILD_STANDALONE)
    if (WIN32)
        add_executable(beepbox_standalone WIN32 ${BEEPBOX_SOURCES} CPLUG/src/cplug_standalone_win.c)
        target_include_directories(beepbox_standalone PRIVATE ${BEEPBOX_INCLUDES})
        target_link_libraries(beepbox_standalone ${LIBRARIES})
    else()
        message(FATAL_ERROR "platform not supported")
    endif()
endif()