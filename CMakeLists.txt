cmake_minimum_required(VERSION 3.27)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0149 NEW)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14 CACHE STRING "Build for 10.14")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(BeepBox VERSION 0.0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories(src)

# imgui
add_library(imgui STATIC
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui.cpp
    src/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui
    PUBLIC
        src/imgui
        src/imgui/backends
)

set_target_properties(imgui PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    LANGUAGE CXX
)

add_subdirectory(src/clap)
add_subdirectory(src/synth)
add_subdirectory(src/plugin_gui ${CMAKE_BINARY_DIR}/src/plugin_gui)

set(CLAP_WRAPPER_DOWNLOAD_DEPENDENCIES TRUE CACHE BOOL "Download Dependencies")
set(CLAP_WRAPPER_DONT_ADD_TARGETS TRUE)
add_subdirectory(src/clap-wrapper)

set(SOURCES
    src/plugin/entry.c
)

set(LIBRARIES clap beepbox_synth)

# set(CLAP_TARGET ${PROJECT_NAME}_clap)
# add_library(${CLAP_TARGET} MODULE ${SOURCES})
# target_link_libraries(${CLAP_TARGET} PRIVATE ${LIBRARIES})
# set_target_properties(${CLAP_TARGET} PROPERTIES
#     SUFFIX ".clap"
#     PREFIX ""    
# )

set(STANDALONE_TARGET ${PROJECT_NAME}_standalone)
add_executable(${STANDALONE_TARGET})
target_sources(${STANDALONE_TARGET} PRIVATE ${SOURCES})
target_link_libraries(${STANDALONE_TARGET} PRIVATE ${LIBRARIES} plugin_gui)
target_add_standalone_wrapper(TARGET ${STANDALONE_TARGET}
    OUTPUT_NAME "BeepBox"
    STATICALLY_LINKED_CLAP_ENTRY True
    PLUGIN_ID "us.pkhead.beepbox"
)

set(PLUGIN_VST3 True)
add_subdirectory(src/plugin_gui ${CMAKE_BINARY_DIR}/src/plugin_gui_vst3)

set(VST3_TARGET ${PROJECT_NAME}_vst3)
add_library(${VST3_TARGET} MODULE)
target_sources(${VST3_TARGET} PRIVATE ${SOURCES})
target_link_libraries(${VST3_TARGET} PRIVATE ${LIBRARIES} plugin_gui_vst3)
target_add_vst3_wrapper(TARGET ${VST3_TARGET}
    OUTPUT_NAME "BeepBox"
    SUPPORTS_ALL_NOTE_EXPRESSIONS False
)