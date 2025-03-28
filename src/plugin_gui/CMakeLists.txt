if (PLUGIN_VST3)
    set(PROJECT_NAME plugin_gui_vst3)
else()
    set(PROJECT_NAME plugin_gui)
endif()

project(${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC
    gui.cpp
    lib_impl.cpp
    platform.cpp
    plugin_controller.cpp
)

if(PLUGIN_VST3)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PLUGIN_VST3)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    LANGUAGE CXX
)
target_include_directories(${PROJECT_NAME}
    PRIVATE ../sokol ../imgui ../gui_libs
    PUBLIC include
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE imgui
)