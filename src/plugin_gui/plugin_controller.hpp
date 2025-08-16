#pragma once

#include <imgui.h>
#include <vector>
#include <unordered_map>
#include "include/plugin_gui.h"
#include "platform.hpp"
#include "util.hpp"
#include "gfx.hpp"

constexpr int GUI_EVENT_QUEUE_MASK = GUI_EVENT_QUEUE_SIZE-1;

// ImGui gui for plugin controller
class PluginController {
private:
    enum Page {
        PAGE_MAIN,
        PAGE_EQ,
        PAGE_NOTE_FILTER
    };

    enum FilterType {
        FILTER_NOTE,
        FILTER_EQ,
    };

    const clap_plugin_t *const plugin;
    const clap_host_t *const host;
    instrument_s *const instrument;
    Page currentPage;

    bool showAbout;
    bpbxsyn_synth_type_e inst_type;
    std::unordered_map<clap_id, double> params;
    
    float uiRightCol;
    void sameLineRightCol();

    std::vector<bpbxsyn_envelope_s> envelopes;

    bool updateParams();
    void updateColors(); // update style based on custom colors

    void queueCheck();
    void paramControls(uint32_t paramId);
    void sliderParameter(uint32_t paramId, const char *id, float v_min, float v_max, const char *fmt = "%.3f", bool normalized = false);
    void vertSliderParameter(uint32_t paramId, const char *id, ImVec2 size, float v_min, float v_max, const char *fmt = "%.3f", bool normalized = false);

    void drawAbout(ImGuiWindowFlags winFlags);
    void drawFmGui();
    void drawChipGui1();
    void drawChipGui2();
    void drawHarmonicsGui();

    void drawFadeWidget(const char *id, ImVec2 size);
    void drawEqWidget(FilterType filter, const char *id, ImVec2 size);
    void drawEnvelopes();
    void drawEffects();
    void drawModulationPad();
    void drawHarmonicsEditor(const char *id, uint32_t paramId, ImVec2 size);
    void drawEqPage(FilterType targetFilter);

    void paramGestureBegin(uint32_t param_id);
    void paramChange(uint32_t param_id, double value);
    void paramGestureEnd(uint32_t param_id);

    void filterRemovePole(FilterType filter, int control_idx);
    void filterInsertPole(FilterType filter, int control_idx, bpbxsyn_filter_type_e type, double freq, double gain);

    int fadeDragMode;
    double fadeDragInit;

    struct {
        int activePoleIndex = -1;
        bool wasDragging = false;
        ImVec2 initialDragPos;
        std::vector<int> activeGestures;

        bpbxsyn_filter_type_e draggingPoleType;
        bool draggingPoleExists;
        bool hasPoleBeenRemoved = false; // if the pole was ever removed during the drag
        bool poleIsNewlyAdded = false;
        bool forceDragBegin = false;
    } eqWidgetDragState;

    struct {
        bool activeGestures[BPBXSYN_HARMONICS_CONTROL_COUNT] = {0};
    } harmonicsEditorState;

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

    PluginController(const clap_plugin_t *plugin, const clap_host_t *host, instrument_s *instrument);

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