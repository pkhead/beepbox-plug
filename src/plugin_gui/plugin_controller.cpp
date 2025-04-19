#define SOKOL_IMGUI_NO_SOKOL_APP
#include <imgui.h>
#include <sokol_gfx.h>
#include <sokol_log.h>
#include <sokol_imgui.h>
#include <stb_image.h>
#include <cstring>

#include "plugin_controller.hpp"

using namespace beepbox;

#ifdef PLUGIN_VST3
#include "resource/vst_logo.hpp"

struct {
    sg_image tex;
    sg_sampler smp;
    int width;
    int height;
} static vstLogo;
#endif

void PluginController::graphicsInit() {
    #ifdef PLUGIN_VST3
    int width, height, channels;
    uint8_t *imageData = stbi_load_from_memory(resources::vst_logo, resources::vst_logo_len, &width, &height, &channels, 4);
    if (imageData == nullptr) {
        vstLogo.tex.id = SG_INVALID_ID;
        vstLogo.smp.id = SG_INVALID_ID;
        vstLogo.width = 0;
        vstLogo.height = 0;
        return;
    }

    {
        sg_image_desc desc = {};
        desc.width = width;
        desc.height = height;
        desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        desc.data.subimage[0][0].ptr = imageData;
        desc.data.subimage[0][0].size = width * height * sizeof(uint8_t) * channels;
        vstLogo.tex = sg_make_image(&desc);
    }

    {
        sg_sampler_desc desc = {};
        desc.mag_filter = SG_FILTER_LINEAR;
        desc.min_filter = SG_FILTER_LINEAR;
        vstLogo.smp = sg_make_sampler(&desc);
    }

    vstLogo.width = width;
    vstLogo.height = height;

    stbi_image_free(imageData);
    #endif
}

void PluginController::graphicsClose() {
    #ifdef PLUGIN_VST3
    sg_destroy_image(vstLogo.tex);
    sg_destroy_sampler(vstLogo.smp);
    #endif
}

PluginController::PluginController(beepbox::bpbx_inst_s *instrument) : instrument(instrument) {
    showAbout = false;

    // initialize copy of plugin state
    sync();
}

void PluginController::sync() {
    beepbox::bpbx_inst_type_e type = beepbox::bpbx_inst_type(instrument);
    constexpr uint32_t param_count = sizeof(params)/sizeof(*params);
    assert(beepbox::bpbx_param_count(type) == param_count);

    for (int i = 0; i < param_count; i++) {
        beepbox::bpbx_inst_get_param_double(instrument, i, params+i);
    }

    envelopes.clear();
    envelopes.reserve(BPBX_MAX_ENVELOPE_COUNT);
    envelopes.resize(beepbox::bpbx_inst_envelope_count(instrument));
    memcpy(envelopes.data(), beepbox::bpbx_inst_get_envelope(instrument, 0), sizeof(bpbx_envelope_s) * envelopes.size());
}

void PluginController::updateParams() {
    gui_event_queue_item_s item;
    while (plugin_to_gui.dequeue(item)) {
        switch (item.type) {
            case GUI_EVENT_PARAM_CHANGE:
                params[item.param_value.param_id] = item.param_value.value;
                break;

            default:
                break;
        }
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
}

void PluginController::paramGestureBegin(uint32_t param_id) {
    gui_event_queue_item_s item = {};
    item.type = GUI_EVENT_PARAM_GESTURE_BEGIN;
    item.gesture.param_id = param_id;

    gui_to_plugin.enqueue(item);
}

void PluginController::paramChange(uint32_t param_id, double value) {
    gui_event_queue_item_s item = {};
    item.type = GUI_EVENT_PARAM_CHANGE;
    item.param_value.param_id = param_id;
    item.param_value.value = value;

    gui_to_plugin.enqueue(item);

    params[param_id] = value;
}

void PluginController::paramGestureEnd(uint32_t param_id) {
    gui_event_queue_item_s item = {};
    item.type = GUI_EVENT_PARAM_GESTURE_END;
    item.gesture.param_id = param_id;

    gui_to_plugin.enqueue(item);
}

void PluginController::event(platform::Event ev, platform::Window *window) {
    updateParams();
}

void PluginController::drawAbout(ImGuiWindowFlags winFlags) {
    ImGui::Begin("about", NULL, winFlags);
    ImGui::Text("emulation of beepbox instruments");
    ImGui::Text("author: pkhead");
    ImGui::Text("original: john nesky (shaktool)");
    ImGui::Text("libraries: sokol, Dear ImGui");

    // show vst3-compatible logo
    #ifdef PLUGIN_VST3
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - (float)vstLogo.width) / 2.f);
    ImGui::Image(simgui_imtextureid_with_sampler(vstLogo.tex, vstLogo.smp), ImVec2((float)vstLogo.width, (float)vstLogo.height));
    #endif

    ImGui::End();
}

