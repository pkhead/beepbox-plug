#include "platform.hpp"
#include <cstdio>
#include <cassert>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#else
#error "win32 support only!"
#endif

namespace platform {
    struct PlatformData {
        HWND window;
        char uniqueClassName[64];

        int width;
        int height;
        platform::EventHandler evCallback;
        platform::DrawHandler drawCallback;

        IDXGISwapChain *swapchain;
        ID3D11Device *dev;
        ID3D11DeviceContext *devcon;
        ID3D11RenderTargetView *backbuffer;

        void *userdata;
    };
}

sg_environment platform::sokolEnvironment(platform::PlatformData *platform) {
    sg_environment env = {};
    env.d3d11.device = platform->dev;
    env.d3d11.device_context = platform->devcon;
    env.defaults.color_format = SG_PIXELFORMAT_RGBA8UI;
    env.defaults.depth_format = SG_PIXELFORMAT_NONE;
    env.defaults.sample_count = 1;
    return env;
}

sg_swapchain platform::sokolSwapchain(platform::PlatformData *platform) {
    sg_swapchain sw = {};
    sw.d3d11.render_view = platform->backbuffer;
    sw.width = platform->width;
    sw.height = platform->height;
    sw.color_format = SG_PIXELFORMAT_RGBA8UI;
    sw.depth_format = SG_PIXELFORMAT_NONE;
    sw.sample_count = 1;
    return sw;
}

void platform::present(platform::PlatformData *platform) {
    platform->swapchain->Present(0, 0);
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

LRESULT CALLBACK MyWinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // fprintf(stderr, "msg: %u wParam: %llu lParam: %lld\n", uMsg, wParam, lParam);

    // NOTE: Might be NULL during initialisation
    platform::PlatformData *platform = (platform::PlatformData*)GetWindowLongPtrA(hwnd, 0);
    if (platform == NULL) return DefWindowProcA(hwnd, uMsg, wParam, lParam);

    platform::Event event;

    switch (uMsg)
    {
    case WM_MOUSEMOVE:
        event.type = platform::Event::MouseMove;
        getMousePos(event, lParam);
        platform->evCallback(event, platform);
        // if (gui->mouseDragging)
        //     RedrawWindow(hwnd, 0, 0, RDW_INVALIDATE);
        break;
    
    case WM_LBUTTONDOWN:
        SetCapture(hwnd);

        event.type = platform::Event::MouseDown;
        getMousePos(event, lParam);
        event.button = 0;
        platform->evCallback(event, platform);
        break;

    case WM_LBUTTONUP:
        ReleaseCapture();
        
        event.type = platform::Event::MouseUp;
        getMousePos(event, lParam);
        event.button = 0;
        platform->evCallback(event, platform);
        break;

    case WM_RBUTTONDOWN:
        SetCapture(hwnd);

        event.type = platform::Event::MouseDown;
        getMousePos(event, lParam);
        event.button = 1;
        platform->evCallback(event, platform);
        break;

    case WM_RBUTTONUP:
        ReleaseCapture();
        
        event.type = platform::Event::MouseUp;
        getMousePos(event, lParam);
        event.button = 1;
        platform->evCallback(event, platform);
        break;

    case WM_KEYDOWN:
        event.type = platform::Event::KeyDown;
        event.key = winKey(wParam);
        if (event.key != platform::Key::None) platform->evCallback(event, platform);
        break;

    case WM_KEYUP:
        event.type = platform::Event::KeyUp;
        event.key = winKey(wParam);
        if (event.key != platform::Key::None) platform->evCallback(event, platform);
        break;
    
    case WM_TIMER:
        // if (tickGUI(gui))
        platform->drawCallback(platform);
        RedrawWindow(hwnd, 0, 0, RDW_INVALIDATE);
        break;
    default:
        break;
    }

    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

#define MY_TIMER_ID 1

platform::PlatformData* platform::init(int width, int height, const char *name, platform::EventHandler evCallback, platform::DrawHandler drawCallback) {
    platform::PlatformData *platform = new platform::PlatformData;
    memset(platform, 0, sizeof(platform::PlatformData));

    platform->width  = width;
    platform->height = height;
    platform->evCallback = evCallback;
    platform->drawCallback = drawCallback;
    //gui->img    = (uint32_t*)realloc(gui->img, gui->width * gui->height * sizeof(*gui->img));

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

    platform->window = CreateWindowExA(
        0L,
        platform->uniqueClassName,
        CPLUG_PLUGIN_NAME,
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
    assert(platform->window != NULL);

    SetWindowLongPtrA((HWND)platform->window, 0, (LONG_PTR)platform);

    // initialize d3d11
    {
        DXGI_SWAP_CHAIN_DESC scd;
        ZeroMemory(&scd, sizeof(scd));

        scd.BufferCount = 1;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.OutputWindow = platform->window;
        scd.SampleDesc.Count = 1;
        scd.Windowed = TRUE;

        D3D11CreateDeviceAndSwapChain(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL, NULL, NULL, NULL,
            D3D11_SDK_VERSION,
            &scd, &platform->swapchain, &platform->dev, NULL, &platform->devcon
        );

        ID3D11Texture2D *pBackBuffer;
        platform->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        platform->dev->CreateRenderTargetView(pBackBuffer, NULL, &platform->backbuffer);
        pBackBuffer->Release();

        // gui->devcon->OMSetRenderTargets(1, &gui->backbuffer, NULL);

        // D3D11_VIEWPORT viewport;
        // ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

        // viewport.TopLeftX = 0;
        // viewport.TopLeftY = 0;
        // viewport.Width = 800;
        // viewport.Height = 600;

        // gui->devcon->RSSetViewports(1, &viewport);
    }

    return platform;
}

void platform::close(PlatformData *platform) {
    platform->swapchain->Release();
    platform->backbuffer->Release();
    platform->dev->Release();
    platform->devcon->Release();

    DestroyWindow(platform->window);
    UnregisterClassA(platform->uniqueClassName, NULL);
    delete platform;
}

void platform::setParent(platform::PlatformData *platform, void* newParent)
{
    HWND oldParent = GetParent(platform->window);
    if (oldParent)
    {
        KillTimer(platform->window, MY_TIMER_ID);

        SetParent(platform->window, NULL);
        DefWindowProcA(platform->window, WM_UPDATEUISTATE, UIS_CLEAR, WS_CHILD);
        DefWindowProcA(platform->window, WM_UPDATEUISTATE, UIS_SET, WS_POPUP);
    }

    if (newParent)
    {
        SetParent(platform->window, (HWND)newParent);
        //memcpy(gui->plugin->paramValuesMain, gui->plugin->paramValuesAudio, sizeof(gui->plugin->paramValuesMain));
        DefWindowProcA(platform->window, WM_UPDATEUISTATE, UIS_CLEAR, WS_POPUP);
        DefWindowProcA(platform->window, WM_UPDATEUISTATE, UIS_SET, WS_CHILD);

        SetTimer(platform->window, MY_TIMER_ID, 10, NULL);
    }
}

void platform::setUserdata(PlatformData *platform, void *ud) {
    platform->userdata = ud;
}

void* platform::getUserdata(PlatformData *platform) {
    return platform->userdata;
}

int platform::getWidth(PlatformData *platform) {
    return platform->width;
}

int platform::getHeight(PlatformData *platform) {
    return platform->height;
}

void platform::setVisible(PlatformData *platform, bool visible) {
    ShowWindow(platform->window, visible ? SW_SHOW : SW_HIDE);
}