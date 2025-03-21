#pragma once
#include <sokol/sokol_gfx.h>

namespace platform {
    struct Event {
        enum EventType {
            MouseDown,
            MouseUp,
            MouseMove
        } type;
        
        int x;
        int y;
        int button;
    };

    struct PlatformData;

    typedef void (*EventHandler)(Event event, PlatformData *platform);
    typedef void (*DrawHandler)(PlatformData *platform);

    extern sg_swapchain sokolSwapchain(PlatformData *platform);
    extern sg_environment sokolEnvironment(PlatformData *platform);
    extern void present(PlatformData *data);

    PlatformData* init(int width, int height, const char *name, EventHandler event, DrawHandler draw);
    void close(PlatformData *data);

    void setUserdata(PlatformData *platform, void *ud);
    void *getUserdata(PlatformData *platform);

    int getWidth(PlatformData *platform);
    int getHeight(PlatformData *platform);

    void setVisible(PlatformData *platform, bool visible);
    void setParent(PlatformData *platform, void *parentHandle);
}