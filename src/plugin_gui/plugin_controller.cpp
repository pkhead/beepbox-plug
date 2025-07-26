#include <imgui.h>
#include <stb_image.h>
#include <cstring>

#include <cbeepsynth/include/beepbox_synth.h>
#include <clap/clap.h>
#include <vector>
#include <algorithm>
#include "plugin_controller.hpp"
#include "util.hpp"
#include "log.hpp"
#include <plugin/include/clap_plugin.h>

#ifdef PLUGIN_VST3
#include "resource/vst_logo.hpp"
#endif

#define FILTER_PARAM_TYPE(controlIndex) ((controlIndex) * 3)
#define FILTER_PARAM_FREQ(controlIndex) ((controlIndex) * 3 + 1)
#define FILTER_PARAM_GAIN(controlIndex) ((controlIndex) * 3 + 2)

void PluginController::graphicsInit() {
    #ifdef PLUGIN_VST3
    int width, height, channels;
    uint8_t *imageData = stbi_load_from_memory(resources::vst_logo, resources::vst_logo_len, &width, &height, &channels, 4);
    if (imageData == nullptr) {
        vstLogo.texture = nullptr;
        vstLogo.width = 0;
        vstLogo.height = 0;
        return;
    }

    vstLogo.texture = gfx::createTexture(imageData, width, height);
    vstLogo.width = width;
    vstLogo.height = height;

    stbi_image_free(imageData);
    #endif
}

void PluginController::graphicsClose() {
    #ifdef PLUGIN_VST3
    gfx::destroyTexture(vstLogo.texture);
    #endif
}

PluginController::PluginController(const clap_plugin_t *plugin, const clap_host_t *host, bpbx_synth_s *instrument) :
    plugin(plugin),
    host(host),
    instrument(instrument),
    plugin_to_gui(),
    gui_to_plugin()
{
    showAbout = false;
    useCustomColors = false;
    currentPage = PAGE_MAIN;
    inst_type = bpbx_synth_type(instrument);

    // initialize copy of plugin state
    sync();
}

void PluginController::sync() {
    bpbx_synth_type_e type = bpbx_synth_type(instrument);
    
    uint32_t param_count = plugin_params_count(plugin);
    params.reserve(param_count * 2.0);

    for (int i = 0; i < param_count; i++) {
        clap_param_info_t param_info;
        
        if (!plugin_params_get_info(plugin, i, &param_info)) {
            log_error("could not initialize parameter #%i because get_info failed", i);
            continue;
        }

        double param_value;
        if (!plugin_params_get_value(plugin, param_info.id, &param_value)) {
            log_error("could not initialize parameter #%i because get_value failed", i);
        }

        params[param_info.id] = param_value;
    }

    envelopes.clear();
    envelopes.reserve(BPBX_MAX_ENVELOPE_COUNT);
    envelopes.resize(bpbx_synth_envelope_count(instrument));
    memcpy(envelopes.data(), bpbx_synth_get_envelope(instrument, 0), sizeof(bpbx_envelope_s) * envelopes.size());
}

bool PluginController::updateParams() {
    bool didWork = false;
    gui_event_queue_item_s item;

    while (plugin_to_gui.dequeue(item)) {
        switch (item.type) {
            case GUI_EVENT_PARAM_CHANGE:
                params[item.param_value.param_id] = item.param_value.value;
                didWork = true;
                break;

            default:
                break;
        }
    }

    return didWork;
}

void PluginController::paramControls(uint32_t paramId) {
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right) && popupContextMenu) {
        ImVec2 mousePos = ImGui::GetMousePos();
        popupContextMenu((int)mousePos.x, (int)mousePos.y, paramId, pluginUserdata);
    }
}

void PluginController::sliderParameter(uint32_t paramId, const char *id, float v_min, float v_max, const char *fmt, bool normalized) {
    double v = params[paramId];
    float floatV = (float)v;
    if (normalized) {
        floatV = (v_max - v_min) * floatV + v_min;
    }

    bool changed = ImGui::SliderFloat(id, &floatV, v_min, v_max, fmt);

    if (ImGui::IsItemActivated()) {
        paramGestureBegin(paramId);
    }

    if (changed) {
        if (normalized) {
            floatV = (floatV - v_min) / (v_max - v_min);
        }

        paramChange(paramId, (double)floatV);
    }

    if (ImGui::IsItemDeactivated()) {
        paramGestureEnd(paramId);
    }

    paramControls(paramId);
}

void PluginController::vertSliderParameter(uint32_t paramId, const char *id, ImVec2 size, float v_min, float v_max, const char *fmt, bool normalized) {
    double v = params[paramId];
    float floatV = (float)v;
    if (normalized) {
        floatV = (v_max - v_min) * floatV + v_min;
    }

    bool changed = ImGui::VSliderFloat(id, size, &floatV, v_min, v_max, fmt);

    if (ImGui::IsItemActivated()) {
        paramGestureBegin(paramId);
    }

    if (changed) {
        if (normalized) {
            floatV = (floatV - v_min) / (v_max - v_min);
        }

        paramChange(paramId, (double)floatV);
    }

    if (ImGui::IsItemDeactivated()) {
        paramGestureEnd(paramId);
    }

    paramControls(paramId);
}

void PluginController::queueCheck() {
    // request process in case plugin is sleeping
    if (host->request_process)
        host->request_process(host);
}

void PluginController::paramGestureBegin(uint32_t param_id) {
    gui_event_queue_item_s item = {};
    item.type = GUI_EVENT_PARAM_GESTURE_BEGIN;
    item.gesture.param_id = param_id;

    gui_to_plugin.enqueue(item);
    queueCheck();

#ifndef _NDEBUG
    log_debug("paramGestureBegin %i", param_id);
#endif
}

void PluginController::paramChange(uint32_t param_id, double value) {
    gui_event_queue_item_s item = {};
    item.type = GUI_EVENT_PARAM_CHANGE;
    item.param_value.param_id = param_id;
    item.param_value.value = value;

    gui_to_plugin.enqueue(item);
    queueCheck();

    params[param_id] = value;
}

void PluginController::paramGestureEnd(uint32_t param_id) {
    gui_event_queue_item_s item = {};
    item.type = GUI_EVENT_PARAM_GESTURE_END;
    item.gesture.param_id = param_id;

    gui_to_plugin.enqueue(item);
    queueCheck();

#ifndef _NDEBUG
    log_debug("paramGestureEnd %i", param_id);
#endif
}

void PluginController::event(platform::Event ev, platform::Window *window) {
    if (updateParams())
        platform::requestRedraw(window);
}

void PluginController::drawAbout(ImGuiWindowFlags winFlags) {
    ImGui::Begin("about", NULL, winFlags);
    ImGui::TextWrapped("Version: " PLUGIN_VERSION);

    ImGui::TextWrapped("Port of BeepBox instruments.");
    ImGui::TextWrapped("Ported by pkhead, designed/programmed by John Nesky and various BeepBox community members.");

    ImGui::TextLinkOpenURL("BeepBox website", "https://beepbox.co");
    ImGui::TextLinkOpenURL("Source code", "https://github.com/pkhead/beepbox-plug");

    ImGui::Text("Libraries:");
    ImGui::Bullet();
    ImGui::TextLinkOpenURL("clap-wrapper", "https://github.com/free-audio/clap-wrapper/tree/main");
    ImGui::Bullet();
    ImGui::TextLinkOpenURL("Dear ImGui", "https://github.com/ocornut/imgui");

    // show vst3-compatible logo
    #ifdef PLUGIN_VST3
    ImGui::NewLine();
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - (float)vstLogo.width) / 2.f);
    ImGui::Image(gfx::imguiTextureId(vstLogo.texture), ImVec2((float)vstLogo.width, (float)vstLogo.height));
    #endif

    ImGui::End();
}

