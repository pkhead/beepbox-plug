#include "gfx.hpp"

#include <cassert>
#include <cstdio>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <pugl/stub.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <dxgi1_2.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")

struct gfx::WindowData {
    IDXGISwapChain1 *swapchain;
    ID3D11RenderTargetView *backbuffer;
};

static ID3D11Device *s_device;
static ID3D11DeviceContext *s_devcon;
static IDXGIFactory2 *s_dxgiFactory;

void gfx::setupWorld() {
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

void gfx::shutdownWorld() {
    s_device->Release();
    s_devcon->Release();
    s_dxgiFactory->Release();
}

void gfx::setupWindow(platform::Window *window) {
    puglSetBackend(window->puglView, puglStubBackend());
}

void gfx::realizeWindow(platform::Window *window) {
    // initialize swapchain/backbuffer
    PuglView *view = window->puglView;
    gfx::WindowData *gfxData = window->gfxData = new gfx::WindowData {};

    HWND hwnd = (HWND) puglGetNativeView(view);

    DXGI_SWAP_CHAIN_DESC1 scd;
    ZeroMemory(&scd, sizeof(scd));

    scd.BufferCount = 1;
    scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.SampleDesc.Count = 1;
    
    HRESULT hr = s_dxgiFactory->CreateSwapChainForHwnd(
        s_device,
        hwnd, &scd, NULL, NULL, &gfxData->swapchain);
    assert(hr == S_OK);

    ID3D11Texture2D *pBackBuffer;
    gfxData->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    s_device->CreateRenderTargetView(pBackBuffer, NULL, &gfxData->backbuffer);
    pBackBuffer->Release();

    ImGui_ImplDX11_Init(s_device, s_devcon);
}

void gfx::shutdownWindow(platform::Window *window) {
    ImGui_ImplDX11_Shutdown();

    if (window->gfxData->swapchain)
        window->gfxData->swapchain->Release();
    if (window->gfxData->backbuffer)
        window->gfxData->backbuffer->Release();
}

void gfx::beginFrame(platform::Window *window) {
    ImGui_ImplDX11_NewFrame();
}

void gfx::endFrame(platform::Window *window) {
    const float clearColor[4] = { 0.f, 0.f, 0.f, 0.f };
    s_devcon->OMSetRenderTargets(1, &window->gfxData->backbuffer, nullptr);
    s_devcon->ClearRenderTargetView(window->gfxData->backbuffer, clearColor);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    window->gfxData->swapchain->Present(0, 0);
}

gfx::Texture* gfx::createTexture(uint8_t *data, int width, int height) {
    // create texture resource
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.ArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.MipLevels = 1;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = data;
    initialData.SysMemPitch = width * sizeof(uint8_t) * 4;

    ID3D11Texture2D *tex = nullptr;
    HRESULT res = s_device->CreateTexture2D(&desc, &initialData, &tex);
    if (res != S_OK) {
        fprintf(stderr, "CreateTexture2D failed: %li\n", res);
        return nullptr;
    }

    assert(tex);

    // create the resource view
    ID3D11ShaderResourceView *srv = nullptr;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = -1;
    res = s_device->CreateShaderResourceView(tex, &srvDesc, &srv);
    if (res != S_OK) {
        fprintf(stderr, "CreateShaderResourceView failed: %li\n", res);
        return nullptr;
    }

    tex->Release();
    return (gfx::Texture*) srv;
}

void gfx::destroyTexture(Texture *texture) {
    if (texture)
        ((ID3D11ShaderResourceView*)texture)->Release();
}

ImTextureID gfx::imguiTextureId(Texture *texture) {
    return (ImTextureID) texture;
}