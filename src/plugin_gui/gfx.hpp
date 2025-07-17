#pragma once
#include <pugl/pugl.h>
#include "platform.hpp"

namespace gfx {
    struct WindowData;
    typedef void Texture;

    void setupWorld();
    void shutdownWorld();

    void setupWindow(platform::Window *window);
    void shutdownWindow(platform::Window *window);

    void beginFrame(platform::Window *window);
    void endFrame(platform::Window *window);

    const PuglBackend* getPuglBackend();

    // create a texture from RGBA8 pixel data. returns nullptr on error.
    Texture* createTexture(uint8_t *data, int width, int height);

    // free previously created texture
    void destroyTexture(Texture* texture);
} // namespace gfx