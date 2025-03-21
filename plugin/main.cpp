#include <cplug.h>

#ifdef CPLUG_WANT_GUI
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_log.h>
#endif

// Apparently denormals aren't a problem on ARM & M1?
// https://en.wikipedia.org/wiki/Subnormal_number
// https://www.kvraudio.com/forum/viewtopic.php?t=575799
#if __arm64__
#define DISABLE_DENORMALS
#define ENABLE_DENORMALS
#elif defined(_WIN32)
#include <immintrin.h>
#define DISABLE_DENORMALS _mm_setcsr(_mm_getcsr() & ~0x8040);
#define ENABLE_DENORMALS _mm_setcsr(_mm_getcsr() | 0x8040);
#else
#include <fenv.h>
#define DISABLE_DENORMALS                                                                                              \
    fenv_t _fenv;                                                                                                      \
    fegetenv(&_fenv);                                                                                                  \
    fesetenv(FE_DFL_DISABLE_SSE_DENORMS_ENV);
#define ENABLE_DENORMALS fesetenv(&_fenv);
#endif

static_assert((int)CPLUG_NUM_PARAMS == kParameterCount, "Must be equal");

struct Plugin {
    float phase;
};

void cplug_libraryLoad(){};
void cplug_libraryUnload(){};

void* cplug_createPlugin() {
    Plugin *plug = new Plugin;
    plug->phase = 0.f;
    return plug;
}

void cplug_destroyPlugin(void *ptr) {
    Plugin *plug = (Plugin*)ptr;
    delete plug;
}

uint32_t cplug_getInputBusChannelCount(void* ptr, uint32_t idx)
{
    if (idx == 0)
        return 2; // 1 bus, stereo
    return 0;
}

uint32_t cplug_getOutputBusChannelCount(void* ptr, uint32_t idx)
{
    if (idx == 0)
        return 2; // 1 bus, stereo
    return 0;
}

const char* cplug_getInputBusName(void* ptr, uint32_t idx)
{
    if (idx == 0)
        return "Stereo Input";
    return "";
}

const char* cplug_getOutputBusName(void* ptr, uint32_t idx)
{
    if (idx == 0)
        return "Stereo Output";
    return "";
}

const char* cplug_getParameterName(void* ptr, uint32_t index)
{
    static const char* param_names[] = {
        "Algorithm",
        "Operator 1 Freq.",
        "Operator 1 Vol.",
        "Operator 2 Freq.",
        "Operator 2 Vol.",
        "Operator 3 Freq.",
        "Operator 3 Vol.",
        "Operator 4 Freq.",
        "Operator 4 Vol.",
    };

    static_assert((sizeof(param_names) / sizeof(param_names[0])) == kParameterCount, "Invalid length");
    return param_names[index];
}

double cplug_getParameterValue(void* ptr, uint32_t index)
{
    return 0.0;
}

double cplug_getDefaultParameterValue(void* ptr, uint32_t index)
{
    return 0.0;
}

void cplug_setParameterValue(void* ptr, uint32_t index, double value)
{
    return;
}

double cplug_denormaliseParameterValue(void* ptr, uint32_t index, double normalised)
{
    return normalised;
}

double cplug_normaliseParameterValue(void* ptr, uint32_t index, double denormalised)
{
    return denormalised;
}

double cplug_parameterStringToValue(void* ptr, uint32_t index, const char* str)
{
    return 0.0;
}

void cplug_parameterValueToString(void* ptr, uint32_t index, char* buf, size_t bufsize, double value)
{
    
}

void cplug_getParameterRange(void* ptr, uint32_t index, double* min, double* max)
{
    *min = 0.0;
    *max = 1.0;
}

uint32_t cplug_getParameterFlags(void* ptr, uint32_t index)
{
    return 0;
}

uint32_t cplug_getLatencyInSamples(void* ptr) { return 0; }
uint32_t cplug_getTailInSamples(void* ptr) { return 0; }

