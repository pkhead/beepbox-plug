#ifdef CPLUG_WANT_GUI

#define SOKOL_IMGUI_NO_SOKOL_APP
#include <imgui.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_log.h>
#include "sokol_imgui.h"

#include "plugin.hpp"
#include "platform.hpp"

#define GUI_DEFAULT_WIDTH 240
#define GUI_DEFAULT_HEIGHT 360
#define GUI_RATIO_X 2
#define GUI_RATIO_Y 3

using namespace beepbox;

struct PluginGui
{
    Plugin *plugin;
    platform::Window *window;
    _simgui_state_t simgui_state;

    bool showAbout;

    int algo;
    int freq[4];
    float vol[4];
    int fdbkType;
    float fdbk;
};

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

void eventHandler(platform::Event ev, platform::Window *window) {
    PluginGui *gui = (PluginGui*) platform::getUserdata(window);

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
            simgui_add_mouse_wheel_event(0.f, ev.y);
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

void drawHandler(platform::Window *window) {
    PluginGui *gui = (PluginGui*) platform::getUserdata(window);
    Plugin *plug = gui->plugin;

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
        gui->algo = (int)       cplug_getParameterValue(plug, FM_PARAM_ALGORITHM);
        gui->freq[0] = (int)  cplug_getParameterValue(plug, FM_PARAM_FREQ1);
        gui->vol[0] = (float)   cplug_getParameterValue(plug, FM_PARAM_VOLUME1) * 15.f;
        gui->freq[1] = (int)  cplug_getParameterValue(plug, FM_PARAM_FREQ2);
        gui->vol[1] = (float)   cplug_getParameterValue(plug, FM_PARAM_VOLUME2) * 15.f;
        gui->freq[2] = (int)  cplug_getParameterValue(plug, FM_PARAM_FREQ3);
        gui->vol[2] = (float)   cplug_getParameterValue(plug, FM_PARAM_VOLUME3) * 15.f;
        gui->freq[3] = (int)  cplug_getParameterValue(plug, FM_PARAM_FREQ4);
        gui->vol[3] = (float)   cplug_getParameterValue(plug, FM_PARAM_VOLUME4) * 15.f;
        gui->fdbkType = (int)   cplug_getParameterValue(plug, FM_PARAM_FEEDBACK_TYPE);
        gui->fdbk = (float)     cplug_getParameterValue(plug, FM_PARAM_FEEDBACK_VOLUME) * 15.f;

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
            ImGui::Text("libraries: CPLUG, Dear ImGui");
            ImGui::End();
        } else {
            if (ImGui::Begin("fm", NULL, winFlags)) {
                // algorithm
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Algorithm");

                ImGui::SameLine();
                float algoEndX = ImGui::GetCursorPosX();
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::Combo("##algo", &gui->algo, algoNames, 13)) {
                    sendParamEventFromMain(plug, CPLUG_EVENT_PARAM_CHANGE_UPDATE, FM_PARAM_ALGORITHM, (double)gui->algo);
                }

                // operator parameters
                for (int op = 0; op < 4; op++) {
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("%s", name[op]);
                    ImGui::PushID(op);

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(algoEndX - ImGui::GetCursorPosX() - ImGui::GetStyle().ItemSpacing.x);
                    //ImGui::Text("%i", gui->freq[op]);
                    if (ImGui::Combo("##freq", &gui->freq[op], freqRatios, FM_FREQ_COUNT, ImGuiComboFlags_HeightLargest)) {
                        sendParamEventFromMain(plug, CPLUG_EVENT_PARAM_CHANGE_UPDATE, FM_PARAM_FREQ1 + op * 2, gui->freq[op]);
                    }
                    // if (ImGui::BeginCombo("##freq", freqRatios[gui->freq[op]], ImGuiComboFlags_HeightLargest)) {
                    //     for (int i = 0; i < sizeof(freqRatios) / sizeof(*freqRatios); i++) {
                    //         ImGui::Selectable(freqRatios[i]);
                    //     }
                    //     ImGui::EndCombo();
                    // }

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    if (ImGui::SliderFloat("##vol", &gui->vol[op], 0.0f, 15.0f, "%.0f")) {
                        sendParamEventFromMain(plug, CPLUG_EVENT_PARAM_CHANGE_UPDATE, FM_PARAM_VOLUME1 + op * 2, (double)gui->vol[op] / 15.f);
                    }
                    ImGui::PopID();
                }

                // feedback type
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Feedback");

                ImGui::SameLine();
                float feedbackEndX = ImGui::GetCursorPosX();
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::Combo("##fdbk", &gui->fdbkType, feedbackNames, FM_FEEDBACK_TYPE_COUNT)) {
                    sendParamEventFromMain(plug, CPLUG_EVENT_PARAM_CHANGE_UPDATE, FM_PARAM_FEEDBACK_TYPE, (double)gui->fdbkType);
                }

                // feedback volume
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Fdbk Vol");
                
                ImGui::SameLine();
                ImGui::SetCursorPosX(feedbackEndX);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::SliderFloat("##vol", &gui->fdbk, 0.0f, 15.0f, "%.0f")) {
                    sendParamEventFromMain(plug, CPLUG_EVENT_PARAM_CHANGE_UPDATE, FM_PARAM_FEEDBACK_VOLUME, (double)gui->fdbk / 15.f);
                }

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

