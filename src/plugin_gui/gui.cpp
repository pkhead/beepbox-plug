#define SOKOL_IMGUI_NO_SOKOL_APP
#include <imgui.h>
#include <sokol_gfx.h>
#include <sokol_log.h>
#include <sokol_imgui.h>
#include <stb_image.h>
#include <atomic>

#include "include/plugin_gui.h"
#include "platform.hpp"

#define GUI_DEFAULT_WIDTH 240
#define GUI_DEFAULT_HEIGHT 360
#define GUI_RATIO_X 2
#define GUI_RATIO_Y 3

using namespace beepbox;

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
};

typedef struct gui_plugin_interface_s {
    beepbox::inst_s *instrument;
    platform::Window *window;
    _simgui_state_t simgui_state;

    bool showAbout;

    double params[BASE_PARAM_COUNT + FM_PARAM_COUNT];

    EventQueue<gui_event_queue_item_s, GUI_EVENT_QUEUE_SIZE> gui_to_plugin;
    EventQueue<gui_event_queue_item_s, GUI_EVENT_QUEUE_SIZE> plugin_to_gui;

    void paramGestureBegin(uint32_t param_id) {
        gui_event_queue_item_s item = {};
        item.type = GUI_EVENT_PARAM_GESTURE_BEGIN;
        item.gesture.param_id = param_id;

        gui_to_plugin.enqueue(item);
    }

    void paramChange(uint32_t param_id, double value) {
        gui_event_queue_item_s item = {};
        item.type = GUI_EVENT_PARAM_CHANGE;
        item.param_value.param_id = param_id;
        item.param_value.value = value;

        gui_to_plugin.enqueue(item);

        params[param_id] = value;
    }

    void paramGestureEnd(uint32_t param_id) {
        gui_event_queue_item_s item = {};
        item.type = GUI_EVENT_PARAM_GESTURE_END;
        item.gesture.param_id = param_id;

        gui_to_plugin.enqueue(item);
    }
} gui_plugin_interface_s;

void gui_event_enqueue(gui_plugin_interface_s *iface, gui_event_queue_item_s item) {
    iface->plugin_to_gui.enqueue(item);
}

bool gui_event_dequeue(gui_plugin_interface_s *iface, gui_event_queue_item_s *item) {
    return iface->gui_to_plugin.dequeue(*item);
}

#ifdef PLUGIN_VST3
#include "resource/vst_logo.hpp"

struct {
    sg_image tex;
    sg_sampler smp;
    int width;
    int height;
} static vstLogo;
#endif

