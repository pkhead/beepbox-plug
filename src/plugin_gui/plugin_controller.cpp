#define SOKOL_IMGUI_NO_SOKOL_APP
#include <imgui.h>
#include <sokol_gfx.h>
#include <sokol_log.h>
#include <sokol_imgui.h>
#include <stb_image.h>
#include <cstring>

#include "plugin_controller.hpp"
#include "util.hpp"

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

PluginController::PluginController(bpbx_inst_s *instrument) :
    instrument(instrument),
    plugin_to_gui(),
    gui_to_plugin()
{
    showAbout = false;
    useCustomColors = false;

    // initialize copy of plugin state
    sync();
}

void PluginController::sync() {
    bpbx_inst_type_e type = bpbx_inst_type(instrument);
    constexpr uint32_t param_count = sizeof(params)/sizeof(*params);
    assert(bpbx_param_count(type) == param_count);

    for (int i = 0; i < param_count; i++) {
        bpbx_inst_get_param_double(instrument, i, params+i);
    }

    envelopes.clear();
    envelopes.reserve(BPBX_MAX_ENVELOPE_COUNT);
    envelopes.resize(bpbx_inst_envelope_count(instrument));
    memcpy(envelopes.data(), bpbx_inst_get_envelope(instrument, 0), sizeof(bpbx_envelope_s) * envelopes.size());
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
    
    sameLineRightCol();
    ImGui::SetCursorPosX(feedbackEndX);
    ImGui::SetNextItemWidth(-FLT_MIN);
    sliderParameter(BPBX_FM_PARAM_FEEDBACK_VOLUME, "##vol", 0.0f, 15.0f, "%.0f");
}

void PluginController::drawEnvelopes() {
    ImGui::SeparatorText("Envelopes");

    if (ImGui::Button("Add")) {
        if (envelopes.size() < BPBX_MAX_ENVELOPE_COUNT) {
            bpbx_envelope_s new_env {};
            new_env.index = BPBX_ENV_INDEX_NONE;
            new_env.curve_preset = 0,
            envelopes.push_back(new_env);

            gui_event_queue_item_s queue_item {};
            queue_item.type = GUI_EVENT_ADD_ENVELOPE;
            gui_to_plugin.enqueue(queue_item);
        }
    }

    bpbx_inst_type_e instType = bpbx_inst_type(instrument);
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
        ImGuiStyle &style = ImGui::GetStyle();
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

void PluginController::sameLineRightCol() {
    ImGui::SameLine();
    ImGui::SetCursorPosX(uiRightCol);
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
        updateColors();
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

        // about window ui
        if (showAbout) {
            drawAbout(winFlags);

        // instrument ui
        } else {
            if (ImGui::Begin("inst", NULL, winFlags)) {
                uiRightCol = ImGui::GetFontSize() * 6.f;
                
                // volume
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Volume");
                sameLineRightCol();
                ImGui::SetNextItemWidth(-FLT_MIN);
                sliderParameter(BPBX_PARAM_VOLUME, "##volume", -25.0, 25.0, "%.0f");

                ImGui::AlignTextToFramePadding();
                ImGui::Text("Fade");
                sameLineRightCol();
                drawFadeWidget("fadectl", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight() * 1.75f));
        
                // fade in
                // ImGui::AlignTextToFramePadding();
                // ImGui::Text("Fadein");
                // sameLineRightCol();
                // ImGui::SetNextItemWidth(-FLT_MIN);
                // sliderParameter(BPBX_PARAM_FADE_IN, "##fadein", 0.0, 9.0, "%.0f");
        
                // // fade out
                // ImGui::AlignTextToFramePadding();
                // ImGui::Text("Fadeout");
                // sameLineRightCol();
                // ImGui::SetNextItemWidth(-FLT_MIN);
                // sliderParameter(BPBX_PARAM_FADE_OUT, "##fadeout", -4.0, 6.0, "%.0f");

                // specific instrument ui
                drawFmGui();

                drawEnvelopes();
            } ImGui::End();
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