void cplug_setSampleRateAndBlockSize(void* ptr, double sampleRate, uint32_t maxBlockSize)
{
    // MyPlugin* plugin      = (MyPlugin*)ptr;
    // plugin->sampleRate    = (float)sampleRate;
    // plugin->maxBufferSize = maxBlockSize;
}

void cplug_process(void* ptr, CplugProcessContext* ctx)
{
    DISABLE_DENORMALS
    
    ENABLE_DENORMALS
}

void cplug_saveState(void* userPlugin, const void* stateCtx, cplug_writeProc writeProc)
{
    
}

void cplug_loadState(void* userPlugin, const void* stateCtx, cplug_readProc readProc)
{
    
}

#ifdef CPLUG_WANT_GUI
#include "platform.hpp"

#define GUI_DEFAULT_WIDTH 640
#define GUI_DEFAULT_HEIGHT 360
#define GUI_RATIO_X 16
#define GUI_RATIO_Y 9

struct PluginGui
{
    Plugin *plugin;
    platform::PlatformData *window;
};

static void drawGUI(PluginGui *gui)
{
    // my_assert(gui->width > 0);
    // my_assert(gui->height > 0);
    // drawRect(gui, 0, gui->width, 0, gui->height, 0xC0C0C0, 0xC0C0C0);
    // drawRect(gui, 10, 40, 10, 40, 0x000000, 0xC0C0C0);

    // double paramFloat = gui->plugin->paramValuesMain[kParameterFloat];
    // paramFloat        = cplug_normaliseParameterValue(gui->plugin, kParameterFloat, paramFloat);

    // drawRect(gui, 10, 40, 10 + 30 * (1.0 - paramFloat), 40, 0x000000, 0x000000);
}

// bool tickGUI(PluginGui *gui)
// {
//     // FLOAT color[4] = {1.f, 0.f, 0.f, 1.f};
//     // gui->devcon->ClearRenderTargetView(gui->backbuffer, color);
//     // gui->swapchain->Present(0, 0);

//     {
//         sg_pass pass = {};
//         pass.swapchain.d3d11.render_view = gui->backbuffer;
//         pass.swapchain.width = gui->width;
//         pass.swapchain.height = gui->height;
//         pass.swapchain.color_format = SG_PIXELFORMAT_RGBA8UI;
//         pass.swapchain.depth_format = SG_PIXELFORMAT_NONE;
//         pass.swapchain.sample_count = 1;
//         pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
//         pass.action.colors[0].clear_value = {1.0f, 0.0f, 0.0f, 1.0f};
//         sg_begin_pass(&pass);
//     }
    
//     sg_end_pass();
//     gui->swapchain->Present(0, 0);

//     return false;
// }

void eventHandler(platform::Event ev, platform::PlatformData *window) {

}

void drawHandler(platform::PlatformData *window) {
    {
        sg_pass pass = {};
        pass.swapchain = platform::sokolSwapchain(window);
        pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
        pass.action.colors[0].clear_value = {1.0f, 0.0f, 0.0f, 1.0f};
        sg_begin_pass(pass);
    }

    sg_end_pass();
    platform::present(window);
}

void* cplug_createGUI(void* userPlugin)
{
    Plugin *plugin = (Plugin*)userPlugin;
    PluginGui *gui    = new PluginGui;

    gui->plugin = plugin;
    gui->window = platform::init(GUI_DEFAULT_WIDTH, GUI_DEFAULT_HEIGHT, CPLUG_PLUGIN_NAME, eventHandler, drawHandler);

    sg_desc desc = {};
    desc.environment = platform::sokolEnvironment(gui->window);
    desc.logger.func = slog_func;
    sg_setup(&desc);

    return gui;
}

void cplug_destroyGUI(void* userGUI)
{
    PluginGui *gui       = (PluginGui*)userGUI;
    platform::close(gui->window);
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