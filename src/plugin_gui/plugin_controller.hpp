#pragma once

#include <imgui.h>
#include <vector>
#include "include/plugin_gui.h"
#include "platform.hpp"
#include "util.hpp"

constexpr int GUI_EVENT_QUEUE_MASK = GUI_EVENT_QUEUE_SIZE-1;

// ImGui gui for plugin controller
class PluginController {
private:
    bpbx_inst_s *const instrument;

    bool showAbout;
    double params[BPBX_BASE_PARAM_COUNT + BPBX_FM_PARAM_COUNT];
    
    float uiRightCol;
    void sameLineRightCol();

    std::vector<bpbx_envelope_s> envelopes;

    void updateParams();
    void updateColors(); // update style based on custom colors
    void sliderParameter(uint32_t paramId, const char *id, float v_min, float v_max, const char *fmt = "%.3f", bool normalized = false);

    void drawAbout(ImGuiWindowFlags winFlags);
    void drawFmGui();

    void drawFadeWidget(const char *id, ImVec2 size);
    void drawEnvelopes();

    void paramGestureBegin(uint32_t param_id);
    void paramChange(uint32_t param_id, double value);
    void paramGestureEnd(uint32_t param_id);

    int fadeDragMode;
    double fadeDragInit;
public:
    PluginController(bpbx_inst_s *instrument);

    static void graphicsInit();
    static void graphicsClose();

    void sync();
    void event(platform::Event ev, platform::Window *window);
    void draw(platform::Window *window);
    
    bool useCustomColors;
    Color customColor;

    EventQueue<gui_event_queue_item_s, GUI_EVENT_QUEUE_SIZE> gui_to_plugin;
    EventQueue<gui_event_queue_item_s, GUI_EVENT_QUEUE_SIZE> plugin_to_gui;
}; // class PluginController