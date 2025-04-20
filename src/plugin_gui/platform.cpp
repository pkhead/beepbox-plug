#include "platform.hpp"
#include <cstdio>
#include <cassert>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <dxgi1_2.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")
#else
#error "win32 support only!"
#endif

namespace platform {
    struct Window {
        HWND hwnd;
        char uniqueClassName[64];

        int width;
        int height;
        platform::EventHandler evCallback;
        platform::DrawHandler drawCallback;

        IDXGISwapChain1 *swapchain;
        ID3D11RenderTargetView *backbuffer;

        void *userdata;
    };
}

ID3D11Device *s_device;
ID3D11DeviceContext *s_devcon;
IDXGIFactory2 *s_dxgiFactory;

void platform::setup() {
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT s = D3D11CreateDevice(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL, NULL, NULL, NULL, D3D11_SDK_VERSION,
        &s_device, &featureLevel, &s_devcon
    );
    assert(s == S_OK);

    s = CreateDXGIFactory(__uuidof(IDXGIFactory2), (void**)&s_dxgiFactory);
    assert(s == S_OK);
}

void platform::shutdown() {
    s_device->Release();
    s_devcon->Release();
    s_dxgiFactory->Release();
}

sg_environment platform::sokolEnvironment() {
    sg_environment env = {};
    env.d3d11.device = s_device;
    env.d3d11.device_context = s_devcon;
    env.defaults.color_format = SG_PIXELFORMAT_RGBA8UI;
    env.defaults.depth_format = SG_PIXELFORMAT_NONE;
    env.defaults.sample_count = 1;
    return env;
}

sg_swapchain platform::sokolSwapchain(platform::Window *window) {
    sg_swapchain sw = {};
    sw.d3d11.render_view = window->backbuffer;
    sw.width = window->width;
    sw.height = window->height;
    sw.color_format = SG_PIXELFORMAT_RGBA8UI;
    sw.depth_format = SG_PIXELFORMAT_NONE;
    sw.sample_count = 1;
    return sw;
}

void platform::present(platform::Window *window) {
    window->swapchain->Present(0, 0);
}

static inline void getMousePos(platform::Event &event, LPARAM lParam) {
    event.x = GET_X_LPARAM(lParam);
    event.y = GET_Y_LPARAM(lParam);
}