void PluginController::drawFmGui() {
    // operator parameters
    static const char *name[] = {
        "1.", "2.", "3.", "4."
    };

    int p_algo = (int) params[BPBX_FM_PARAM_ALGORITHM];
    int p_freq[4] = {
        (int) params[BPBX_FM_PARAM_FREQ1],
        (int) params[BPBX_FM_PARAM_FREQ2],
        (int) params[BPBX_FM_PARAM_FREQ3],
        (int) params[BPBX_FM_PARAM_FREQ4],
    };
    int p_fdbkType = (int) params[BPBX_FM_PARAM_FEEDBACK_TYPE];

    // algorithm
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Algorithm");

    sameLineRightCol();
    float algoEndX = ImGui::GetCursorPosX();
    ImGui::SetNextItemWidth(-FLT_MIN);

    {
        const bpbx_param_info_s *p_info = bpbx_param_info(bpbx_synth_type(instrument), BPBX_FM_PARAM_ALGORITHM);
        assert(p_info);
        const char **algoNames = p_info->enum_values;
        if (ImGui::Combo("##algo", &p_algo, algoNames, 13)) {
            paramGestureBegin(BPBX_FM_PARAM_ALGORITHM);
            paramChange(BPBX_FM_PARAM_ALGORITHM, (double)p_algo);
            paramGestureEnd(BPBX_FM_PARAM_ALGORITHM);
        }

        paramControls(BPBX_FM_PARAM_ALGORITHM);
    }

    // operator parameters
    for (int op = 0; op < 4; op++) {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", name[op]);
        ImGui::PushID(op);

        ImGui::SameLine();
        ImGui::SetNextItemWidth(algoEndX - ImGui::GetCursorPosX() - ImGui::GetStyle().ItemSpacing.x);
        //ImGui::Text("%i", gui->freq[op]);

        const uint32_t id = BPBX_FM_PARAM_FREQ1 + op * 2;

        const bpbx_param_info_s *p_info = bpbx_param_info(bpbx_synth_type(instrument), id);
        assert(p_info);
        const char **freqRatios = p_info->enum_values;

        if (ImGui::Combo("##freq", &p_freq[op], freqRatios, BPBX_FM_FREQ_COUNT, ImGuiComboFlags_HeightLargest)) {
            paramGestureBegin(id);
            paramChange(id, p_freq[op]);
            paramGestureEnd(id);
        }
        paramControls(BPBX_FM_PARAM_FREQ1 + op * 2);

        sameLineRightCol();
        ImGui::SetNextItemWidth(-FLT_MIN);
        sliderParameter(BPBX_FM_PARAM_VOLUME1 + op*2, "##vol", 0.0f, 15.0f, "%.0f");
        ImGui::PopID();
    }

    // feedback type
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Feedback");

    sameLineRightCol();
    float feedbackEndX = ImGui::GetCursorPosX();
    ImGui::SetNextItemWidth(-FLT_MIN);
    {
        const bpbx_param_info_s *p_info = bpbx_param_info(bpbx_synth_type(instrument), BPBX_FM_PARAM_FEEDBACK_TYPE);
        assert(p_info);
        const char **feedbackNames = p_info->enum_values;

        if (ImGui::Combo("##fdbk", &p_fdbkType, feedbackNames, BPBX_FM_FEEDBACK_TYPE_COUNT)) {
            paramGestureBegin(BPBX_FM_PARAM_FEEDBACK_TYPE);
            paramChange(BPBX_FM_PARAM_FEEDBACK_TYPE, (double)p_fdbkType);
            paramGestureEnd(BPBX_FM_PARAM_FEEDBACK_TYPE);
        }

        paramControls(BPBX_FM_PARAM_FEEDBACK_TYPE);
    }

    // feedback volume
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Fdback Vol");
    
    sameLineRightCol();
    ImGui::SetCursorPosX(feedbackEndX);
    ImGui::SetNextItemWidth(-FLT_MIN);
    sliderParameter(BPBX_FM_PARAM_FEEDBACK_VOLUME, "##vol", 0.0f, 15.0f, "%.0f");
}

void PluginController::drawChipGui1() {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Wave");

    sameLineRightCol();
    ImGui::SetNextItemWidth(-FLT_MIN);
    {
        int p_wave = params[BPBX_CHIP_PARAM_WAVEFORM];
        const bpbx_param_info_s *p_info = bpbx_param_info(bpbx_synth_type(instrument), BPBX_CHIP_PARAM_WAVEFORM);
        assert(p_info);
        const char **waveNames = p_info->enum_values;
        if (ImGui::Combo("##wave", &p_wave, waveNames, BPBX_CHIP_WAVE_COUNT)) {
            paramGestureBegin(BPBX_CHIP_PARAM_WAVEFORM);
            paramChange(BPBX_CHIP_PARAM_WAVEFORM, (double)p_wave);
            paramGestureEnd(BPBX_CHIP_PARAM_WAVEFORM);
        }

        paramControls(BPBX_CHIP_PARAM_WAVEFORM);
    }
}

void PluginController::drawChipGui2() {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Unison");

    sameLineRightCol();
    ImGui::SetNextItemWidth(-FLT_MIN);
    {
        int p_unison = params[BPBX_CHIP_PARAM_UNISON];
        const bpbx_param_info_s *p_info = bpbx_param_info(bpbx_synth_type(instrument), BPBX_CHIP_PARAM_UNISON);
        assert(p_info);
        const char **unisonNames = p_info->enum_values;
        if (ImGui::Combo("##unison", &p_unison, unisonNames, BPBX_UNISON_COUNT)) {
            paramGestureBegin(BPBX_CHIP_PARAM_UNISON);
            paramChange(BPBX_CHIP_PARAM_UNISON, (double)p_unison);
            paramGestureEnd(BPBX_CHIP_PARAM_UNISON);
        }

        paramControls(BPBX_CHIP_PARAM_UNISON);
    }
}

void PluginController::drawHarmonicsGui() {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Harmonics");

    sameLineRightCol();
    drawHarmonicsEditor("harmonicsctl", BPBX_HARMONICS_PARAM_CONTROL_FIRST, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight() * 1.75f));

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Unison");

    sameLineRightCol();
    ImGui::SetNextItemWidth(-FLT_MIN);
    {
        int p_unison = params[BPBX_HARMONICS_PARAM_UNISON];
        const bpbx_param_info_s *p_info = bpbx_param_info(bpbx_synth_type(instrument), BPBX_HARMONICS_PARAM_UNISON);
        assert(p_info);
        const char **unisonNames = p_info->enum_values;
        if (ImGui::Combo("##unison", &p_unison, unisonNames, BPBX_UNISON_COUNT)) {
            paramGestureBegin(BPBX_HARMONICS_PARAM_UNISON);
            paramChange(BPBX_HARMONICS_PARAM_UNISON, (double)p_unison);
            paramGestureEnd(BPBX_HARMONICS_PARAM_UNISON);
        }

        paramControls(BPBX_HARMONICS_PARAM_UNISON);
    }
}

