cmake_minimum_required(VERSION 3.22)
project(BEEPBOX VERSION 0.0.1)

find_package(JUCE CONFIG REQUIRED)

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

juce_add_plugin(BeepboxPlugin
    PLUGIN_MANUFACTURER_CODE Pkhd
    PLUGIN_CODE BpBx

    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE

    FORMATS VST3
    PRODUCT_NAME "BeepBox")

juce_generate_juce_header(BeepboxPlugin)

add_subdirectory(imgui_juce)
add_subdirectory(synth)

target_sources(BeepboxPlugin
    PRIVATE
        plugin/plugin_editor.cpp
        plugin/plugin_processor.cpp)

target_link_libraries(BeepboxPlugin
    PRIVATE
        juce::juce_audio_utils
        juce::juce_opengl
        imgui
        imgui_impl_juce
        beepbox_synth

    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)