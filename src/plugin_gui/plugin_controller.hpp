#pragma once

#include <atomic>
#include "include/plugin_gui.h"
#include "platform.hpp"
#include <imgui.h>

constexpr int GUI_EVENT_QUEUE_MASK = GUI_EVENT_QUEUE_SIZE-1;

template <typename T, size_t SIZE>
struct EventQueue {
private:
    T data[SIZE];
    std::atomic_size_t write_ptr;
    std::atomic_size_t read_ptr;

public:
    EventQueue() {};
    EventQueue(EventQueue&) = delete;
    EventQueue operator=(EventQueue&) = delete;

    void enqueue(const T &item) {
        uint32_t ptr = write_ptr.load();
        data[ptr] = item;
        ptr = (ptr + 1) & GUI_EVENT_QUEUE_MASK;
        write_ptr.store(ptr);
    }

    bool dequeue(T &item) {
        size_t read = read_ptr.load();
        size_t write = write_ptr.load();
        if (read == write) return false;

        item = data[read];
        read = (read + 1) & GUI_EVENT_QUEUE_MASK;
        read_ptr.store(read);

        return true;
    }
}; // struct EventQueue

// ImGui gui for plugin controller
class PluginController {
private:
    beepbox::inst_s *const instrument;

    bool showAbout;
    double params[BASE_PARAM_COUNT + FM_PARAM_COUNT];

    uint32_t envelope_count;
    beepbox::envelope_s envelopes[MAX_ENVELOPE_COUNT];

    void updateParams();
    void sliderParameter(uint32_t paramId, const char *id, float v_min, float v_max, const char *fmt = "%.3f", bool normalized = false);

    void drawAbout(ImGuiWindowFlags winFlags);
    void drawFmGui(ImGuiWindowFlags winFlags);

    void paramGestureBegin(uint32_t param_id);
    void paramChange(uint32_t param_id, double value);
    void paramGestureEnd(uint32_t param_id);
public:
    PluginController(beepbox::inst_s *instrument);

    static void graphicsInit();
    static void graphicsClose();

    void event(platform::Event ev, platform::Window *window);
    void draw(platform::Window *window);

    EventQueue<gui_event_queue_item_s, GUI_EVENT_QUEUE_SIZE> gui_to_plugin;
    EventQueue<gui_event_queue_item_s, GUI_EVENT_QUEUE_SIZE> plugin_to_gui;
}; // class PluginController