void PluginController::drawEffects() {
    ImGui::AlignTextToFramePadding();
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Effects").x) / 2.f);
    ImGui::Text("Effects");

    ImGui::SameLine();
    // why do i need to subtract one pixel
    ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - ImGui::GetFrameHeight() - 1);

    //if (ImGui::Button("v##ToggleEffects", ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()))) {
    if (ImGui::ArrowButton("##ToggleEffects", ImGuiDir_Down)) {
        ImGui::OpenPopup("EffectsMenu");
    }

    if (ImGui::BeginPopup("EffectsMenu")) {
        static const char *effectNames[] = {"transition type", "chord type", "pitch shift", "detune", "vibrato", "note filter"};

        for (int i = 0; i < 6; i++) {
            unsigned int toggle_param = bpbx_effect_toggle_param((bpbx_synthfx_type_e) i);
            bool is_active = params[toggle_param] != 0.0;

            if (ImGui::Selectable(effectNames[i], is_active)) {
                paramGestureBegin(toggle_param);
                paramChange(toggle_param, (!is_active) ? 1.0 : 0.0);
                paramGestureEnd(toggle_param);

                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }

    // transition type

    // chord type
    if (params[BPBX_PARAM_ENABLE_CHORD_TYPE] != 0.0) {
        static const char *chordTypes[] = {"simultaneous", "strum", "arpeggio", "custom interval"};

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Chords");
        sameLineRightCol();
        ImGui::SetNextItemWidth(-FLT_MIN);

        int curItem = (int)params[BPBX_PARAM_CHORD_TYPE];
        if (ImGui::Combo("##ChordTypes", &curItem, chordTypes, 4)) {
            paramGestureBegin(BPBX_PARAM_CHORD_TYPE);
            paramChange(BPBX_PARAM_CHORD_TYPE, (double)curItem);
            paramGestureEnd(BPBX_PARAM_CHORD_TYPE);
        }
        paramControls(BPBX_PARAM_CHORD_TYPE);
    }

    if (params[BPBX_PARAM_CHORD_TYPE] == BPBX_CHORD_TYPE_ARPEGGIO) {
        // arpeggio speed
        ImGui::AlignTextToFramePadding();
        ImGui::Bullet();

        ImGui::Text("Speed");
        sameLineRightCol();
        ImGui::SetNextItemWidth(-FLT_MIN);

        const bpbx_param_info_s *param_info = bpbx_param_info(inst_type, BPBX_PARAM_ARPEGGIO_SPEED);
        sliderParameter(BPBX_PARAM_ARPEGGIO_SPEED, "##ArpeggioSpeed", 0.f, 50.f, param_info->enum_values[(int)params[BPBX_PARAM_ARPEGGIO_SPEED]]);

        // fast two-note option
        ImGui::AlignTextToFramePadding();
        ImGui::Bullet();

        ImGui::Text("Fast Two-Note");
        ImGui::SameLine();

        bool fast_two_note = params[BPBX_PARAM_FAST_TWO_NOTE_ARPEGGIO] != 0.0;
        if (ImGui::Checkbox("##FastTwoNote", &fast_two_note)) {
            paramGestureBegin(BPBX_PARAM_FAST_TWO_NOTE_ARPEGGIO);
            paramChange(BPBX_PARAM_FAST_TWO_NOTE_ARPEGGIO, fast_two_note ? 1.0 : 0.0);
            paramGestureEnd(BPBX_PARAM_FAST_TWO_NOTE_ARPEGGIO);
        }
    }

    // pitch shift
    if (params[BPBX_PARAM_ENABLE_PITCH_SHIFT] != 0.0) {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Pitch Shift");
        sameLineRightCol();
        ImGui::SetNextItemWidth(-FLT_MIN);

        sliderParameter(BPBX_PARAM_PITCH_SHIFT, "##Pitch Shift", -12.f, 12.f, "%.0f");
    }

    // detune
    if (params[BPBX_PARAM_ENABLE_DETUNE] != 0.0) {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Detune");
        sameLineRightCol();
        ImGui::SetNextItemWidth(-FLT_MIN);

        sliderParameter(BPBX_PARAM_DETUNE, "##Detune", -200.f, 200.f, "%.0f");
    }

    // vibrato
    if (params[BPBX_PARAM_ENABLE_VIBRATO] != 0.0) {
        static const char *vibratoPresets[] = {"none", "light", "delayed", "heavy", "shaky", "custom"};

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Vibrato");
        sameLineRightCol();
        ImGui::SetNextItemWidth(-FLT_MIN);

        int curItem = (int)params[BPBX_PARAM_VIBRATO_PRESET];
        if (ImGui::Combo("##VibratoPresets", &curItem, vibratoPresets, 6)) {
            paramGestureBegin(BPBX_PARAM_VIBRATO_PRESET);
            paramChange(BPBX_PARAM_VIBRATO_PRESET, (double)curItem);
            paramGestureEnd(BPBX_PARAM_VIBRATO_PRESET);
        }
        paramControls(BPBX_PARAM_VIBRATO_PRESET);

        // show custom vibrato
        if (params[BPBX_PARAM_VIBRATO_PRESET] == BPBX_VIBRATO_PRESET_CUSTOM) {
            // vibrato depth
            ImGui::AlignTextToFramePadding();
            ImGui::Bullet();
            ImGui::Text("Depth");

            sameLineRightCol();
            ImGui::SetNextItemWidth(-FLT_MIN);
            sliderParameter(BPBX_PARAM_VIBRATO_DEPTH, "##VibratoDepth", 0.f, 2.f, "%.2f");

            // vibrato speed
            ImGui::AlignTextToFramePadding();
            ImGui::Bullet();
            ImGui::Text("Speed");

            sameLineRightCol();
            ImGui::SetNextItemWidth(-FLT_MIN);
            sliderParameter(BPBX_PARAM_VIBRATO_SPEED, "##VibratoSpeed", 0.f, 3.f, "×%.1f");

            // vibrato delay
            ImGui::AlignTextToFramePadding();
            ImGui::Bullet();
            ImGui::Text("Delay");

            sameLineRightCol();
            ImGui::SetNextItemWidth(-FLT_MIN);
            sliderParameter(BPBX_PARAM_VIBRATO_DELAY, "##VibratoDelay", 0.f, 50.f, "%.0f");

            // vibrato type
            ImGui::AlignTextToFramePadding();
            ImGui::Bullet();
            ImGui::Text("Type");

            int vibratoType = (int) params[BPBX_PARAM_VIBRATO_TYPE];

            sameLineRightCol();
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::Combo("##VibratoType", &vibratoType, "normal\0shaky\0")) {
                paramGestureBegin(BPBX_PARAM_VIBRATO_TYPE);
                paramChange(BPBX_PARAM_VIBRATO_TYPE, (double)vibratoType);
                paramGestureEnd(BPBX_PARAM_VIBRATO_TYPE);
            }
            paramControls(BPBX_PARAM_VIBRATO_TYPE);
        }
    }

    // note filter
    if (params[BPBX_PARAM_ENABLE_NOTE_FILTER] != 0.0) {
        ImGui::AlignTextToFramePadding();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImGui::GetStyle().ItemInnerSpacing);
        ImGui::Text("Note Filt");

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.f, 0.f));
        ImGui::SameLine();
        if (ImGui::SmallButton("+")) {
            currentPage = PAGE_NOTE_FILTER;
        }
        ImGui::PopStyleVar(2);

        sameLineRightCol();
        drawEqWidget(FILTER_NOTE, "eqctl", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight() * 1.75f));
    }
}

