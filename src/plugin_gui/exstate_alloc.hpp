#pragma once
#include <atomic>
#include <cstdint>
#include "include/plugin_gui.h"

struct gui_event_exstate {
    std::atomic_bool free;
    std::uint8_t data[];
};
