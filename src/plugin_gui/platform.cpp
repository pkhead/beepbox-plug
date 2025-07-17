#include "platform.hpp"

#include <cstdio>
#include <cassert>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <pugl/pugl.h>
#include <pugl/stub.h>
#include "gfx.hpp"

#ifdef GFX_D3D11
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <dxgi1_2.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "dwmapi.lib")
#elif defined(GFX_OPENGL)
#include <pugl/gl.h>
#else
#error "no graphics backend chosen!"
#endif

static PuglWorld *puglWorld;
static const char *worldClassName = "BeepBoxPluginWindow";

#ifdef GFX_D3D11
static ID3D11Device *s_device;
static ID3D11DeviceContext *s_devcon;
static IDXGIFactory2 *s_dxgiFactory;
#endif

void platform::setup() {
    // initialize pugl
    puglWorld = puglNewWorld(PUGL_MODULE, 0);
    puglSetWorldString(puglWorld, PUGL_CLASS_NAME, worldClassName);

    gfx::setupWorld();
}

void platform::shutdown() {
    gfx::shutdownWorld();

    puglFreeWorld(puglWorld);
    puglWorld = nullptr;
}

static platform::Key puglKey(uint32_t k) {
    if (k >= 'A' && k <= 'Z') {
        return (platform::Key) ((int)(k - 'A') + (int)platform::Key::A);
    }

    if (k >= '0' && k <= '9') {
        return (platform::Key) ((int)(k - '0') + (int)platform::Key::Zero);
    }

    switch (k) {
        case PUGL_KEY_BACKSPACE:
            return platform::Key::Backspace;
        case PUGL_KEY_TAB:
            return platform::Key::Tab;
        case PUGL_KEY_ENTER:
            return platform::Key::Enter;
        case PUGL_KEY_SHIFT_L:
            return platform::Key::LeftShift;
        case PUGL_KEY_CTRL_L:
            return platform::Key::LeftControl;
        case PUGL_KEY_ALT_L:
            return platform::Key::LeftAlt;
        case PUGL_KEY_SHIFT_R:
            return platform::Key::RightShift;
        case PUGL_KEY_CTRL_R:
            return platform::Key::RightControl;
        case PUGL_KEY_ALT_R:
            return platform::Key::RightAlt;
        case PUGL_KEY_CAPS_LOCK:
            return platform::Key::CapsLock;
        case PUGL_KEY_ESCAPE:
            return platform::Key::Escape;
        case PUGL_KEY_SPACE:
            return platform::Key::Space;
        case PUGL_KEY_PAGE_UP:
            return platform::Key::PageUp;
        case PUGL_KEY_PAGE_DOWN:
            return platform::Key::PageDown;
        case PUGL_KEY_END:
            return platform::Key::End;
        case PUGL_KEY_HOME:
            return platform::Key::Home;
        case PUGL_KEY_LEFT:
            return platform::Key::Left;
        case PUGL_KEY_UP:
            return platform::Key::Up;
        case PUGL_KEY_RIGHT:
            return platform::Key::Right;
        case PUGL_KEY_DOWN:
            return platform::Key::Down;
        case PUGL_KEY_PRINT_SCREEN:
            return platform::Key::PrintScreen;
        case PUGL_KEY_DELETE:
            return platform::Key::Delete;
        case PUGL_KEY_SUPER_L:
            return platform::Key::LeftSuper;
        case PUGL_KEY_SUPER_R:
            return platform::Key::RightSuper;

        default:
            return platform::Key::None;
        
        // VK_NUMPAD*, VK_MULTIPLY, VK_ADD, VK_SUBTRACT, VK_DECIMAL, VK_DIVIDE
        // VK_F*
        // VK_NUMLOCK, VK_SCROLL
    }
}

static void setupGraphics(platform::Window *window) {
    // initialize imgui context
    ImGuiContext *ctx = ImGui::CreateContext();
    window->imguiCtx = ctx;
    ImGui::SetCurrentContext(ctx);

    ImGui::GetIO().IniFilename = nullptr;

    platform::Event ev;
    ev.type = platform::Event::Realize;
    window->evCallback(ev, window);

    gfx::setupWindow(window);
}

static void shutdownGraphics(platform::Window *window) {
    platform::Event ev;
    ev.type = platform::Event::Unrealize;
    window->evCallback(ev, window);

    gfx::shutdownWindow(window);

    if (window->imguiCtx) {
        ImGui::DestroyContext(window->imguiCtx);
        window->imguiCtx = nullptr;
    }
}

// opening a link with ImGui causes the program to crash i think?
// but for some reason when i added this it fixed the issue. What.
static bool debounce = false;
class DebounceHandle {
public:
    DebounceHandle() { debounce = true; }
    ~DebounceHandle() { debounce = false; }
};