void PluginController::drawEnvelopes() {
    ImGuiStyle &style = ImGui::GetStyle();

    ImGui::AlignTextToFramePadding();
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Envelopes").x) / 2.f);
    ImGui::Text("Envelopes");

    if (envelopes.size() < BPBX_MAX_ENVELOPE_COUNT) {
        ImGui::SameLine();
        // why do i need to subtract one pixel
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - ImGui::GetFrameHeight() - 1);

        if (ImGui::Button("+##AddEnvelope", ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()))) {
            bpbx_envelope_s new_env {};
            new_env.index = BPBX_ENV_INDEX_NONE;
            new_env.curve_preset = 0,
            envelopes.push_back(new_env);

            gui_event_queue_item_s queue_item {};
            queue_item.type = GUI_EVENT_ADD_ENVELOPE;
            gui_to_plugin.enqueue(queue_item);
        }
    }

    bpbx_synth_type_e instType = bpbx_synth_type(instrument);
    const char **curveNames = bpbx_envelope_curve_preset_names();

    for (int envIndex = 0; envIndex < envelopes.size(); envIndex++) {
        bpbx_envelope_s &env = envelopes[envIndex];
        bool isEnvDirty = false;

        const char *envTargetStr = bpbx_envelope_index_name(env.index);
        assert(envTargetStr);

        ImGui::PushID(envIndex);

        // there will be a square deletion button at the right side of each line.
        // then, i want to have each combobox take half of the available content width,
        // and also have X spacing inbetween all items be the same as inner spacing X
        float itemWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetFrameHeight() - style.ItemInnerSpacing.x * 2) / 2.f;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemInnerSpacing.x, style.ItemSpacing.y));
        
        // envelope target combobox
        ImGui::SetNextItemWidth(itemWidth);
        if (ImGui::BeginCombo("##target", envTargetStr)) {
            std::vector<bpbx_envelope_compute_index_e> targets;
            targets.push_back(BPBX_ENV_INDEX_NONE);
            targets.push_back(BPBX_ENV_INDEX_NOTE_VOLUME);

            {
                int size;
                const bpbx_envelope_compute_index_e *instTargets = bpbx_envelope_targets(instType, &size);
                targets.reserve(targets.size() + size);

                for (int i = 0; i < size; i++) {
                    targets.push_back(instTargets[i]);
                }

                if (params[BPBX_PARAM_ENABLE_PITCH_SHIFT])
                    targets.push_back(BPBX_ENV_INDEX_PITCH_SHIFT);

                if (params[BPBX_PARAM_ENABLE_DETUNE])
                    targets.push_back(BPBX_ENV_INDEX_DETUNE);

                if (params[BPBX_PARAM_ENABLE_VIBRATO])
                    targets.push_back(BPBX_ENV_INDEX_VIBRATO_DEPTH);

                if (params[BPBX_PARAM_ENABLE_NOTE_FILTER]) {
                    targets.push_back(BPBX_ENV_INDEX_NOTE_FILTER_ALL_FREQS);

                    for (int i = 0; i < BPBX_FILTER_GROUP_COUNT; i++) {
                        int filtType = (int)params[BPBX_PARAM_NOTE_FILTER_TYPE0 + FILTER_PARAM_TYPE(i)]; 
                        if (filtType != BPBX_FILTER_TYPE_OFF) {
                            targets.push_back(
                                (bpbx_envelope_compute_index_e)(BPBX_ENV_INDEX_NOTE_FILTER_FREQ0 + i));
                        }
                    }
                }
            }

            for (int i = 0; i < targets.size(); i++) {
                const char *name = bpbx_envelope_index_name(targets[i]);
                assert(name);

                bool isSelected = env.index == targets[i];
                if (ImGui::Selectable(name, isSelected)) {
                    env.index = targets[i];
                    isEnvDirty = true;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        // envelope curve preset combobox
        ImGui::SetNextItemWidth(itemWidth);
        ImGui::SameLine();
        if (ImGui::BeginCombo("##curve", curveNames[env.curve_preset])) {
            for (int i = 0; i < BPBX_ENVELOPE_CURVE_PRESET_COUNT; i++) {
                if (i == 1) continue; // skip note size...
                bool isSelected = env.curve_preset == i;
                if (ImGui::Selectable(curveNames[i], isSelected)) {
                    env.curve_preset = i;
                    isEnvDirty = true;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        // envelope deletion button
        ImGui::SameLine();
        if (ImGui::Button("×", ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()))) {
            gui_event_queue_item_s queue_item {};
            queue_item.type = GUI_EVENT_REMOVE_ENVELOPE;
            queue_item.envelope_removal.index = envIndex;
            gui_to_plugin.enqueue(queue_item);

            envelopes.erase(envelopes.begin() + envIndex);
            envIndex--;

            // don't want to modify a deleted envelope...
            isEnvDirty = false;
        }
        
        ImGui::PopID();
        ImGui::PopStyleVar();

        if (isEnvDirty) {
            gui_event_queue_item_s queue_item {};
            queue_item.type = GUI_EVENT_MODIFY_ENVELOPE;
            queue_item.modify_envelope.index = envIndex;
            queue_item.modify_envelope.envelope = env;
            gui_to_plugin.enqueue(queue_item);
        }
    }
}

static void drawDashedHLine(
    ImDrawList *drawList, float startX, float endX, float y,
    float segmentWidth, float spacing,
    ImU32 col
) {
    if (endX < startX) {
        float temp = startX;
        startX = endX;
        endX = temp;
    }

    // int segmentCount = (int)ceilf((endX - startX) / (segmentWidth + spacing));
    // float x = startX;
    // for (int i = 0; i < segmentCount; i++) {
    for (float x = startX; x <= endX; x += segmentWidth + spacing) {
        float segmentEndX = x + segmentWidth;
        if (segmentEndX > endX)
            segmentEndX = endX;
    
        drawList->AddLine(ImVec2(x, y), ImVec2(segmentEndX, y), col);
        // x += segmentWidth + spacing;
    }
}

void PluginController::drawFadeWidget(const char *id, ImVec2 size) {
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec2 ui_origin = ImGui::GetCursorScreenPos();
    ImVec2 ui_end = ImVec2(ui_origin.x + size.x, ui_origin.y + size.y);

    constexpr int FADE_IN_MAX = 9;
    constexpr int FADE_OUT_MAX = 6;
    constexpr int FADE_OUT_MIN = -4;

    const int steps = FADE_OUT_MAX - FADE_OUT_MIN + FADE_IN_MAX + 2;
    const float pixelsPerStep = size.x / steps;

    const float fadeInVal =  (float) params[BPBX_PARAM_FADE_IN];
    const float fadeOutVal = (float) params[BPBX_PARAM_FADE_OUT];

    float fadeInX = ui_origin.x + pixelsPerStep * fadeInVal;
    float fadeOutRestX = ui_end.x - pixelsPerStep * FADE_OUT_MAX;
    float fadeOutX = fadeOutRestX + pixelsPerStep * fadeOutVal;

    ImU32 barColor = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
    ImU32 graphColor = ImGui::GetColorU32(ImGuiCol_Header);

    // draw "graph"
    {
        // fade in triangle
        if (fadeInVal > 0) {
            drawList->AddTriangleFilled(
                ImVec2(fadeInX, ui_origin.y),
                ImVec2(fadeInX, ui_end.y),
                ImVec2(ui_origin.x, ui_end.y),
                graphColor
            );
        }

        // the rect inbetween fade in and fade out
        drawList->AddRectFilled(
            ImVec2(fadeInX, ui_origin.y),
            ImVec2(min(fadeOutX, fadeOutRestX), ui_end.y),
            graphColor
        );

        // fade out triangle
        if (fadeOutVal > 0) {
            drawList->AddTriangleFilled(
                ImVec2(fadeOutRestX, ui_origin.y),
                ImVec2(fadeOutX, ui_end.y),
                ImVec2(fadeOutRestX, ui_end.y),
                graphColor
            );
        } else if (fadeOutVal < 0) {
            drawList->AddTriangleFilled(
                ImVec2(fadeOutX, ui_origin.y),
                ImVec2(fadeOutRestX, ui_end.y),
                ImVec2(fadeOutX, ui_end.y),
                graphColor
            );
        }
    }

    // draw fade in bar
    drawList->AddRectFilled(
        ImVec2(fadeInX, ui_origin.y),
        ImVec2(fadeInX + 2.f, ui_end.y),
        barColor
    );

    // draw dashed line for fade out rest bar
    float dashHeight = 4.f;
    float dashSpaceHeight = 3.f;
    int i = 0;
    for (float y = ui_origin.y; y < ui_end.y;) {
        if (i++ % 2 == 0) {
            float endY = clamp(y + dashHeight, ui_origin.y, ui_end.y);
            drawList->AddRectFilled(
                ImVec2(fadeOutRestX - 1.f, y),
                ImVec2(fadeOutRestX, endY),
                barColor
            );

            y += dashHeight;
        } else {
            y += dashSpaceHeight;
        }
    }

    // draw fade out bar
    drawList->AddRectFilled(
        ImVec2(fadeOutX - 2.f, ui_origin.y),
        ImVec2(fadeOutX, ui_end.y),
        barColor
    );

    // ui controls
    ImGui::InvisibleButton(id, size);

    if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }

    if (ImGui::IsItemActivated()) {
        float centerX = ui_origin.x + pixelsPerStep * (FADE_IN_MAX + 1);
        float mouseX = ImGui::GetMousePos().x;

        if (mouseX < centerX) {
            fadeDragMode = 1;
            fadeDragInit = params[BPBX_PARAM_FADE_IN];
            paramGestureBegin(BPBX_PARAM_FADE_IN);
        } else {
            fadeDragMode = 2;
            fadeDragInit = params[BPBX_PARAM_FADE_OUT];
            paramGestureBegin(BPBX_PARAM_FADE_OUT);
        }
    }

    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        float drag_x = ImGui::GetMouseDragDelta().x;

        double pos = fadeDragInit + (int)(drag_x / pixelsPerStep);

        if (fadeDragMode == 1) {
            double newValue = clamp(pos, 0.0, (double)FADE_IN_MAX);

            if (newValue != params[BPBX_PARAM_FADE_IN]) {
                paramChange(BPBX_PARAM_FADE_IN, newValue);
            }
        } else if (fadeDragMode == 2) {
            double newValue = clamp(pos, (double)FADE_OUT_MIN, (double)FADE_OUT_MAX);

            if (newValue != params[BPBX_PARAM_FADE_OUT]) {
                paramChange(BPBX_PARAM_FADE_OUT, newValue);
            }
        }
    }

    if (ImGui::IsItemDeactivated()) {
        if (fadeDragMode == 1) {
            paramGestureEnd(BPBX_PARAM_FADE_IN);
        } else if (fadeDragMode == 2) {
            paramGestureEnd(BPBX_PARAM_FADE_OUT);
        }
    }
}

void PluginController::filterRemovePole(FilterType filter, int control_idx) {
    assert(control_idx >= 0 && control_idx < BPBX_FILTER_GROUP_COUNT);

    int baseEnum = 0;
    switch (filter) {
        case FILTER_EQ:
            assert(false);
            // baseEnum = BPBX_PARAM_EQ_TYPE0;
            break;

        case FILTER_NOTE:
            baseEnum = BPBX_PARAM_NOTE_FILTER_TYPE0;
            break;
    }

    constexpr int lastIdx = BPBX_FILTER_GROUP_COUNT - 1;
    for (int i = control_idx; i < lastIdx; i++) {
        paramChange(baseEnum + i * 3, params[baseEnum + (i+1) * 3]);
        paramChange(baseEnum + i * 3 + 1, params[baseEnum + (i+1) * 3 + 1]);
        paramChange(baseEnum + i * 3 + 2, params[baseEnum + (i+1) * 3 + 2]);
    }

    paramChange(baseEnum + lastIdx * 3, (double)BPBX_FILTER_TYPE_OFF);
    paramChange(baseEnum + lastIdx * 3 + 1, 0.0);
    paramChange(baseEnum + lastIdx * 3 + 2, BPBX_FILTER_GAIN_CENTER);
}

void PluginController::filterInsertPole(FilterType filter, int control_idx, bpbx_filter_type_e type, double freq, double gain) {
    assert(control_idx >= 0 && control_idx < BPBX_FILTER_GROUP_COUNT);

    int baseEnum = 0;
    switch (filter) {
        case FILTER_EQ:
            assert(false);
            // baseEnum = BPBX_PARAM_EQ_TYPE0;
            break;

        case FILTER_NOTE:
            baseEnum = BPBX_PARAM_NOTE_FILTER_TYPE0;
            break;
    }

    for (int i = BPBX_FILTER_GROUP_COUNT - 2; i >= control_idx; i--) {
        paramChange(baseEnum + (i+1) * 3, params[baseEnum + i * 3]);
        paramChange(baseEnum + (i+1) * 3 + 1, params[baseEnum + i * 3 + 1]);
        paramChange(baseEnum + (i+1) * 3 + 2, params[baseEnum + i * 3 + 2]);
    }

    paramChange(baseEnum + control_idx * 3, type);
    paramChange(baseEnum + control_idx * 3 + 1, freq);
    paramChange(baseEnum + control_idx * 3 + 2, gain);
}

void PluginController::drawEqWidget(FilterType filter, const char *id, ImVec2 size) {
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec2 ui_origin = ImGui::GetCursorScreenPos();
    ImVec2 ui_end = ImVec2(ui_origin.x + size.x, ui_origin.y + size.y);
    ImGui::InvisibleButton(id, size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

    ImU32 graph_color = ImGui::GetColorU32(ImGuiCol_Header);

    int baseEnum = 0;
    switch (filter) {
        case FILTER_EQ:
            assert(false);
            // baseEnum = BPBX_PARAM_EQ_TYPE0;
            break;

        case FILTER_NOTE:
            baseEnum = BPBX_PARAM_NOTE_FILTER_TYPE0;
            break;
    }

    // calculate pole screen positions
    ImVec2 polePositions[BPBX_FILTER_GROUP_COUNT];

    for (int i = 0; i < BPBX_FILTER_GROUP_COUNT; i++) {
        int filtType = (int)params[baseEnum + FILTER_PARAM_TYPE(i)];
        if (filtType != BPBX_FILTER_TYPE_OFF) {
            float freqN = params[baseEnum + FILTER_PARAM_FREQ(i)] / BPBX_FILTER_FREQ_MAX;
            float gainN = params[baseEnum + FILTER_PARAM_GAIN(i)] / BPBX_FILTER_GAIN_MAX;

            float drawX = floor((ui_end.x - ui_origin.x) * freqN + ui_origin.x);
            float drawY = floor((ui_origin.y - ui_end.y) * gainN + ui_end.y);

            polePositions[i] = ImVec2(drawX, drawY);
        }
    }

    // drag state
    int &activePoleIndex = this->eqWidgetDragState.activePoleIndex;
    bool &wasDragging = this->eqWidgetDragState.wasDragging;
    ImVec2 &initialDragPos = this->eqWidgetDragState.initialDragPos;
    std::vector<int> &activeGestures = this->eqWidgetDragState.activeGestures;

    auto tryBeginGesture = [this](uint32_t param_idx) {
        std::vector<int> &activeGestures = this->eqWidgetDragState.activeGestures;
        auto it = std::find(activeGestures.begin(), activeGestures.end(), param_idx);
        if (it == activeGestures.end()) {
            activeGestures.push_back(param_idx);
            paramGestureBegin(param_idx);
        }
    };
    auto tryEndGesture = [this](uint32_t param_idx) {
        std::vector<int> &activeGestures = this->eqWidgetDragState.activeGestures;
        auto it = std::find(activeGestures.begin(), activeGestures.end(), param_idx);
        if (it != activeGestures.end()) {
            activeGestures.erase(it);
            paramGestureEnd(param_idx);
        }
    };

    // more drag state: when the pole is dragged OOB,
    // it will be removed. but when the user drags it back inside of OOB,
    // it should be reinserted.
    bpbx_filter_type_e &draggingPoleType = this->eqWidgetDragState.draggingPoleType;
    bool &draggingPoleExists = this->eqWidgetDragState.draggingPoleExists;
    bool &hasPoleBeenRemoved = this->eqWidgetDragState.hasPoleBeenRemoved; // if the pole was ever removed during the drag
    bool &poleIsNewlyAdded = this->eqWidgetDragState.poleIsNewlyAdded;
    bool &forceDragBegin = this->eqWidgetDragState.forceDragBegin;

    // hover/insertion state
    int hoveredPoleIndex = -1;
    bpbx_filter_type_e poleInsertionType = BPBX_FILTER_TYPE_OFF;
    int poleInsertionFreq = 0;
    int poleInsertionGain = 0;

    if (ImGui::IsItemHovered()) {
        ImVec2 mpos = ImGui::GetMousePos();

        // find the pole that is currently being hovered over
        constexpr float maxDist = 16.0;
        float hoveredPoleDistSq = maxDist * maxDist;
        for (int i = 0; i < BPBX_FILTER_GROUP_COUNT; i++) {
            int filtType = (int)params[baseEnum + FILTER_PARAM_TYPE(i)];
            if (filtType != BPBX_FILTER_TYPE_OFF) {
                ImVec2 drawPos = polePositions[i];
                float dx = drawPos.x - mpos.x;
                float dy = drawPos.y - mpos.y;
                float distSq = dx*dx + dy*dy;

                if (distSq < hoveredPoleDistSq) {
                    hoveredPoleIndex = i;
                    hoveredPoleDistSq = distSq;
                }
            }
        }

        // if there is no currently hovered-over pole,
        // show the insertion ui
        if (hoveredPoleIndex == -1) {
            float ratioX = (mpos.x - ui_origin.x) / (ui_end.x - ui_origin.x);
            float newFreq = ratioX * BPBX_FILTER_FREQ_MAX;
            float newGain = (mpos.y - ui_end.y) / (ui_origin.y - ui_end.y) * BPBX_FILTER_GAIN_MAX;

            if (ratioX < 0.2f) {
                poleInsertionType = BPBX_FILTER_TYPE_HP;
            } else if (ratioX < 0.8f) {
                poleInsertionType = BPBX_FILTER_TYPE_NOTCH;
            } else {
                poleInsertionType = BPBX_FILTER_TYPE_LP;
            }

            poleInsertionFreq = newFreq;
            poleInsertionGain = newGain;
        }
    }


    // begin dragging
    if (ImGui::IsItemActivated()) {
        forceDragBegin = false;

        if (hoveredPoleIndex != -1) {
            activePoleIndex = hoveredPoleIndex;
            wasDragging = false;
            poleIsNewlyAdded = false;
        } else {
            // left-click to insert pole, and then begin drag of this new pole
            if (poleInsertionType != BPBX_FILTER_TYPE_OFF && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                for (int newIndex = 0; newIndex < BPBX_FILTER_GROUP_COUNT; newIndex++) {
                    if (params[baseEnum + FILTER_PARAM_TYPE(newIndex)] == BPBX_FILTER_TYPE_OFF) {
                        tryBeginGesture(baseEnum + FILTER_PARAM_TYPE(newIndex));
                        tryBeginGesture(baseEnum + FILTER_PARAM_FREQ(newIndex));
                        tryBeginGesture(baseEnum + FILTER_PARAM_GAIN(newIndex));

                        paramChange(baseEnum + FILTER_PARAM_TYPE(newIndex), (double)poleInsertionType);
                        paramChange(baseEnum + FILTER_PARAM_FREQ(newIndex), poleInsertionFreq);
                        paramChange(baseEnum + FILTER_PARAM_GAIN(newIndex), poleInsertionGain);

                        activePoleIndex = newIndex;
                        forceDragBegin = true;
                        poleIsNewlyAdded = true;

                        float freqN = params[baseEnum + FILTER_PARAM_FREQ(newIndex)] / BPBX_FILTER_FREQ_MAX;
                        float gainN = params[baseEnum + FILTER_PARAM_GAIN(newIndex)] / BPBX_FILTER_GAIN_MAX;

                        float drawX = floor((ui_end.x - ui_origin.x) * freqN + ui_origin.x);
                        float drawY = floor((ui_origin.y - ui_end.y) * gainN + ui_end.y);

                        polePositions[newIndex] = ImVec2(drawX, drawY);
                        break;
                    }
                }
                
            }
        }

        if (activePoleIndex != -1) {
            initialDragPos = polePositions[activePoleIndex];
            draggingPoleExists = true;
            hasPoleBeenRemoved = false;
        }
    }

    {
        int typeParamIdx = baseEnum + FILTER_PARAM_TYPE(activePoleIndex);
        int freqParamIdx = baseEnum + FILTER_PARAM_FREQ(activePoleIndex);
        int gainParamIdx = baseEnum + FILTER_PARAM_GAIN(activePoleIndex);

        if (ImGui::IsItemDeactivated() && activePoleIndex != -1) {
            if (wasDragging) {
                if (draggingPoleExists) {
                    tryEndGesture(freqParamIdx);
                    tryEndGesture(gainParamIdx);
                }

                // if the pole was ever removed while dragging, end
                // the consequent gestures
                if (hasPoleBeenRemoved) {
                    tryEndGesture(typeParamIdx);
                    for (int i = activePoleIndex + 1; i < BPBX_FILTER_GROUP_COUNT; i++) {
                        tryEndGesture(baseEnum + FILTER_PARAM_TYPE(i));
                        tryEndGesture(baseEnum + FILTER_PARAM_FREQ(i));
                        tryEndGesture(baseEnum + FILTER_PARAM_GAIN(i));
                    }
                } else if (poleIsNewlyAdded) {
                    tryEndGesture(typeParamIdx);
                }
            } else {
                // a left click will remove the pole
                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                    for (int i = activePoleIndex; i < BPBX_FILTER_GROUP_COUNT; i++) {
                        tryBeginGesture(baseEnum + FILTER_PARAM_TYPE(i));
                        tryBeginGesture(baseEnum + FILTER_PARAM_FREQ(i));
                        tryBeginGesture(baseEnum + FILTER_PARAM_GAIN(i));
                    }

                    filterRemovePole(filter, activePoleIndex);
                }

                // a right click will remove the pole without shifting the list
                else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
                    tryBeginGesture(typeParamIdx);
                    tryBeginGesture(freqParamIdx);
                    tryBeginGesture(gainParamIdx);

                    paramChange(typeParamIdx, (double)BPBX_FILTER_TYPE_OFF);
                    paramChange(freqParamIdx, 0.0);
                    paramChange(gainParamIdx, BPBX_FILTER_GAIN_CENTER);

                    tryEndGesture(typeParamIdx);
                    tryEndGesture(freqParamIdx);
                    tryEndGesture(gainParamIdx);
                }
            }

            for (uint32_t param_idx : activeGestures) {
                paramGestureEnd(param_idx);
            }
            activeGestures.clear();

            activePoleIndex = -1;
        }

        if (ImGui::IsItemActive() && activePoleIndex != -1) {
            hoveredPoleIndex = activePoleIndex;

            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) || forceDragBegin) {
                if (!wasDragging) {
                    tryBeginGesture(freqParamIdx);
                    tryBeginGesture(gainParamIdx);

                    draggingPoleType = (bpbx_filter_type_e)params[typeParamIdx];
                }

                wasDragging = true;
                ImVec2 delta = ImGui::GetMouseDragDelta();
                ImVec2 newPos;
                newPos.x = initialDragPos.x + delta.x;
                newPos.y = initialDragPos.y + delta.y;

                float newFreq = (newPos.x - ui_origin.x) / (ui_end.x - ui_origin.x) * BPBX_FILTER_FREQ_MAX;
                float newGain = (newPos.y - ui_end.y) / (ui_origin.y - ui_end.y) * BPBX_FILTER_GAIN_MAX;

                newFreq = floorf(newFreq + 0.5);
                newGain = floorf(clamp(newGain, 0.f, (float)BPBX_FILTER_GAIN_MAX) + 0.5);

                // pole disappears when out of freq bounds
                bool poleExists = newFreq >= 0.0 && newFreq <= BPBX_FILTER_FREQ_MAX;
                if (draggingPoleExists != poleExists) {
                    draggingPoleExists = poleExists;

                    if (!poleExists) {
                        if (!hasPoleBeenRemoved) {
                            tryBeginGesture(typeParamIdx);
                            for (int i = activePoleIndex + 1; i < BPBX_FILTER_GROUP_COUNT; i++) {
                                tryBeginGesture(baseEnum + FILTER_PARAM_TYPE(i));
                                tryBeginGesture(baseEnum + FILTER_PARAM_FREQ(i));
                                tryBeginGesture(baseEnum + FILTER_PARAM_GAIN(i));
                            }
                        }
                        hasPoleBeenRemoved = true;

                        filterRemovePole(filter, activePoleIndex);
                    }
                    else {
                        filterInsertPole(filter, activePoleIndex, draggingPoleType, newFreq, newGain);
                    }
                }

                newPos.x = floor((ui_end.x - ui_origin.x) * (newFreq / BPBX_FILTER_FREQ_MAX) + ui_origin.x);
                newPos.y = floor((ui_origin.y - ui_end.y) * (newGain / BPBX_FILTER_GAIN_MAX) + ui_end.y);

                if (poleExists) {
                    paramChange(freqParamIdx, newFreq);
                    paramChange(gainParamIdx, newGain);
                    polePositions[activePoleIndex] = newPos;
                } else {
                    hoveredPoleIndex = -1;
                }
            }
        }
    }

    // now, do the drawing
    // draw frequency response graph
    const double ref_sample_rate = 48000;
    float last_y;
    float last_x;
    for (int i = 0; i < BPBX_FILTER_FREQ_RANGE * 2; i++) {
        double freqIndex = (double)i / 2.0;
        double hz = bpbx_freq_setting_to_hz(freqIndex);

        double linear_gain = 1.0;
        for (int ctl = 0; ctl < BPBX_FILTER_GROUP_COUNT; ctl++) {
            bpbx_complex_s cmp;
            bpbx_analyze_freq_response(
                (bpbx_filter_type_e)params[baseEnum + FILTER_PARAM_TYPE(ctl)],
                params[baseEnum + FILTER_PARAM_FREQ(ctl)],
                params[baseEnum + FILTER_PARAM_GAIN(ctl)],
                hz, ref_sample_rate, &cmp);
            
            linear_gain *= sqrt(cmp.real * cmp.real + cmp.imag * cmp.imag);
        }

        double gain_setting = bpbx_linear_gain_to_setting(linear_gain);
        float gain_y_normalized = (gain_setting / BPBX_FILTER_GAIN_MAX);
        if (gain_y_normalized < 0.0) gain_y_normalized = 0.0;
        if (gain_y_normalized > 1.0) gain_y_normalized = 1.0;
        
        float y = (ui_origin.y - ui_end.y) * gain_y_normalized + ui_end.y;
        float x = floor((ui_end.x - ui_origin.x) * ((float)freqIndex / BPBX_FILTER_FREQ_MAX) + ui_origin.x);

        if (i > 0 && (fabs(y - ui_end.y) >= 2 || fabs(last_y - ui_end.y) >= 2)) {
            drawList->AddQuadFilled(
                ImVec2(x, y), ImVec2(x, ui_end.y),
                ImVec2(last_x, ui_end.y), ImVec2(last_x, last_y),
                graph_color);
        }

        last_x = x;
        last_y = y;
    }
     
    // draw the poles
    ImU32 poleColor = ImGui::GetColorU32(ImGuiCol_HeaderActive);
    ImU32 poleActiveColor = ImGui::GetColorU32(ImGuiCol_Text);
    for (int i = 0; i < BPBX_FILTER_GROUP_COUNT; i++) {
        int filtType = (int)params[baseEnum + i * 3];
        if (filtType != BPBX_FILTER_TYPE_OFF) {
            ImU32 color = i == hoveredPoleIndex ? poleActiveColor : poleColor;

            ImVec2 drawPos = polePositions[i];
            float drawX = drawPos.x;
            float drawY = drawPos.y;

            // draw lp/hp dashed lines
            switch (filtType) {
                case BPBX_FILTER_TYPE_LP:
                    drawDashedHLine(drawList, drawX, ui_end.x, drawY, 3.0, 2.0, poleColor);
                    break;

                case BPBX_FILTER_TYPE_HP:
                    drawDashedHLine(drawList, ui_origin.x, drawX, drawY, 3.0, 2.0, poleColor);
                    break;

                default: break;
            }

            // draw circle
            drawList->AddCircleFilled(ImVec2(drawX, drawY), activePoleIndex == i ? 3.5 : 2.5, color);
        }
    }

    // draw hovered/active pole info
    if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
        char textBuf[64];
        const char *poleTypeStr;
        bool insertion = poleInsertionType != BPBX_FILTER_TYPE_OFF;
        switch (insertion ? poleInsertionType : (bpbx_filter_type_e)params[baseEnum + FILTER_PARAM_TYPE(hoveredPoleIndex)]) {
            case BPBX_FILTER_TYPE_HP:
                poleTypeStr = "high-pass";
                break;

            case BPBX_FILTER_TYPE_LP:
                poleTypeStr = "low-pass";
                break;

            case BPBX_FILTER_TYPE_NOTCH:
                poleTypeStr = "peak";
                break;

            default:
                poleTypeStr = nullptr;
                break;
        }

        if (poleTypeStr) {
            if (insertion) {
                snprintf(textBuf, 64, "+ %s", poleTypeStr);
            } else {
                snprintf(textBuf, 64, "%i: %s", hoveredPoleIndex + 1, poleTypeStr);
            }

            drawList->AddText(ImVec2(ui_origin.x, ui_end.y - ImGui::GetFontSize()), poleColor, textBuf);
        }
    }
}