void PluginController::drawFmGui(ImGuiWindowFlags winFlags) {
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
    
    if (ImGui::Begin("fm", NULL, winFlags)) {
        // volume
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Volume");
        ImGui::SameLine();
        sliderParameter(BPBX_PARAM_VOLUME, "##volume", -25.0, 25.0, "%.0f");

        // fade in
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Fadein");
        ImGui::SameLine();
        sliderParameter(BPBX_PARAM_FADE_IN, "##fadein", 0.0, 9.0, "%.0f");

        // fade out
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Fadeout");
        ImGui::SameLine();
        sliderParameter(BPBX_PARAM_FADE_OUT, "##fadeout", -4.0, 6.0, "%.0f");

        // algorithm
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Algorithm");

        ImGui::SameLine();
        float algoEndX = ImGui::GetCursorPosX();
        ImGui::SetNextItemWidth(-FLT_MIN);

        {
            const bpbx_inst_param_info_s *p_info = bpbx_param_info(bpbx_inst_type(instrument), BPBX_FM_PARAM_ALGORITHM);
            assert(p_info);
            const char **algoNames = p_info->enum_values;
            if (ImGui::Combo("##algo", &p_algo, algoNames, 13)) {
                paramGestureBegin(BPBX_FM_PARAM_ALGORITHM);
                paramChange(BPBX_FM_PARAM_ALGORITHM, (double)p_algo);
                paramGestureEnd(BPBX_FM_PARAM_ALGORITHM);
            }
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

            const bpbx_inst_param_info_s *p_info = bpbx_param_info(bpbx_inst_type(instrument), id);
            assert(p_info);
            const char **freqRatios = p_info->enum_values;

            if (ImGui::Combo("##freq", &p_freq[op], freqRatios, BPBX_FM_FREQ_COUNT, ImGuiComboFlags_HeightLargest)) {

                paramGestureBegin(id);
                paramChange(id, p_freq[op]);
                paramGestureEnd(id);
            }
            // if (ImGui::BeginCombo("##freq", freqRatios[gui->freq[op]], ImGuiComboFlags_HeightLargest)) {
            //     for (int i = 0; i < sizeof(freqRatios) / sizeof(*freqRatios); i++) {
            //         ImGui::Selectable(freqRatios[i]);
            //     }
            //     ImGui::EndCombo();
            // }

            ImGui::SameLine();
            ImGui::SetNextItemWidth(-FLT_MIN);
            sliderParameter(BPBX_FM_PARAM_VOLUME1 + op*2, "##vol", 0.0f, 15.0f, "%.0f");
            ImGui::PopID();
        }

        // feedback type
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Feedback");

        ImGui::SameLine();
        float feedbackEndX = ImGui::GetCursorPosX();
        ImGui::SetNextItemWidth(-FLT_MIN);
        {
            const bpbx_inst_param_info_s *p_info = bpbx_param_info(bpbx_inst_type(instrument), BPBX_FM_PARAM_FEEDBACK_TYPE);
            assert(p_info);
            const char **feedbackNames = p_info->enum_values;

            if (ImGui::Combo("##fdbk", &p_fdbkType, feedbackNames, BPBX_FM_FEEDBACK_TYPE_COUNT)) {
                paramGestureBegin(BPBX_FM_PARAM_FEEDBACK_TYPE);
                paramChange(BPBX_FM_PARAM_FEEDBACK_TYPE, (double)p_fdbkType);
                paramGestureEnd(BPBX_FM_PARAM_FEEDBACK_TYPE);
            }
        }

        // feedback volume
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Fdbk Vol");
        
        ImGui::SameLine();
        ImGui::SetCursorPosX(feedbackEndX);
        ImGui::SetNextItemWidth(-FLT_MIN);
        sliderParameter(BPBX_FM_PARAM_FEEDBACK_VOLUME, "##vol", 0.0f, 15.0f, "%.0f");

        drawEnvelopes();

    } ImGui::End();
}