int openGuiCount = 0;

void* cplug_createGUI(void* userPlugin)
{
    Plugin *plugin = (Plugin*)userPlugin;
    PluginGui *gui    = new PluginGui;
    gui->showAbout = false;

    if (openGuiCount == 0) {
        platform::setup();
    }

    gui->plugin = plugin;
    gui->window = platform::createWindow(GUI_DEFAULT_WIDTH, GUI_DEFAULT_HEIGHT, CPLUG_PLUGIN_NAME, eventHandler, drawHandler);
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

    openGuiCount++;
    return gui;
}

void cplug_destroyGUI(void* userGUI)
{
    PluginGui *gui       = (PluginGui*)userGUI;

    simgui_restore_global_state(&gui->simgui_state);
    simgui_set_current_context();
    simgui_shutdown();
    
    platform::closeWindow(gui->window);

    if (--openGuiCount == 0) {
        sg_shutdown();
        platform::shutdown();
    }
}

void cplug_setParent(void* userGUI, void* newParent)
{
    PluginGui *gui = (PluginGui*)userGUI;
    platform::setParent(gui->window, newParent);
}

void cplug_setVisible(void* userGUI, bool visible)
{
    PluginGui *gui = (PluginGui*)userGUI;
    platform::setVisible(gui->window, visible);
}

void cplug_setScaleFactor(void* userGUI, float scale)
{
    // handle change
}
void cplug_getSize(void* userGUI, uint32_t* width, uint32_t* height)
{
    PluginGui* gui = (PluginGui*)userGUI;
    *width = (uint32_t) platform::getWidth(gui->window);
    *height = (uint32_t) platform::getHeight(gui->window);
}
bool cplug_setSize(void* userGUI, uint32_t width, uint32_t height)
{
    PluginGui* gui  = (PluginGui*)userGUI;
    return false;
    // gui->width  = width;
    // gui->height = height;
    
    // return SetWindowPos(
    //     (HWND)gui->window,
    //     HWND_TOP,
    //     0,
    //     0,
    //     width,
    //     height,
    //     SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE);
}

void cplug_checkSize(void* userGUI, uint32_t* width, uint32_t* height)
{
    if (*width < (GUI_RATIO_X * 10))
        *width = (GUI_RATIO_X * 10);
    if (*height < (GUI_RATIO_Y * 10))
        *height = (GUI_RATIO_Y * 10);

    // This preserves the aspect ratio when resizing from a corner, or expanding horizontally/vertically.
    // Shrinking the window from the edge doesn't work, and I'm currently not sure how to disable resizing from the edge
    // Win/macOS aren't very helpful at letting you know which edge/corner the user is pulling from.
    // Some people wanting to preserve aspect ratio will disable resizing the window and add a widget in the corner
    // The user of this library is left to implement their own strategy
    uint32_t numX = *width / GUI_RATIO_X;
    uint32_t numY = *height / GUI_RATIO_Y;
    uint32_t num  = numX > numY ? numX : numY;
    *width        = num * GUI_RATIO_X;
    *height       = num * GUI_RATIO_Y;
}

bool cplug_getResizeHints(
    void*     userGUI,
    bool*     resizableX,
    bool*     resizableY,
    bool*     preserveAspectRatio,
    uint32_t* aspectRatioX,
    uint32_t* aspectRatioY)
{
    *resizableX          = false;
    *resizableY          = false;
    *preserveAspectRatio = true;
    *aspectRatioX        = GUI_RATIO_X;
    *aspectRatioY        = GUI_RATIO_Y;
    return true;
}

#endif // CPLUG_WANT_GUI