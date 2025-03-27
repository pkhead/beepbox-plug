cmake_minimum_required(VERSION 3.22)
project(beepbox_synth VERSION 0.0.1)

add_library(beepbox_synth STATIC
    src/fm_algo.c
    src/fm.c
    src/instrument.c
    src/wavetables.c
    src/envelope.c
)

set_target_properties(beepbox_synth PROPERTIES
    LANGUAGE C
    C_STANDARD 11
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(beepbox_synth
    PRIVATE src
    PUBLIC include
)