ImGuiKey keyToImgui(platform::Key key) {
    switch (key) {
        case platform::Key::Left: return ImGuiKey_LeftArrow;
        case platform::Key::Right: return ImGuiKey_RightArrow;
        case platform::Key::Up: return ImGuiKey_UpArrow;
        case platform::Key::Down: return ImGuiKey_DownArrow;
        case platform::Key::PageUp: return ImGuiKey_PageUp;
        case platform::Key::PageDown: return ImGuiKey_PageDown;
        case platform::Key::Home: return ImGuiKey_Home;
        case platform::Key::End: return ImGuiKey_End;
        case platform::Key::Insert: return ImGuiKey_Insert;
        case platform::Key::Delete: return ImGuiKey_Delete;
        case platform::Key::Backspace: return ImGuiKey_Backspace;
        case platform::Key::Space: return ImGuiKey_Space;
        case platform::Key::Enter: return ImGuiKey_Enter;
        case platform::Key::Escape: return ImGuiKey_Escape;
        case platform::Key::LeftControl: return ImGuiKey_LeftCtrl;
        case platform::Key::LeftShift: return ImGuiKey_LeftShift;
        case platform::Key::LeftAlt: return ImGuiKey_LeftAlt;
        case platform::Key::LeftSuper: return ImGuiKey_LeftSuper;
        case platform::Key::RightControl: return ImGuiKey_RightCtrl;
        case platform::Key::RightShift: return ImGuiKey_RightShift;
        case platform::Key::RightAlt: return ImGuiKey_RightAlt;
        case platform::Key::RightSuper: return ImGuiKey_RightSuper;
        case platform::Key::Zero: return ImGuiKey_0;
        case platform::Key::One: return ImGuiKey_1;
        case platform::Key::Two: return ImGuiKey_2;
        case platform::Key::Three: return ImGuiKey_3;
        case platform::Key::Four: return ImGuiKey_4;
        case platform::Key::Five: return ImGuiKey_5;
        case platform::Key::Six: return ImGuiKey_6;
        case platform::Key::Seven: return ImGuiKey_7;
        case platform::Key::Eight: return ImGuiKey_8;
        case platform::Key::Nine: return ImGuiKey_9;
        case platform::Key::A: return ImGuiKey_A;
        case platform::Key::B: return ImGuiKey_B;
        case platform::Key::C: return ImGuiKey_C;
        case platform::Key::D: return ImGuiKey_D;
        case platform::Key::E: return ImGuiKey_E;
        case platform::Key::F: return ImGuiKey_F;
        case platform::Key::G: return ImGuiKey_G;
        case platform::Key::H: return ImGuiKey_H;
        case platform::Key::I: return ImGuiKey_I;
        case platform::Key::J: return ImGuiKey_J;
        case platform::Key::K: return ImGuiKey_K;
        case platform::Key::L: return ImGuiKey_L;
        case platform::Key::M: return ImGuiKey_M;
        case platform::Key::N: return ImGuiKey_N;
        case platform::Key::O: return ImGuiKey_O;
        case platform::Key::P: return ImGuiKey_P;
        case platform::Key::Q: return ImGuiKey_Q;
        case platform::Key::R: return ImGuiKey_R;
        case platform::Key::S: return ImGuiKey_S;
        case platform::Key::T: return ImGuiKey_T;
        case platform::Key::U: return ImGuiKey_U;
        case platform::Key::V: return ImGuiKey_V;
        case platform::Key::W: return ImGuiKey_W;
        case platform::Key::X: return ImGuiKey_X;
        case platform::Key::Y: return ImGuiKey_Y;
        case platform::Key::Z: return ImGuiKey_Z;
        case platform::Key::Apostrophe: return ImGuiKey_Apostrophe;
        case platform::Key::Comma: return ImGuiKey_Comma;
        case platform::Key::Minus: return ImGuiKey_Minus;
        case platform::Key::Period: return ImGuiKey_Period;
        case platform::Key::Slash: return ImGuiKey_Slash;
        case platform::Key::Semicolon: return ImGuiKey_Semicolon;
        case platform::Key::Equal: return ImGuiKey_Equal;
        case platform::Key::LeftBracket: return ImGuiKey_LeftBracket;
        case platform::Key::Backslash: return ImGuiKey_Backslash;
        case platform::Key::RightBracket: return ImGuiKey_RightBracket;
        case platform::Key::GraveAccent: return ImGuiKey_GraveAccent;
        case platform::Key::CapsLock: return ImGuiKey_CapsLock;
        case platform::Key::ScrollLock: return ImGuiKey_ScrollLock;
        case platform::Key::NumLock: return ImGuiKey_NumLock;
        case platform::Key::PrintScreen: return ImGuiKey_PrintScreen;
        default: return ImGuiKey_None;
    }
}

void update_params(gui_plugin_interface_s *iface) {
    gui_event_queue_item_s item;
    while (iface->plugin_to_gui.dequeue(item)) {
        switch (item.type) {
            case GUI_EVENT_PARAM_CHANGE:
                iface->params[item.param_value.param_id] = item.param_value.value;
                break;

            default:
                break;
        }
    }
}

void eventHandler(platform::Event ev, platform::Window *window) {
    gui_plugin_interface_s *gui = (gui_plugin_interface_s*) platform::getUserdata(window);
    update_params(gui);

    simgui_restore_global_state(&gui->simgui_state);
    simgui_set_current_context();

    switch (ev.type) {
        case platform::Event::MouseMove:
            simgui_add_mouse_pos_event((float)ev.x, (float)ev.y);
            break;

        case platform::Event::MouseDown:
            simgui_add_mouse_button_event(ev.button, true);
            break;

        case platform::Event::MouseUp:
            simgui_add_mouse_button_event(ev.button, false);
            break;
        
        case platform::Event::MouseWheel:
            simgui_add_mouse_wheel_event(0.f, (float)ev.y);
            break;

        case platform::Event::KeyDown:
            simgui_add_key_event(keyToImgui(ev.key), true);
            break;

        case platform::Event::KeyUp:
            simgui_add_key_event(keyToImgui(ev.key), false);
            break;
    }

    simgui_save_global_state(&gui->simgui_state);
}

static void sliderParameter(gui_plugin_interface_s *gui, uint32_t paramId, const char *id, float v_min, float v_max, const char *fmt = "%.3f", bool normalized = false) {
    double v = gui->params[paramId];
    float floatV = (float)v;
    if (normalized) {
        floatV = (v_max - v_min) * floatV + v_min;
    }

    bool changed = ImGui::SliderFloat(id, &floatV, v_min, v_max, fmt);

    if (ImGui::IsItemActivated()) {
        gui->paramGestureBegin(paramId);
    }

    if (changed) {
        if (normalized) {
            floatV = (floatV - v_min) / (v_max - v_min);
        }

        gui->paramChange(paramId, (double)floatV);
    }

    if (ImGui::IsItemDeactivated()) {
        gui->paramGestureEnd(paramId);
    }
}