void PluginController::drawHarmonicsEditor(const char *id, uint32_t paramId, ImVec2 size) {
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec2 ui_origin = ImGui::GetCursorScreenPos();
    ui_origin = ImVec2(floorf(ui_origin.x), floorf(ui_origin.y));

    ImVec2 ui_end = ImVec2(ui_origin.x + size.x, ui_origin.y + size.y);
    ui_end = ImVec2(floorf(ui_end.x), floorf(ui_end.y));

    ImGui::InvisibleButton(id, size, ImGuiButtonFlags_MouseButtonLeft);

    auto &state = this->harmonicsEditorState;

    // this display only looks right at one width...
    float lastColumnWidth = 6;
    float columnWidth = roundf( (ui_end.x - ui_origin.x - lastColumnWidth) / (BPBX_HARMONICS_CONTROL_COUNT) );

    if (ImGui::IsItemActivated()) {
        memset(state.activeGestures, 0, sizeof(state.activeGestures));
    }

    if (ImGui::IsItemActive()) {
        ImVec2 mpos = ImGui::GetMousePos();
        float relX = mpos.x - ui_origin.x;

        int i;
        if (relX < ui_end.x && relX >= ui_end.x - lastColumnWidth) {
            i = BPBX_HARMONICS_CONTROL_COUNT - 1;
        } else {
            i = floorf(relX / columnWidth);
        }

        if (i >= 0 && i < BPBX_HARMONICS_CONTROL_COUNT) {
            if (!state.activeGestures[i]) {
                state.activeGestures[i] = true;
                paramGestureBegin(paramId + i);
            }

            float value = 1.f - (mpos.y - ui_origin.y) / (ui_end.y - ui_origin.y);
            paramChange(paramId + i, roundf(clamp(value, 0.f, 1.f) * BPBX_HARMONICS_CONTROL_MAX));
        }
    }

    if (ImGui::IsItemDeactivated()) {
        for (int i = 0; i < BPBX_HARMONICS_CONTROL_COUNT; i++) {
            if (state.activeGestures[i])
                paramGestureEnd(paramId + i);

            state.activeGestures[i] = false;
        }
    }

    ImU32 controlColor = ImGui::GetColorU32(ImGuiCol_ButtonActive);
    ImU32 octaveGuideColor = ImGui::GetColorU32(ImGuiCol_Button);
    // {
    //     ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    //     float h, s, v;
    //     ImGui::ColorConvertRGBtoHSV(buttonColor.x, buttonColor.y, buttonColor.z, h, s, v);
    //     h = fmodf(h + 0.14, 1.0);
    //     ImGui::ColorConvertHSVtoRGB(h, s, v, buttonColor.x, buttonColor.y, buttonColor.z);

    //     // buttonColor.w = clamp(buttonColor.w + 0.3f, 0.f, 1.f);
    //     octaveGuideColor = ImGui::ColorConvertFloat4ToU32(buttonColor);
    // }

    ImU32 fifthGuideColor = ImGui::GetColorU32(ImGuiCol_FrameBg);  

    // draw the guides
    static const int octaves[] = {1, 2, 4, 8, 16};
    static const int fifths[] = {3, 6, 12, 24};

    // octaves
    for (int i = 0; i < sizeof(octaves)/sizeof(*octaves); i++) {
        float x = ui_origin.x + columnWidth * (octaves[i] - 1);
        drawList->AddRectFilled(
            ImVec2(x, ui_origin.y),
            ImVec2(x + 3, ui_end.y),
            octaveGuideColor);
    }

    // fifths
    for (int i = 0; i < sizeof(fifths)/sizeof(*fifths); i++) {
        float x = ui_origin.x + columnWidth * (fifths[i] - 1);
        drawList->AddRectFilled(
            ImVec2(x, ui_origin.y),
            ImVec2(x + 3, ui_end.y),
            fifthGuideColor);
    }

    for (int i = 0; i < BPBX_HARMONICS_CONTROL_COUNT - 1; i++) {
        float x = ui_origin.x + columnWidth * i;
        float size = (float)params[paramId + i] / BPBX_HARMONICS_CONTROL_MAX;

        if (size > 0.f) {
            drawList->AddRectFilled(
                ImVec2(x, floorf((ui_origin.y - ui_end.y) * size + ui_end.y)),
                ImVec2(x + 3, ui_end.y),
                controlColor);
        }
    }

    // last control looks special
    {
        int i = BPBX_HARMONICS_CONTROL_COUNT - 1;
        float x0 = ui_origin.x + columnWidth * i;
        float size = (float)params[paramId + i] / BPBX_HARMONICS_CONTROL_MAX;

        for (float x = x0; x < ui_end.x; x += 2) {
            drawList->AddRectFilled(
                ImVec2(x, floorf((ui_origin.y - ui_end.y) * size + ui_end.y)),
                ImVec2(x + 1, floorf(ui_end.y)),
                controlColor);
        }
    }
}

