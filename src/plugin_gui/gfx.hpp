#pragma once
#include <pugl/pugl.h>
#include "platform.hpp"

namespace gfx {
    struct WindowData;
    typedef void Texture;

    void setupWorld();
    void shutdownWorld();

    // set up the pugl backend + api-specific stuff for a window
    // before realization
    void setupWindow(platform::Window *window);

    // set up graphics resources for realization
    void realizeWindow(platform::Window *window);

    // unrealize the window
    void shutdownWindow(platform::Window *window);

    void beginFrame(platform::Window *window);
    void endFrame(platform::Window *window);

    // create a texture from RGBA8 pixel data. returns nullptr on error.
    Texture* createTexture(uint8_t *data, int width, int height);

    // free previously created texture
    void destroyTexture(Texture* texture);

    ImTextureID imguiTextureId(Texture *texture);
} // namespace gfx