void drawHandler(platform::Window *window) {
    gui_plugin_interface_s *gui = (gui_plugin_interface_s*) platform::getUserdata(window);
    update_params(gui);

    simgui_restore_global_state(&gui->simgui_state);
    simgui_set_current_context();

    // operator parameters
    static const char *name[] = {
        "1.", "2.", "3.", "4."
    };

    static const char *freqRatios[] = {
        "0.12×",
        "0.25×",
        "0.5×",
        "0.75×",
        "1×",
        "~1×",
        "2×",
        "~2×",
        "3×",
        "3.5×",
        "4×",
        "~4×",
        "5×",
        "6×",
        "7×",
        "8×",
        "9×",
        "10×",
        "11×",
        "12×",
        "13×",
        "14×",
        "15×",
        //ultrabox
        "16×",
        "17×",
        //ultrabox
        "18×",
        "19×",
        //ultrabox
        "20×",
        "~20×",
        // dogebox (maybe another mod also adds this? I got it from dogebox)
        "25×",
        "50×",
        "75×",
        "100×",
        //50 and 100 are from dogebox
        //128 and 256 from slarmoo's box
        "128×",
        "256×",
    };
    static const char *algoNames[] = {
        "1←(2 3 4)",
        "1←(2 3←4)",
        "1←2←(3 4)",
        "1←(2 3)←4",
        "1←2←3←4",
        "1←3 2←4",
        "1 2←(3 4)",
        "1 2←3←4",
        "(1 2)←3←4",
        "(1 2)←(3 4)",
        "1 2 3←4",
        "(1 2 3)←4",
        "1 2 3 4",
    };

    static const char *feedbackNames[] = {
        "1⟲",
		"2⟲",
		"3⟲",
		"4⟲",
		"1⟲ 2⟲",
		"3⟲ 4⟲",
		"1⟲ 2⟲ 3⟲",
		"2⟲ 3⟲ 4⟲",
		"1⟲ 2⟲ 3⟲ 4⟲",
		"1→2",
		"1→3",
		"1→4",
		"2→3",
		"2→4",
		"3→4",
		"1→3 2→4",
		"1→4 2→3",
		"1→2→3→4",
    };

    {
        simgui_frame_desc_t desc = {};
        desc.width = platform::getWidth(window);
        desc.height = platform::getHeight(window);
        desc.delta_time = 1.0f / 60.0f; // TODO
        simgui_new_frame(&desc);
    }

    // imgui
    {
        int p_algo = (int) gui->params[FM_PARAM_ALGORITHM];
        int p_freq[4] = {
            (int) gui->params[FM_PARAM_FREQ1],
            (int) gui->params[FM_PARAM_FREQ2],
            (int) gui->params[FM_PARAM_FREQ3],
            (int) gui->params[FM_PARAM_FREQ4],
        };
        int p_fdbkType = (int) gui->params[FM_PARAM_FEEDBACK_TYPE];

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Presets")) {
                ImGui::MenuItem("Test");
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("About")) {
                gui->showAbout = !gui->showAbout;
            }

            ImGui::EndMainMenuBar();
        }

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
        if (gui->showAbout) {
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
        } else {
            if (ImGui::Begin("fm", NULL, winFlags)) {
                // volume
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Volume");
                ImGui::SameLine();
                sliderParameter(gui, PARAM_VOLUME, "##volume", -25.0, 25.0, "%.0f");

                // fade in
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Fadein");
                ImGui::SameLine();
                sliderParameter(gui, PARAM_FADE_IN, "##fadein", 0.0, 9.0, "%.0f");

                // fade out
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Fadeout");
                ImGui::SameLine();
                sliderParameter(gui, PARAM_FADE_OUT, "##fadeout", 0.0, 7.0, "%.0f");

                // algorithm
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Algorithm");

                ImGui::SameLine();
                float algoEndX = ImGui::GetCursorPosX();
                ImGui::SetNextItemWidth(-FLT_MIN);
                {
                    if (ImGui::Combo("##algo", &p_algo, algoNames, 13)) {
                        gui->paramGestureBegin(FM_PARAM_ALGORITHM);
                        gui->paramChange(FM_PARAM_ALGORITHM, (double)p_algo);
                        gui->paramGestureEnd(FM_PARAM_ALGORITHM);
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
                    if (ImGui::Combo("##freq", &p_freq[op], freqRatios, FM_FREQ_COUNT, ImGuiComboFlags_HeightLargest)) {
                        const uint32_t id = FM_PARAM_FREQ1 + op * 2;

                        gui->paramGestureBegin(id);
                        gui->paramChange(id, p_freq[op]);
                        gui->paramGestureEnd(id);
                    }
                    // if (ImGui::BeginCombo("##freq", freqRatios[gui->freq[op]], ImGuiComboFlags_HeightLargest)) {
                    //     for (int i = 0; i < sizeof(freqRatios) / sizeof(*freqRatios); i++) {
                    //         ImGui::Selectable(freqRatios[i]);
                    //     }
                    //     ImGui::EndCombo();
                    // }

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    sliderParameter(gui, FM_PARAM_VOLUME1 + op*2, "##vol", 0.0f, 15.0f, "%.0f", true);
                    ImGui::PopID();
                }

                // feedback type
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Feedback");

                ImGui::SameLine();
                float feedbackEndX = ImGui::GetCursorPosX();
                ImGui::SetNextItemWidth(-FLT_MIN);
                {
                    if (ImGui::Combo("##fdbk", &p_fdbkType, feedbackNames, FM_FEEDBACK_TYPE_COUNT)) {
                        gui->paramGestureBegin(FM_PARAM_FEEDBACK_TYPE);
                        gui->paramChange(FM_PARAM_FEEDBACK_TYPE, (double)p_fdbkType);
                        gui->paramGestureEnd(FM_PARAM_FEEDBACK_TYPE);
                    }
                }

                // feedback volume
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Fdbk Vol");
                
                ImGui::SameLine();
                ImGui::SetCursorPosX(feedbackEndX);
                ImGui::SetNextItemWidth(-FLT_MIN);
                sliderParameter(gui, FM_PARAM_FEEDBACK_VOLUME, "##vol", 0.0f, 15.0f, "%.0f", true);

            } ImGui::End();
        }
    }

    {
        sg_pass pass = {};
        pass.swapchain = platform::sokolSwapchain(window);
        pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
        pass.action.colors[0].clear_value = {0.0f, 0.0f, 0.0f, 1.0f};
        sg_begin_pass(pass);
    }

    simgui_render();

    sg_end_pass();

    sg_commit();
    platform::present(window);

    simgui_save_global_state(&gui->simgui_state);
}