void PluginController::drawModulationPad() {
    ImGui::AlignTextToFramePadding();
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Modulation").x) / 2.f);
    ImGui::Text("Modulation");

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    float sliderLabelY = ImGui::GetCursorScreenPos().y;

    // make space for the X and Y labels of the vertical sliders
    ImGui::NewLine();

    ImGuiStyle &style = ImGui::GetStyle();

    float padHeight = ImGui::GetTextLineHeight() * 10.f;
    float sliderWidth = (ImGui::GetContentRegionAvail().x - padHeight) / 2.f - style.ItemSpacing.x;

    // x and y sliders
    drawList->AddText(
        ImVec2(ImGui::GetCursorScreenPos().x + (sliderWidth - ImGui::CalcTextSize("X").x) / 2.f, sliderLabelY),
        ImGui::GetColorU32(ImGuiCol_Text),
        "X"
    );
    vertSliderParameter(BPBX_PARAM_MOD_X, "##X", ImVec2(sliderWidth, padHeight), 0.f, 1.f);
    ImGui::SameLine();

    drawList->AddText(
        ImVec2(ImGui::GetCursorScreenPos().x + (sliderWidth - ImGui::CalcTextSize("Y").x) / 2.f, sliderLabelY),
        ImGui::GetColorU32(ImGuiCol_Text),
        "Y"
    );
    vertSliderParameter(BPBX_PARAM_MOD_Y, "##Y", ImVec2(sliderWidth, padHeight), 0.f, 1.f);

    // the pad
    ImGui::SameLine();
    ImVec2 padSize = ImVec2(padHeight, padHeight);

    //ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - padWidth) / 2.f + ImGui::GetStyle().WindowPadding.x);
    ImVec2 padTL = ImGui::GetCursorScreenPos();
    ImVec2 padBR = ImVec2(padTL.x + padSize.x, padTL.y + padSize.y);
    ImGui::InvisibleButton("modpad", padSize);

    // draw pad label
    drawList->AddText(
        ImVec2(padTL.x + (padSize.x - ImGui::CalcTextSize("Pad").x) / 2.f, sliderLabelY),
        ImGui::GetColorU32(ImGuiCol_Text),
        "Pad"
    );

    // draw pad graphics
    drawList->AddRectFilled(padTL, padBR, ImGui::GetColorU32(ImGuiCol_FrameBg));
    drawList->AddCircleFilled(
        ImVec2(
            params[BPBX_PARAM_MOD_X] * padSize.x + padTL.x,
            padBR.y - params[BPBX_PARAM_MOD_Y] * padSize.y
        ),
        4.f, ImGui::GetColorU32(ImGuiCol_ButtonHovered)
    );

    // pad controls
    if (ImGui::IsItemActivated()) {
        paramGestureBegin(BPBX_PARAM_MOD_X);
        paramGestureBegin(BPBX_PARAM_MOD_Y);
    }

    if (ImGui::IsItemActive()) {
        ImVec2 mousePos = ImGui::GetMousePos();
        float newX = clamp((mousePos.x - padTL.x) / padSize.x);
        float newY = clamp((padBR.y - mousePos.y) / padSize.y);

        paramChange(BPBX_PARAM_MOD_X, (double) newX);
        paramChange(BPBX_PARAM_MOD_Y, (double) newY);
    }

    if (ImGui::IsItemDeactivated()) {
        paramGestureEnd(BPBX_PARAM_MOD_X);
        paramGestureEnd(BPBX_PARAM_MOD_Y);
    }
}