void PluginController::drawEnvelopes() {
    ImGui::SeparatorText("Envelopes");

    if (ImGui::Button("Add")) {
        if (envelopes.size() < BPBX_MAX_ENVELOPE_COUNT) {
            beepbox::bpbx_envelope_s new_env {};
            new_env.index = beepbox::BPBX_ENV_INDEX_NONE;
            new_env.curve_preset = 0,
            envelopes.push_back(new_env);

            gui_event_queue_item_s queue_item {};
            queue_item.type = GUI_EVENT_ADD_ENVELOPE;
            gui_to_plugin.enqueue(queue_item);
        }
    }

    beepbox::bpbx_inst_type_e instType = beepbox::bpbx_inst_type(instrument);
    const char **curveNames = beepbox::bpbx_envelope_curve_preset_names();

    for (int envIndex = 0; envIndex < envelopes.size(); envIndex++) {
        bpbx_envelope_s &env = envelopes[envIndex];
        bool isEnvDirty = false;

        const char *envTargetStr = beepbox::bpbx_envelope_index_name(env.index);
        assert(envTargetStr);

        ImGui::PushID(envIndex);

        // there will be a square deletion button at the right side of each line.
        // then, i want to have each combobox take half of the available content width,
        // and also have X spacing inbetween all items be the same as inner spacing X
        ImGuiStyle &style = ImGui::GetStyle();
        float itemWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetFrameHeight() - style.ItemInnerSpacing.x * 2) / 2.f;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemInnerSpacing.x, style.ItemSpacing.y));
        
        // envelope target combobox
        ImGui::SetNextItemWidth(itemWidth);
        if (ImGui::BeginCombo("##target", envTargetStr)) {
            std::vector<beepbox::bpbx_envelope_compute_index_e> targets;
            targets.push_back(beepbox::BPBX_ENV_INDEX_NONE);
            targets.push_back(beepbox::BPBX_ENV_INDEX_NOTE_VOLUME);

            {
                int size;
                const bpbx_envelope_compute_index_e *instTargets = beepbox::bpbx_envelope_targets(instType, &size);
                targets.reserve(targets.size() + size);

                for (int i = 0; i < size; i++) {
                    targets.push_back(instTargets[i]);
                }
            }

            for (int i = 0; i < targets.size(); i++) {
                const char *name = beepbox::bpbx_envelope_index_name(targets[i]);
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

void PluginController::draw(platform::Window *window) {
    updateParams();

    // begin imgui frame
    {
        simgui_frame_desc_t desc = {};
        desc.width = platform::getWidth(window);
        desc.height = platform::getHeight(window);
        desc.delta_time = 1.0f / 60.0f; // TODO
        simgui_new_frame(&desc);
    }

    // imgui
    {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Presets")) {
                ImGui::MenuItem("Test");
                ImGui::EndMenu();
            }
    
            if (ImGui::MenuItem("About")) {
                showAbout = !showAbout;
            }
    
            ImGui::EndMainMenuBar();
        }
    
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        if (showAbout) {
            drawAbout(winFlags);
        } else {
            drawFmGui(winFlags);
        }
    }

    // begin sokol pass
    {
        sg_pass pass = {};
        pass.swapchain = platform::sokolSwapchain(window);
        pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
        pass.action.colors[0].clear_value = {0.0f, 0.0f, 0.0f, 1.0f};
        sg_begin_pass(pass);
    }

    // render imgui
    simgui_render();

    // end pass
    sg_end_pass();
}