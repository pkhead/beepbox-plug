#pragma once

#include <atomic>
#include "include/plugin_gui.h"
#include "platform.hpp"
#include <imgui.h>
#include <vector>

constexpr int GUI_EVENT_QUEUE_MASK = GUI_EVENT_QUEUE_SIZE-1;

struct Hsv {
public:
    float h;
    float s;
    float v;

    inline constexpr Hsv() : h(0.f), s(0.f), v(0.f) {}
    inline constexpr Hsv(float h, float s, float v) : h(h), s(s), v(v) {}
};

struct Color {
public:
    float r;
    float g;
    float b;

    inline constexpr Color() : r(0.f), g(0.f), b(0.f) {}
    inline constexpr Color(float r, float g = 0.f, float b = 0.f) noexcept : r(r), g(g), b(b) {}

    Hsv toHsv() const;
    static Color fromHsv(const Hsv &hsv);

    inline constexpr ImVec4 toImVec4(float a) {
        return ImVec4(r, g, b, a);
    }

    inline constexpr operator ImVec4() {
        return ImVec4(r, g, b, 1.0f);
    }
};

inline constexpr Color operator*(const Color &col, float mul) {
    return Color(col.r * mul, col.g * mul, col.b * mul);
}

inline constexpr Color operator/(const Color &col, float mul) {
    return Color(col.r / mul, col.g / mul, col.b / mul);
}

template <typename T>
struct EventQueue {
private:
    T *data;
    std::atomic_size_t write_ptr;
    std::atomic_size_t read_ptr;

public:
    EventQueue(size_t size) noexcept : data(new T[size]) {};
    EventQueue(EventQueue&) = delete;
    EventQueue operator=(EventQueue&) = delete;
    ~EventQueue() noexcept { delete[] data; }

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
    bpbx_inst_s *const instrument;

    bool showAbout;
    double params[BPBX_BASE_PARAM_COUNT + BPBX_FM_PARAM_COUNT];

    std::vector<bpbx_envelope_s> envelopes;

    void updateParams();
    void updateColors(); // update style based on custom colors
    void sliderParameter(uint32_t paramId, const char *id, float v_min, float v_max, const char *fmt = "%.3f", bool normalized = false);

    void drawAbout(ImGuiWindowFlags winFlags);
    void drawFmGui(ImGuiWindowFlags winFlags);

    void drawEnvelopes();

    void paramGestureBegin(uint32_t param_id);
    void paramChange(uint32_t param_id, double value);
    void paramGestureEnd(uint32_t param_id);
public:
    PluginController(bpbx_inst_s *instrument);

    static void graphicsInit();
    static void graphicsClose();

    void sync();
    void event(platform::Event ev, platform::Window *window);
    void draw(platform::Window *window);
    
    bool useCustomColors;
    Color customColor;

    EventQueue<gui_event_queue_item_s> gui_to_plugin;
    EventQueue<gui_event_queue_item_s> plugin_to_gui;
}; // class PluginController