static platform::Key winKey(WPARAM k) {
    if (k >= 'A' && k <= 'Z') {
        return (platform::Key) ((int)(k - 'A') + (int)platform::Key::A);
    }

    if (k >= '0' && k <= '9') {
        return (platform::Key) ((int)(k - '0') + (int)platform::Key::Zero);
    }

    switch (k) {
        case VK_BACK:
            return platform::Key::Backspace;
        case VK_TAB:
            return platform::Key::Tab;
        case VK_RETURN:
            return platform::Key::Enter;
        case VK_LSHIFT:
            return platform::Key::LeftShift;
        case VK_LCONTROL:
            return platform::Key::LeftControl;
        case VK_LMENU:
            return platform::Key::LeftAlt;
        case VK_RSHIFT:
            return platform::Key::RightShift;
        case VK_RCONTROL:
            return platform::Key::RightControl;
        case VK_RMENU:
            return platform::Key::RightAlt;
        case VK_CAPITAL:
            return platform::Key::CapsLock;
        case VK_ESCAPE:
            return platform::Key::Escape;
        case VK_SPACE:
            return platform::Key::Space;
        case VK_PRIOR:
            return platform::Key::PageUp;
        case VK_NEXT:
            return platform::Key::PageDown;
        case VK_END:
            return platform::Key::End;
        case VK_HOME:
            return platform::Key::Home;
        case VK_LEFT:
            return platform::Key::Left;
        case VK_UP:
            return platform::Key::Up;
        case VK_RIGHT:
            return platform::Key::Right;
        case VK_DOWN:
            return platform::Key::Down;
        case VK_SNAPSHOT:
            return platform::Key::PrintScreen;
        case VK_DELETE:
            return platform::Key::Delete;
        case VK_LWIN:
            return platform::Key::LeftSuper;
        case VK_RWIN:
            return platform::Key::RightSuper;

        default:
            return platform::Key::None;
        
        // VK_NUMPAD*, VK_MULTIPLY, VK_ADD, VK_SUBTRACT, VK_DECIMAL, VK_DIVIDE
        // VK_F*
        // VK_NUMLOCK, VK_SCROLL
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

LRESULT CALLBACK MyWinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (debounce) return DefWindowProcA(hwnd, uMsg, wParam, lParam);
    DebounceHandle h;

    // fprintf(stderr, "msg: %u wParam: %llu lParam: %lld\n", uMsg, wParam, lParam);

    // NOTE: Might be NULL during initialisation
    platform::Window *window = (platform::Window*)GetWindowLongPtrA(hwnd, 0);
    if (window == NULL) return DefWindowProcA(hwnd, uMsg, wParam, lParam);

    platform::Event event;

    switch (uMsg)
    {
    case WM_MOUSEMOVE:
        event.type = platform::Event::MouseMove;
        getMousePos(event, lParam);
        window->evCallback(event, window);
        break;
    
    case WM_LBUTTONDOWN:
        SetCapture(hwnd);

        event.type = platform::Event::MouseDown;
        getMousePos(event, lParam);
        event.button = 0;
        window->evCallback(event, window);
        break;

    case WM_LBUTTONUP:
        ReleaseCapture();
        
        event.type = platform::Event::MouseUp;
        getMousePos(event, lParam);
        event.button = 0;
        window->evCallback(event, window);
        break;

    case WM_RBUTTONDOWN:
        SetCapture(hwnd);

        event.type = platform::Event::MouseDown;
        getMousePos(event, lParam);
        event.button = 1;
        window->evCallback(event, window);
        break;

    case WM_RBUTTONUP:
        ReleaseCapture();
        
        event.type = platform::Event::MouseUp;
        getMousePos(event, lParam);
        event.button = 1;
        window->evCallback(event, window);
        break;

    case WM_MOUSEWHEEL:
        event.type = platform::Event::MouseWheel;
        event.x = 0;
        event.y = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
        window->evCallback(event, window);
        break;
    
    case WM_KEYDOWN:
        event.type = platform::Event::KeyDown;
        event.key = winKey(wParam);
        if (event.key != platform::Key::None) window->evCallback(event, window);
        break;

    case WM_KEYUP:
        event.type = platform::Event::KeyUp;
        event.key = winKey(wParam);
        if (event.key != platform::Key::None) window->evCallback(event, window);
        break;

    
    case WM_TIMER:
        window->drawCallback(window);
        RedrawWindow(hwnd, 0, 0, RDW_INVALIDATE);
        break;
    default:
        break;
    }

    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

#define MY_TIMER_ID 1

platform::Window* platform::createWindow(int width, int height, const char *name, platform::EventHandler evCallback, platform::DrawHandler drawCallback) {
    platform::Window *platform = new platform::Window {};

    platform->width  = width;
    platform->height = height;
    platform->evCallback = evCallback;
    platform->drawCallback = drawCallback;

    LARGE_INTEGER timenow;
    QueryPerformanceCounter(&timenow);
    sprintf_s(platform->uniqueClassName, sizeof(platform->uniqueClassName), "%s-%llx", name, timenow.QuadPart);

    WNDCLASSEXA wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = MyWinProc;
    wc.lpszClassName = platform->uniqueClassName;
    wc.cbWndExtra    = 32; // leave space for our pointer we set
    ATOM result      = RegisterClassExA(&wc);
    assert(result != 0);

    platform->hwnd = CreateWindowExA(
        0L,
        platform->uniqueClassName,
        name,
        WS_CHILD | WS_CLIPSIBLINGS,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        GetDesktopWindow(),
        NULL,
        NULL,
        NULL);
    DWORD err = GetLastError();
    (void)err;
    assert(platform->hwnd != NULL);

    SetWindowLongPtrA((HWND)platform->hwnd, 0, (LONG_PTR)platform);

    // initialize swapchain/backbuffer
    {
        DXGI_SWAP_CHAIN_DESC1 scd;
        ZeroMemory(&scd, sizeof(scd));

        scd.BufferCount = 1;
        scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.SampleDesc.Count = 1;
        
        HRESULT hr = s_dxgiFactory->CreateSwapChainForHwnd(
            s_device,
            platform->hwnd, &scd, NULL, NULL, &platform->swapchain);
        assert(hr == S_OK);

        ID3D11Texture2D *pBackBuffer;
        platform->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        s_device->CreateRenderTargetView(pBackBuffer, NULL, &platform->backbuffer);
        pBackBuffer->Release();
    }

    return platform;
}

void platform::closeWindow(Window *platform) {
    platform->swapchain->Release();
    platform->backbuffer->Release();

    DestroyWindow(platform->hwnd);
    UnregisterClassA(platform->uniqueClassName, NULL);
    delete platform;
}

void platform::setParent(platform::Window *window, void* newParent)
{
    HWND oldParent = GetParent(window->hwnd);
    if (oldParent)
    {
        KillTimer(window->hwnd, MY_TIMER_ID);

        SetParent(window->hwnd, NULL);
        DefWindowProcA(window->hwnd, WM_UPDATEUISTATE, UIS_CLEAR, WS_CHILD);
        DefWindowProcA(window->hwnd, WM_UPDATEUISTATE, UIS_SET, WS_POPUP);
    }

    if (newParent)
    {
        SetParent(window->hwnd, (HWND)newParent);
        //memcpy(gui->plugin->paramValuesMain, gui->plugin->paramValuesAudio, sizeof(gui->plugin->paramValuesMain));
        DefWindowProcA(window->hwnd, WM_UPDATEUISTATE, UIS_CLEAR, WS_POPUP);
        DefWindowProcA(window->hwnd, WM_UPDATEUISTATE, UIS_SET, WS_CHILD);

        SetTimer(window->hwnd, MY_TIMER_ID, 10, NULL);
    }
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
    ShowWindow(window->hwnd, visible ? SW_SHOW : SW_HIDE);
}