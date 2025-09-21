cmake_minimum_required(VERSION 3.27)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0149 NEW)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14 CACHE STRING "Build for 10.14")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(BeepBox VERSION 0.4.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# graphics backend
if (NOT GRAPHICS_BACKEND)
    if (WIN32)
        set(GRAPHICS_BACKEND d3d11)
    else()
        set(GRAPHICS_BACKEND gl)
    endif()
endif()

include_directories(src)
set(LIBRARIES )

#################
# imgui library #
#################

set(IMGUI_SOURCES
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui.cpp
)

if (${GRAPHICS_BACKEND} STREQUAL "d3d11")
    set(IMGUI_SOURCES ${IMGUI_SOURCES} src/imgui/backends/imgui_impl_dx11.cpp)
    set(LIBRARIES ${LIBRARIES} d3d11 dxgi dwmapi)
elseif (${GRAPHICS_BACKEND} STREQUAL "gl")
    set(IMGUI_SOURCES ${IMGUI_SOURCES} src/imgui/backends/imgui_impl_opengl3.cpp)
else()
    message(FATAL_ERROR "Unknown graphics backend option " ${GRAPHICS_BACKEND})
endif()

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui
    PUBLIC
        src/imgui
        src/imgui/backends
)

set_target_properties(imgui PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    LANGUAGE CXX
)

#####################
## other libraries ##
#####################

set(BEEPBOX_SYNTH_BUILD_STATIC 1)
add_subdirectory(src/cbeepsynth)
add_subdirectory(src/clap)
add_subdirectory(src/plugin_gui ${CMAKE_BINARY_DIR}/src/plugin_gui)

if(BUILD_STANDALONE OR BUILD_VST3)
    set(CLAP_WRAPPER_DOWNLOAD_DEPENDENCIES TRUE CACHE BOOL "Download Dependencies")
    set(CLAP_WRAPPER_DONT_ADD_TARGETS TRUE)
    add_subdirectory(src/clap-wrapper)
endif()

# plugin_gui is not added to LIBRARIES because i want a variation of the plugin_gui
# library when building vst3 (it will add the vst3 trademark to the about page)
set(LIBRARIES ${LIBRARIES} clap beepbox_synth_static)

#################
## clap target ##
#################

set(CLAP_SOURCES src/plugin/entry.c src/plugin/plugin.c src/plugin/instrument.c)
set(CLAP_TARGET ${PROJECT_NAME}_clap)
add_library(${CLAP_TARGET} MODULE ${CLAP_SOURCES})
target_compile_definitions(${CLAP_TARGET} PRIVATE PLUGIN_VERSION="${PROJECT_VERSION}")
target_link_libraries(${CLAP_TARGET} PRIVATE ${LIBRARIES} plugin_gui)
set_target_properties(${CLAP_TARGET} PROPERTIES
    SUFFIX ".clap"
    PREFIX ""
    VERSION ${PROJECT_VERSION}
)

#######################
## standalone target ##
#######################

if(BUILD_STANDALONE)
    function(create_standalone INST)
        set(STANDALONE_INST ${PROJECT_NAME}_standalone_${INST})
        add_executable(${STANDALONE_INST})
        target_sources(${STANDALONE_INST} PRIVATE ${CLAP_SOURCES})
        target_compile_definitions(${STANDALONE_INST} PRIVATE PLUGIN_VERSION="${PROJECT_VERSION}")
        target_link_libraries(${STANDALONE_INST} PRIVATE ${LIBRARIES} plugin_gui)
        target_add_standalone_wrapper(TARGET ${STANDALONE_INST}
            OUTPUT_NAME "BeepBox_${INST}"
            STATICALLY_LINKED_CLAP_ENTRY True
            PLUGIN_ID "us.pkhead.beepbox.${INST}"
        )
    endfunction()

    create_standalone(fm)
    create_standalone(chip)
    create_standalone(harmonics)
    create_standalone(spectrum)
endif()

#################
## vst3 target ##
#################

if(BUILD_VST3)
    set(PLUGIN_VST3 True)
    add_subdirectory(src/plugin_gui ${CMAKE_BINARY_DIR}/src/plugin_gui_vst3)

    set(VST3_TARGET ${PROJECT_NAME}_vst3)
    add_library(${VST3_TARGET} MODULE)
    target_sources(${VST3_TARGET} PRIVATE ${CLAP_SOURCES})
    target_compile_definitions(${VST3_TARGET} PRIVATE PLUGIN_VERSION="${PROJECT_VERSION}")
    target_link_libraries(${VST3_TARGET} PRIVATE ${LIBRARIES} plugin_gui_vst3)
    target_add_vst3_wrapper(TARGET ${VST3_TARGET}
        OUTPUT_NAME "BeepBox"
        SUPPORTS_ALL_NOTE_EXPRESSIONS False
    )
endif()

message(STATUS "Configuration: ")

if (BUILD_VST3)
    message(STATUS      "\tBUILD_VST3:           On")
else()
    message(STATUS      "\tBUILD_VST3:           Off")
endif()

if (BUILD_STANDALONE)
    message(STATUS      "\tBUILD_STANDALONE:     On")
else()
    message(STATUS      "\tBUILD_STANDALONE:     Off")
endif()

message(STATUS          "\tGRAPHICS_BACKEND:     " ${GRAPHICS_BACKEND})