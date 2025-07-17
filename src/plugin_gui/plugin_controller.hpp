#pragma once

#include <imgui.h>
#include <vector>
#include "include/plugin_gui.h"
#include "platform.hpp"
#include "util.hpp"
#include "gfx.hpp"

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

    bool updateParams();
    void updateColors(); // update style based on custom colors

    void paramControls(uint32_t paramId);
    void sliderParameter(uint32_t paramId, const char *id, float v_min, float v_max, const char *fmt = "%.3f", bool normalized = false);
    void vertSliderParameter(uint32_t paramId, const char *id, ImVec2 size, float v_min, float v_max, const char *fmt = "%.3f", bool normalized = false);

    void drawAbout(ImGuiWindowFlags winFlags);
    void drawFmGui();

    void drawFadeWidget(const char *id, ImVec2 size);
    void drawEnvelopes();
    void drawEffects();
    void drawModulationPad();

    void paramGestureBegin(uint32_t param_id);
    void paramChange(uint32_t param_id, double value);
    void paramGestureEnd(uint32_t param_id);

    int fadeDragMode;
    double fadeDragInit;

#ifdef PLUGIN_VST3
    struct {
        gfx::Texture *texture;
        int width;
        int height;
    } vstLogo;
#endif
public:
    show_context_menu_f popupContextMenu;
    void *pluginUserdata;

    PluginController(bpbx_inst_s *instrument);

    void graphicsInit();
    void graphicsClose();

    void sync();
    void event(platform::Event ev, platform::Window *window);
    void draw(platform::Window *window);
    
    bool useCustomColors;
    Color customColor;

    EventQueue<gui_event_queue_item_s, GUI_EVENT_QUEUE_SIZE> gui_to_plugin;
    EventQueue<gui_event_queue_item_s, GUI_EVENT_QUEUE_SIZE> plugin_to_gui;
}; // class PluginController