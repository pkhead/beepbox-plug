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
        PAGE_NOTE_FILTER,
        PAGE_CUSTOM_CHIP,
    };

    enum FilterType {
        FILTER_NOTE,
        FILTER_EQ,
    };

    const clap_plugin_t *const plugin;
    const clap_host_t *const host;
    instrument_s *const instrument;
    bbxs_context *const bb_context;

    Page currentPage;

    bool showAbout;
    bbxs_synth_type_e inst_type;
    std::vector<double> params;
    std::unordered_map<clap_id, int> paramIndexMap;
    std::int8_t chipWave[64];
    std::vector<bbxs_envelope_s> envelopes;

    std::vector<gui_event_queue_item_s> guiToPluginOverflow;
    std::vector<gui_event_exstate_s *> allocatedExstates;

    bool isExstateDirty;
    
    float uiRightCol;
    void sameLineRightCol();

    void sendToPlugin(const gui_event_queue_item_s &item);
    void paramGestureBegin(uint32_t param_id);
    void paramChange(uint32_t param_id, double value);
    void paramGestureEnd(uint32_t param_id);

    bool updateParams();
    void updateColors(); // update style based on custom colors

    bool idle();
    void postDraw();

    void paramControls(uint32_t paramId);
    void sliderParameter(uint32_t paramId, const char *id, float v_min, float v_max, const char *fmt = "%.3f", bool normalized = false);
    void vertSliderParameter(uint32_t paramId, const char *id, ImVec2 size, float v_min, float v_max, const char *fmt = "%.3f", bool normalized = false);
    void centeredText(const char *text);

    void drawAbout(ImGuiWindowFlags winFlags);
    void drawFmGui();
    void drawChipGui1();
    void drawChipGui2();
    void drawCustomChipGui1();
    void drawCustomChipGui2();
    void drawHarmonicsGui();
    void drawSpectrumGui();
    void drawNoiseGui1();
    void drawNoiseGui2();
    void drawPwmGui();
    void drawPickedStringGui();

    void drawFadeWidget(const char *id, ImVec2 size);
    void drawEqWidget(FilterType filter, const char *id, ImVec2 size);
    void drawEnvelopes();
    void drawEffects();
    void drawModulationPad();
    void drawHarmonicsEditor(const char *id, uint32_t paramId, ImVec2 size);
    void drawSpectrumEditor(const char *id, uint32_t baseParamId, ImVec2 size);
    void drawWaveEditor(const char *id, int8_t *wave, int waveLength, float uiWidth, float uiHeightScale = 1.f);
    void drawEqPage(FilterType targetFilter);

    void filterRemovePole(FilterType filter, int control_idx);
    void filterInsertPole(FilterType filter, int control_idx, bbxs_filter_type_e type, double freq, double gain);

    void applyCustomChipPreset(int presetIndex);

    int fadeDragMode;
    double fadeDragInit;

    bool showPanDelay;

    struct {
        ImGuiID widgetId;

        int activePoleIndex = -1;
        bool wasDragging = false;
        ImVec2 initialDragPos;
        std::vector<int> activeGestures;

        bbxs_filter_type_e draggingPoleType;
        bool draggingPoleExists;
        bool hasPoleBeenRemoved = false; // if the pole was ever removed during the drag
        bool poleIsNewlyAdded = false;
        bool forceDragBegin = false;
    } eqWidgetDragState;

    struct {
        bool activeGestures[BBXS_HARMONICS_CONTROL_COUNT] = {0};
    } harmonicsEditorState;

    struct {
        bool activeGestures[BBXS_SPECTRUM_CONTROL_COUNT] = {0};
    } spectrumEditorState;

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

    PluginController(const clap_plugin_t *plugin, const clap_host_t *host,
                     instrument_s *instrument, bbxs_context_s *bb_context);

    void graphicsInit();
    void graphicsClose();

    void sync();
    void event(platform::Event ev, platform::Window *window);
    void draw(platform::Window *window);

    void setParam(clap_id id, double value);
    double getParam(clap_id id) const;
    
    bool useCustomColors;
    Color customColor;
    std::atomic_bool needRefresh;

    EventQueue<gui_event_queue_item_s, GUI_EVENT_QUEUE_SIZE> gui_to_plugin;
    EventQueue<gui_event_queue_item_s, GUI_EVENT_QUEUE_SIZE> plugin_to_gui;
}; // class PluginController