static uint8_t leftArrowSymbol[] = {
    0b000000,
    0b000000,
    0b001000,
    0b010000,
    0b111111,
    0b010000,
    0b001000,
    0b000000,
    0b000000,
    0b000000,
    0b000000,
    0b000000,
};

static uint8_t rightArrowSymbol[] = {
    0b000000,
    0b000000,
    0b000100,
    0b000010,
    0b111111,
    0b000010,
    0b000100,
    0b000000,
    0b000000,
    0b000000,
    0b000000,
    0b000000,
};

static uint8_t feedbackSymbol[] = {
    0b000000,
    0b000000,
    0b101110,
    0b110001,
    0b111001,
    0b000001,
    0b010001,
    0b001110,
    0b000000,
    0b000000,
    0b000000,
    0b000000,
};

#define GET_PIXEL(x, y) (((y) * width) + (x)) * 4
static void symbol_patch(unsigned char *pixels, int width, int height, void *userdata) {
    int *idList = (int*)userdata;

    ImGuiIO &io = ImGui::GetIO();

    // left arrow
    ImFontAtlasCustomRect *rect = io.Fonts->GetCustomRectByIndex(idList[0]);
    
    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < 6; x++) {
            int pixIndex = GET_PIXEL(rect->X + x, rect->Y + y);

            pixels[pixIndex] = 255;
            pixels[pixIndex+1] = 255;
            pixels[pixIndex+2] = 255;
            pixels[pixIndex+3] = ((leftArrowSymbol[y] >> (5 - x)) & 1) ? 255 : 0;
        }
    }

    // right arrow
    rect = io.Fonts->GetCustomRectByIndex(idList[1]);
    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < 6; x++) {
            int pixIndex = GET_PIXEL(rect->X + x, rect->Y + y);

            pixels[pixIndex] = 255;
            pixels[pixIndex+1] = 255;
            pixels[pixIndex+2] = 255;
            pixels[pixIndex+3] = ((rightArrowSymbol[y] >> (5 - x)) & 1) ? 255 : 0;
        }
    }

    // loop symbol
    rect = io.Fonts->GetCustomRectByIndex(idList[2]);
    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < 6; x++) {
            int pixIndex = GET_PIXEL(rect->X + x, rect->Y + y);

            pixels[pixIndex] = 255;
            pixels[pixIndex+1] = 255;
            pixels[pixIndex+2] = 255;
            pixels[pixIndex+3] = ((feedbackSymbol[y] >> (5 - x)) & 1) ? 255 : 0;
        }
    }
}