static PuglStatus puglEventFunc(PuglView *view, const PuglEvent *rawEvent) {
    if (debounce) return PUGL_SUCCESS;
    DebounceHandle _;

    platform::Window *window = (platform::Window*) puglGetHandle(view);
    platform::Event event = {};

    if (window->imguiCtx) {
        ImGui::SetCurrentContext(window->imguiCtx);
    }

    switch (rawEvent->type) {
        case PUGL_REALIZE:
            setupGraphics(window);
            break;
        
        case PUGL_UNREALIZE:
            shutdownGraphics(window);
            break;

        case PUGL_MOTION:
            event.type = platform::Event::MouseMove;
            event.x = (int)rawEvent->motion.x;
            event.y = (int)rawEvent->motion.y;

            window->evCallback(event, window);
            puglObscureView(view);
            break;
        
        case PUGL_BUTTON_PRESS:
            event.type = platform::Event::MouseDown;
            event.button = rawEvent->button.button;
            event.x = rawEvent->button.x;
            event.y = rawEvent->button.y;

            window->evCallback(event, window);
            puglObscureView(view);
            break;

        case PUGL_BUTTON_RELEASE:
            event.type = platform::Event::MouseUp;
            event.button = rawEvent->button.button;
            event.x = rawEvent->button.x;
            event.y = rawEvent->button.y;

            window->evCallback(event, window);
            puglObscureView(view);
            break;

        case PUGL_SCROLL:
            event.type = platform::Event::MouseWheel;
            event.x = (int) rawEvent->scroll.dx;
            event.y = (int) rawEvent->scroll.dy;

            window->evCallback(event, window);
            puglObscureView(view);
            break;

        case PUGL_KEY_PRESS:
            event.type = platform::Event::KeyDown;
            event.key = puglKey(rawEvent->key.key);
            
            if (event.key != platform::Key::None)
            {
                window->evCallback(event, window);
                puglObscureView(view);
            }
            break;

        case PUGL_KEY_RELEASE:
            event.type = platform::Event::KeyUp;
            event.key = puglKey(rawEvent->key.key);
            
            if (event.key != platform::Key::None)
            {
                window->evCallback(event, window);
                puglObscureView(view);
            }
            break;
        
        case PUGL_EXPOSE: {
            // call ImGui new frame
            gfx::beginFrame(window);

            ImGuiIO &io = ImGui::GetIO();
            io.DisplaySize.x = window->width;
            io.DisplaySize.y = window->height;
            ImGui::NewFrame();

            // then, the window callback
            window->drawCallback(window);

            // render the ImGui frame
            ImGui::Render();
            gfx::endFrame(window);
            break;
        }

        default: break;
    }

    return PUGL_SUCCESS;
}

platform::Window* platform::createWindow(int width, int height, const char *name, platform::EventHandler evCallback, platform::DrawHandler drawCallback) {
    platform::Window *platform = new platform::Window {};

    platform->width  = width;
    platform->height = height;
    platform->evCallback = evCallback;
    platform->drawCallback = drawCallback;

    PuglView *view = platform->puglView = puglNewView(puglWorld);
    puglSetHandle(view, (PuglHandle*) platform);
    puglSetSizeHint(view, PUGL_DEFAULT_SIZE, width, height);
    puglSetViewHint(view, PUGL_RESIZABLE, false);
    puglSetEventFunc(view, puglEventFunc);

#ifdef GFX_D3D11
    puglSetBackend(view, puglStubBackend());
#elif defined(GFX_OPENGL)
    puglSetBackend(view, puglGlBackend());
#endif

    return platform;
}

void platform::closeWindow(Window *platform) {
    if (platform->puglView)
    {
        if (platform->isRealized)
            puglUnrealize(platform->puglView);

        puglFreeView(platform->puglView);
    }
    
    delete platform;
}

void platform::setParent(platform::Window *window, void* newParent)
{
    if (window->isRealized) return;

    puglSetParent(window->puglView, (PuglNativeView) newParent);
    
    PuglStatus status = puglRealize(window->puglView);
    if (status) {
        fprintf(stderr, "Error realizing view (%s\n)", puglStrerror(status));
        return;
    }
    window->isRealized = true;
}

void platform::setUserdata(platform::Window *window, void *ud) {
    window->userdata = ud;
}

void* platform::getUserdata(Window *window) {
    return window->userdata;
}

int platform::getWidth(Window *window) {
    return window->width;
}

int platform::getHeight(Window *window) {
    return window->height;
}

void platform::setVisible(Window *window, bool visible) {
    if (visible)
        puglShow(window->puglView, PUGL_SHOW_RAISE);
    else
        puglHide(window->puglView);
}