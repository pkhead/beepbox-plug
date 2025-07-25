#include "pugl/pugl.h"
#include <imgui.h>
#include <stb_image.h>
#include <cstring>

#include "include/plugin_gui.h"
#include "platform.hpp"
#include "plugin_controller.hpp"
#include "gfx.hpp"

#define GUI_DEFAULT_WIDTH 240
#define GUI_DEFAULT_HEIGHT 360
#define GUI_RATIO_X 2
#define GUI_RATIO_Y 3

typedef struct plugin_gui_s {
    platform::Window *window;
    PluginController control;

    plugin_gui_s(const clap_plugin_t *plugin, const clap_host_t *host, bpbx_inst_s *inst) : control(plugin, host, inst) {} 
} plugin_gui_s;

void gui_event_enqueue(plugin_gui_s *iface, gui_event_queue_item_s item) {
    iface->control.plugin_to_gui.enqueue(item);
}

bool gui_event_dequeue(plugin_gui_s *iface, gui_event_queue_item_s *item) {
    return iface->control.gui_to_plugin.dequeue(*item);
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

static void setUpGraphics(platform::Window *window) {
    // patch symbols to font atlas
    ImGuiIO &io = ImGui::GetIO();
    
    ImFont *font = io.Fonts->AddFontDefault();

    int glyphIds[3];
    glyphIds[0] = io.Fonts->AddCustomRectFontGlyph(font, 0x2190, 6, 12, 7.f, ImVec2(0.f, 2.f)); // left arrow
    glyphIds[1] = io.Fonts->AddCustomRectFontGlyph(font, 0x2192, 6, 12, 7.f, ImVec2(0.f, 2.f)); // right arrow
    glyphIds[2] = io.Fonts->AddCustomRectFontGlyph(font, 0x27F2, 6, 12, 7.f, ImVec2(0.f, 2.f)); // loop symbol
    io.Fonts->Build();

    uint8_t *pixels;
    int width;
    int height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    symbol_patch(pixels, width, height, (void*)glyphIds);

    plugin_gui_s *gui = (plugin_gui_s*) platform::getUserdata(window);
    gui->control.graphicsInit();
}

static void closeGraphics(platform::Window *window) {
    plugin_gui_s *gui = (plugin_gui_s*) platform::getUserdata(window);
    gui->control.graphicsClose();
}

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
    plugin_gui_s *gui = (plugin_gui_s*) platform::getUserdata(window);
    ImGuiIO &io = ImGui::GetIO();

    switch (ev.type) {
        case platform::Event::MouseMove:
            io.AddMousePosEvent((float)ev.x, (float)ev.y);
            platform::requestRedraw(window);
            break;

        case platform::Event::MouseDown:
            io.AddMouseButtonEvent(ev.button, true);
            platform::requestRedraw(window);
            break;

        case platform::Event::MouseUp:
            io.AddMouseButtonEvent(ev.button, false);

            // recalculate the frame two more times after releasing the button
            // as imgui sometimes defers actions to the next frame.
            // (and then wait an extra frame in case layout changed from this action,
            // in which case it will fix the scroll position on the frame afterwards)
            platform::requestRedraw(window, 2);
            break;
        
        case platform::Event::MouseWheel:
            io.AddMouseWheelEvent((float)ev.x, (float)ev.y);
            platform::requestRedraw(window);
            break;

        case platform::Event::KeyDown:
            io.AddKeyEvent(keyToImgui(ev.key), true);
            platform::requestRedraw(window);
            break;

        case platform::Event::KeyUp:
            io.AddKeyEvent(keyToImgui(ev.key), false);
            platform::requestRedraw(window);
            break;            

        case platform::Event::Realize:
            setUpGraphics(window);

            // request extra frame, since the ImGui menu bar does not appear
            // on the first ever frame
            platform::requestRedraw(window, 1);
            break;

        case platform::Event::Unrealize:
            closeGraphics(window);
            break;
    }

    gui->control.event(ev, window);
}

void drawHandler(platform::Window *window) {
    plugin_gui_s *gui = (plugin_gui_s*) platform::getUserdata(window);
    gui->control.draw(window);
}

int openGuiCount = 0;

bool gui_is_api_supported(const char *api, bool is_floating) {
#ifdef _WIN32
    return !strcmp(api, CLAP_WINDOW_API_WIN32);
#else
    return !strcmp(api, CLAP_WINDOW_API_X11);
#endif
}

bool gui_get_preferred_api(const char **api, bool *is_floating) {
#ifdef _WIN32
    *api = CLAP_WINDOW_API_WIN32;
#elif defined(__APPLE__)
    *api = CLAP_WINDOW_API_COCOA;
#else
    *api = CLAP_WINDOW_API_X11;
#endif

    *is_floating = false;
    return true;
}

plugin_gui_s* gui_create(const gui_creation_params_s *params) {
    plugin_gui_s *gui = new plugin_gui_s(params->plugin, params->host, params->instrument);
    gui->control.popupContextMenu = params->show_context_menu;
    gui->control.pluginUserdata = params->userdata;

    if (openGuiCount == 0) {
        platform::setup();
    }
    
    gui->window = platform::createWindow(GUI_DEFAULT_WIDTH, GUI_DEFAULT_HEIGHT, "BeepBox", eventHandler, drawHandler);
    platform::setUserdata(gui->window, gui);

    openGuiCount++;
    return gui;
}

void gui_destroy(plugin_gui_s *gui) {
    platform::closeWindow(gui->window);

    if (--openGuiCount == 0) {
        platform::shutdown();
    }
}

void gui_sync_state(plugin_gui_s *gui) {
    gui->control.sync();
}

bool gui_get_size(const plugin_gui_s *iface, uint32_t *width, uint32_t *height) {
    *width = (uint32_t) platform::getWidth(iface->window);
    *height = (uint32_t) platform::getHeight(iface->window);
    return true;
}

bool gui_set_parent(plugin_gui_s *iface, const clap_window_t *window) {
#ifdef _WIN32
    assert(!strcmp(window->api, CLAP_WINDOW_API_WIN32));
    return platform::setParent(iface->window, window->win32);
#else
    assert(!strcmp(window->api, CLAP_WINDOW_API_X11));
    return platform::setParent(iface->window, (void*)window->x11);
#endif
}

bool gui_set_transient(plugin_gui_s *iface, const clap_window_t *window) {
#ifdef _WIN32
    assert(!strcmp(window->api, CLAP_WINDOW_API_WIN32));
    return platform::setTransientParent(iface->window, window->win32);
#else
    assert(!strcmp(window->api, CLAP_WINDOW_API_X11));
    return platform::setTransientParent(iface->window, (void*)window->x11);
#endif
}

void gui_suggest_title(plugin_gui_s *iface, const char *title) {
    platform::setTitle(iface->window, title);
}

bool gui_show(plugin_gui_s *iface) {
    platform::setVisible(iface->window, true);
    return true;
}

bool gui_hide(plugin_gui_s *iface) {
    platform::setVisible(iface->window, false);
    return true;
}

void gui_update_color(plugin_gui_s *gui, clap_color_t color) {
    if (color.alpha == 0) {
        gui->control.useCustomColors = false;
    } else {
        gui->control.useCustomColors = true;
        gui->control.customColor = Color(
            (float)color.red / 255.f,
            (float)color.green / 255.f,
            (float)color.blue / 255.f
        );
    }
}