static void graphicsInit() {
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

static void graphicsClose() {
    #ifdef PLUGIN_VST3
    sg_destroy_image(vstLogo.tex);
    sg_destroy_sampler(vstLogo.smp);
    #endif
}

int openGuiCount = 0;

bool gui_is_api_supported(const char *api, bool is_floating) {
    return !strcmp(api, CLAP_WINDOW_API_WIN32);
}

bool gui_get_preferred_api(const char **api, bool *is_floating) {
    *api = CLAP_WINDOW_API_WIN32;
    *is_floating = false;
    return true;
}

gui_plugin_interface_s* gui_create(beepbox::inst_s *instrument, const char *api, bool is_floating) {
    gui_plugin_interface_s *gui = new gui_plugin_interface_s {};
    gui->instrument = instrument;
    gui->showAbout = false;

    if (openGuiCount == 0) {
        platform::setup();
    }

    // initialize parameters
    {
        beepbox::inst_type_e type = beepbox::inst_type(instrument);
        constexpr uint32_t param_count = sizeof(gui->params)/sizeof(*gui->params);
        assert(beepbox::inst_param_count(type) == param_count);

        for (int i = 0; i < param_count; i++) {
            beepbox::inst_get_param_double(instrument, i, gui->params+i);
        }
    }
    
    gui->window = platform::createWindow(GUI_DEFAULT_WIDTH, GUI_DEFAULT_HEIGHT, "BeepBox", eventHandler, drawHandler);
    platform::setUserdata(gui->window, gui);

    if (openGuiCount == 0) {
        sg_desc desc = {};
        desc.environment = platform::sokolEnvironment();
        desc.logger.func = slog_func;
        sg_setup(&desc);
    }

    {
        simgui_desc_t desc = {};
        desc.no_default_font = true;
        simgui_setup(&desc);
    }
    ImGuiIO &io = ImGui::GetIO();
    
    ImFont *font = io.Fonts->AddFontDefault();

    int glyphIds[3];
    glyphIds[0] = io.Fonts->AddCustomRectFontGlyph(font, 0x2190, 6, 12, 7.f, ImVec2(0.f, 2.f)); // left arrow
    glyphIds[1] = io.Fonts->AddCustomRectFontGlyph(font, 0x2192, 6, 12, 7.f, ImVec2(0.f, 2.f)); // right arrow
    glyphIds[2] = io.Fonts->AddCustomRectFontGlyph(font, 0x27F2, 6, 12, 7.f, ImVec2(0.f, 2.f)); // loop symbol
    io.Fonts->Build();

    {
        simgui_font_tex_desc_t desc = {};
        desc.image_patch = symbol_patch;
        desc.userdata = glyphIds;
        simgui_create_fonts_texture(&desc);
    }

    simgui_save_global_state(&gui->simgui_state);

    if (openGuiCount++ == 0) {
        graphicsInit();
    }

    return gui;
}

void gui_destroy(gui_plugin_interface_s *gui) {
    simgui_restore_global_state(&gui->simgui_state);
    simgui_set_current_context();
    simgui_shutdown();
    
    platform::closeWindow(gui->window);

    if (--openGuiCount == 0) {
        graphicsClose();
        sg_shutdown();
        platform::shutdown();
    }
}

bool gui_get_size(const gui_plugin_interface_s *iface, uint32_t *width, uint32_t *height) {
    *width = (uint32_t) platform::getWidth(iface->window);
    *height = (uint32_t) platform::getHeight(iface->window);
    return true;
}

bool gui_set_parent(gui_plugin_interface_s *iface, const clap_window_t *window) {
    assert(!strcmp(window->api, CLAP_WINDOW_API_WIN32));
    platform::setParent(iface->window, window->win32);
    return true;
}

bool gui_set_transient(gui_plugin_interface_s *iface, const clap_window_t *window) {
    return false;
}

void gui_suggest_title(gui_plugin_interface_s *iface, const char *title) {

}

bool gui_show(gui_plugin_interface_s *iface) {
    platform::setVisible(iface->window, true);
    return true;
}

bool gui_hide(gui_plugin_interface_s *iface) {
    platform::setVisible(iface->window, false);
    return true;
}