void PluginController::drawEqPage(FilterType targetFilter) {
    drawEqWidget(targetFilter, "eqWidget", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight() * 3.0));

    static const char *filterTypes[] = {"off", "low pass", "high pass", "peak"};

    const char *filterName = "???";
    int baseEnum;
    if (targetFilter == FILTER_EQ) {
        assert(false);
        filterName = "EQ";
        // baseEnum = BPBX_PARAM_EQ_TYPE0;
    } else if (targetFilter == FILTER_NOTE) {
        filterName = "N. Filt.";
        baseEnum = BPBX_PARAM_NOTE_FILTER_TYPE0;
    }

    for (int i = 0; i < BPBX_FILTER_GROUP_COUNT; i++) {
        ImGui::PushID(i);

        int typeParam = baseEnum + i * 3;
        int freqParam = typeParam + 1;
        int gainParam = typeParam + 2;

        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s %i", filterName, i+1);

        sameLineRightCol();
        ImGui::SetNextItemWidth(-FLT_MIN);

        int curType = (int)params[typeParam];
        if (ImGui::Combo("##NoteFilterType", &curType, filterTypes, 4)) {
            paramGestureBegin(typeParam);
            paramChange(typeParam, (double)curType);
            paramGestureEnd(typeParam);
        }
        paramControls(typeParam);

        if (curType != BPBX_FILTER_TYPE_OFF) {
            ImGui::PushStyleVarY(ImGuiStyleVar_FramePadding, 0.f);

            ImGui::AlignTextToFramePadding();
            ImGui::BulletText("Freq.");

            sameLineRightCol();
            ImGui::SetNextItemWidth(-FLT_MIN);
            sliderParameter(freqParam, "##Freq", 0.0, BPBX_FILTER_FREQ_MAX);

            ImGui::AlignTextToFramePadding();
            ImGui::BulletText("Gain");

            sameLineRightCol();
            ImGui::SetNextItemWidth(-FLT_MIN);
            sliderParameter(gainParam, "##Gain", 0.0, BPBX_FILTER_GAIN_MAX);

            ImGui::PopStyleVar();
        }

        ImGui::PopID();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0);
    }

}

