#include "gfx.hpp"
#include "pugl/pugl.h"

#include <cassert>
#include <cstdio>
#include <pugl/gl.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>

void gfx::setupWorld() {}
void gfx::shutdownWorld() {}

void gfx::setupWindow(platform::Window *window) {
    PuglView *view = window->puglView;
    puglSetBackend(view, puglGlBackend());

    puglSetViewHint(view, PUGL_CONTEXT_VERSION_MAJOR, 3);
    puglSetViewHint(view, PUGL_CONTEXT_VERSION_MINOR, 3);
    puglSetViewHint(view, PUGL_CONTEXT_PROFILE, PUGL_OPENGL_CORE_PROFILE);
    puglSetViewHint(view, PUGL_SWAP_INTERVAL, 0);
}

void gfx::realizeWindow(platform::Window *window) {
    // ImGui_ImplDX11_Init(s_device, s_devcon);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

void gfx::shutdownWindow(platform::Window *window) {
    ImGui_ImplOpenGL3_Shutdown();
}

void gfx::beginFrame(platform::Window *window) {
    ImGui_ImplOpenGL3_NewFrame();
}

void gfx::endFrame(platform::Window *window) {
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // buffer swap is done by pugl
}

gfx::Texture* gfx::createTexture(uint8_t *data, int width, int height) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return (gfx::Texture*)(size_t)tex;
}

void gfx::destroyTexture(Texture *texture) {
    if (texture) {
        GLuint tex = (GLuint)(size_t)texture;
        glDeleteTextures(1, &tex);
    }
}

ImTextureID gfx::imguiTextureId(Texture *texture) {
    return (ImTextureID)(size_t)texture;
}