void PluginController::sameLineRightCol() {
    ImGui::SameLine();
    ImGui::SetCursorPosX(uiRightCol);
}

void PluginController::draw(platform::Window *window) {
    updateParams();
    updateColors();


    // imgui
    {
        if (ImGui::BeginMainMenuBar()) {
            // if (ImGui::BeginMenu("."))
            {
                if (ImGui::BeginMenu("Presets")) {
                    ImGui::MenuItem("Test");
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Control")) {
                    ImGui::PushStyleVarY(ImGuiStyleVar_FramePadding, 0.f);
                    ImGui::PushItemWidth(ImGui::GetFontSize() * 9.f);

                    ImGui::BeginGroup();
                    ImGui::Text("Gain");
                    ImGui::Text("Tempo Mult.");
                    ImGui::Text("Force Tempo");

                    if (params[PLUGIN_CPARAM_TEMPO_USE_OVERRIDE])
                        ImGui::Text("Tempo");
                    
                    ImGui::EndGroup();

                    ImGui::SameLine();
                    ImGui::BeginGroup();
                    sliderParameter(PLUGIN_CPARAM_GAIN, "###gain", -10.0, 10.0, "%.3f dB");
                    sliderParameter(PLUGIN_CPARAM_TEMPO_MULTIPLIER, "###tempomult", 0.0, 10.0, "%.1fx");
                    
                    bool useTempoOverride = params[PLUGIN_CPARAM_TEMPO_USE_OVERRIDE] != 0.0;
                    if (ImGui::Checkbox("##tempooverridetoggle", &useTempoOverride)) {
                        paramGestureBegin(PLUGIN_CPARAM_TEMPO_USE_OVERRIDE);
                        paramChange(PLUGIN_CPARAM_TEMPO_USE_OVERRIDE, useTempoOverride ? 1.0 : 0.0);
                        paramGestureEnd(PLUGIN_CPARAM_TEMPO_USE_OVERRIDE);
                    }
                    paramControls(PLUGIN_CPARAM_TEMPO_USE_OVERRIDE);
                    
                    if (params[PLUGIN_CPARAM_TEMPO_USE_OVERRIDE])
                        sliderParameter(PLUGIN_CPARAM_TEMPO_OVERRIDE, "###tempooverride", 30.0, 500.0, "%.0f");
                    
                    ImGui::EndGroup();
                    
                    ImGui::PopItemWidth();
                    ImGui::PopStyleVar();

                    ImGui::EndMenu();
                }

                if (ImGui::MenuItem("About")) {
                    showAbout = !showAbout;
                }
                
                // ImGui::EndMenu();
            }

            if (currentPage != PAGE_MAIN || showAbout) {
                const char *label = "×";
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::GetFrameHeightWithSpacing());

                float paddingY = ImGui::GetStyle().FramePadding.y;
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(paddingY, paddingY));

                if (ImGui::MenuItem(label)) {
                    if (showAbout)  showAbout = false;
                    else            currentPage = PAGE_MAIN;
                }

                ImGui::PopStyleVar();
            }
    
            ImGui::EndMainMenuBar();
        }
    
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        // about window ui
        if (showAbout) {
            drawAbout(winFlags);

        // instrument ui
        } else {
            uiRightCol = ImGui::GetFontSize() * 7.f;
            
            switch (currentPage) {
                case PAGE_MAIN:
                    if (ImGui::Begin("inst", NULL, winFlags)) {
                        // volume
                        ImGui::AlignTextToFramePadding();
                        ImGui::Text("Volume");
                        sameLineRightCol();
                        ImGui::SetNextItemWidth(-FLT_MIN);
                        sliderParameter(BPBX_PARAM_VOLUME, "##volume", -25.0, 25.0, "%.0f");

                        // why are some uis separated like this
                        switch (inst_type) {
                            case BPBX_INSTRUMENT_CHIP:
                                drawChipGui1();
                                break;

                            default: break;
                        }
        
                        ImGui::AlignTextToFramePadding();
                        ImGui::Text("Fade In/Out");
                        sameLineRightCol();
                        drawFadeWidget("fadectl", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight() * 1.75f));
        
                        // specific instrument
                        switch (inst_type) {
                            case BPBX_INSTRUMENT_FM:
                                drawFmGui();
                                break;

                            case BPBX_INSTRUMENT_CHIP:
                                drawChipGui2();
                                break;
                            
                            case BPBX_INSTRUMENT_HARMONICS:
                                drawHarmonicsGui();
                                break;

                            default: break;
                        }
        
                        drawEffects();
                        drawEnvelopes();
                        drawModulationPad();
                    }
                    ImGui::End();

                    break;

                case PAGE_NOTE_FILTER:
                    if (ImGui::Begin("eq", NULL, winFlags)) {
                        drawEqPage(FILTER_NOTE);
                    }
                    ImGui::End();
                    break;
                
                case PAGE_EQ:
                    if (ImGui::Begin("eq", NULL, winFlags)) {
                        drawEqPage(FILTER_EQ);
                    }
                    ImGui::End();
                    break;
            }
        }
    }
}

// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
Hsv Color::toHsv() const {
    float r = clamp(this->r);
    float g = clamp(this->g);
    float b = clamp(this->b);

    Hsv out;
    double min, max, delta;

    min = r   < g ? r   : g;
    min = min < b ? min : b;

    max = r   > g ? r   : g;
    max = max > b ? max : b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001) {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        goto end;
    }

    if (max > 0.0) {    // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN; // its now undefined
        goto end;
    }

    if (r >= max)                  // > is bogus, just keeps compilor happy
        out.h = ( g - b ) / delta; // between yellow & magenta
    else if (g >= max)
        out.h = 2.0 + ( b - r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( r - g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    end:;
    out.h /= 360.0f;
    return out;
}

Color Color::fromHsv(const Hsv &hsv) {
    Hsv in(fwrapf(hsv.h, 1.0f) * 360.0, clamp(hsv.s), clamp(hsv.v));

    Color out;

    double hh, p, q, t, ff;
    long   i;

    if (in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }

    hh = in.h;
    if (hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }

    return out;
}

void PluginController::updateColors() {
    ImGui::StyleColorsDark();

    if (useCustomColors) {
        ImGuiStyle &style = ImGui::GetStyle();

        Hsv customColorHsv = customColor.toHsv();
        style.Colors[ImGuiCol_WindowBg] = (customColor * 0.1f).toImVec4(240.0f / 255.0f);
        style.Colors[ImGuiCol_PopupBg] = (customColor * 0.12f).toImVec4(240.0f / 255.0f);

        Color menuBarBg;
        {
            Hsv hsv = customColorHsv;
            hsv.s *= 0.5f;
            hsv.v = 0.1f;
            menuBarBg = Color::fromHsv(hsv);
        }

        style.Colors[ImGuiCol_MenuBarBg] = menuBarBg;

        Color frameBgBase;
        {
            Hsv hsv = customColorHsv;
            hsv.s *= 1.1f;
            hsv.v *= 0.4f;
            frameBgBase = Color::fromHsv(hsv);
        }

        Color frameBgHovered;
        {
            Hsv hsv = frameBgBase.toHsv();
            hsv.s *= 0.5f;
            hsv.v *= 1.5f;
            frameBgHovered = Color::fromHsv(hsv);
        }

        Color buttonBase;
        {
            Hsv hsv = customColorHsv;
            hsv.v *= 1.4f;
            buttonBase = Color::fromHsv(hsv);
        }

        Color buttonActive;
        {
            Hsv hsv = customColorHsv;
            hsv.s *= 1.3f;
            buttonActive = Color::fromHsv(hsv);
        }

        Color sliderBase;
        {
            Hsv hsv = customColorHsv;
            hsv.s *= 0.9f;
            hsv.v *= 0.8f;
            sliderBase = Color::fromHsv(hsv);
        }

        Color sliderHovered;
        {
            Hsv hsv = customColorHsv;
            hsv.s *= 0.9f;
            sliderHovered = Color::fromHsv(hsv);
        }

        style.Colors[ImGuiCol_FrameBg] = frameBgBase.toImVec4(138.f / 255.f);
        style.Colors[ImGuiCol_FrameBgHovered] = frameBgHovered.toImVec4(102.f / 255.f);
        style.Colors[ImGuiCol_FrameBgActive] = frameBgHovered.toImVec4(171.f / 255.f);

        style.Colors[ImGuiCol_SliderGrab] = sliderBase;
        style.Colors[ImGuiCol_SliderGrabActive] = sliderHovered;

        style.Colors[ImGuiCol_Button] = buttonBase.toImVec4(102.f / 255.f);
        style.Colors[ImGuiCol_ButtonHovered] = buttonBase;
        style.Colors[ImGuiCol_ButtonActive] = buttonActive;

        style.Colors[ImGuiCol_Header] = buttonBase.toImVec4(79.f / 255.f);
        style.Colors[ImGuiCol_HeaderHovered] = buttonBase.toImVec4(204.f / 255.f);
        style.Colors[ImGuiCol_HeaderActive] = buttonBase;
    }
}