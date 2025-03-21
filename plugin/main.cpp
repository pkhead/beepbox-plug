#include <cplug.h>
#include <beepbox_synth.h>

#ifdef CPLUG_WANT_GUI
#define SOKOL_IMGUI_NO_SOKOL_APP
#include <imgui.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_log.h>
#include <sokol/util/sokol_imgui.h>
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

static_assert((int)CPLUG_NUM_PARAMS == FM_PARAM_COUNT, "Must be equal");

struct Plugin {
    int sampleRate;
    float* processBlock;

    beepbox::inst_type_t instType;
    beepbox::inst_t *synth;

    cplug_atomic_i32 mainToAudioHead;
    cplug_atomic_i32 mainToAudioTail;
    CplugEvent       mainToAudioQueue[CPLUG_EVENT_QUEUE_SIZE];
};

void cplug_libraryLoad(){};
void cplug_libraryUnload(){};

void* cplug_createPlugin() {
    Plugin *plug = new Plugin;
    memset(plug, 0, sizeof(Plugin));

    plug->instType = beepbox::INSTRUMENT_FM;
    plug->sampleRate = 0;
    plug->processBlock = nullptr;
    plug->synth = beepbox::inst_new(plug->instType);

    return plug;
}

void cplug_destroyPlugin(void *ptr) {
    Plugin *plug = (Plugin*)ptr;

    if (plug->processBlock) delete[] plug->processBlock;
    beepbox::inst_destroy(plug->synth);
    
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
    Plugin *plug = (Plugin*)ptr;
    return beepbox::inst_param_info(plug->instType)[index].name;
}

double cplug_getParameterValue(void* ptr, uint32_t index)
{
    Plugin *plug = (Plugin*) ptr;

    if (beepbox::inst_param_info(plug->instType)[index].type == beepbox::PARAM_DOUBLE) {
        double v;
        beepbox::inst_get_param_double(plug->synth, index, &v);
        return v;
    } else {
        int v;
        beepbox::inst_get_param_int(plug->synth, index, &v);
        return (double) v;
    }
}

double cplug_getDefaultParameterValue(void* ptr, uint32_t index)
{
    Plugin *plug = (Plugin*) ptr;

    if (index < beepbox::inst_param_count(plug->instType)) return 0.0;

    const beepbox::inst_param_info_t *info = beepbox::inst_param_info(plug->instType);
    return (double) info[index].default_value;
}

void cplug_setParameterValue(void* ptr, uint32_t index, double value)
{
    Plugin *plug = (Plugin*) ptr;

    if (beepbox::inst_param_info(plug->instType)[index].type == beepbox::PARAM_DOUBLE) {
        beepbox::inst_set_param_double(plug->synth, index, value);
    } else {
        beepbox::inst_set_param_int(plug->synth, index, (int)value);
    }
}

double cplug_denormaliseParameterValue(void* ptr, uint32_t index, double normalised)
{
    Plugin *plug = (Plugin*) ptr;
    auto &info = beepbox::inst_param_info(plug->instType)[index];

    double denormalized = normalised * (info.max_value - info.min_value) + info.min_value;

    if (denormalized < info.min_value)
        denormalized = info.min_value;
    if (denormalized > info.max_value)
        denormalized = info.max_value;

    return denormalized;
}

double cplug_normaliseParameterValue(void* ptr, uint32_t index, double denormalised)
{
    Plugin *plug = (Plugin*) ptr;
    auto &info = beepbox::inst_param_info(plug->instType)[index];

    double normalized = (denormalised - info.min_value) / (info.max_value - info.min_value);
    assert(normalized == normalized); // inf/nan check

    if (normalized < 0.0)
        normalized = 0.0;
    if (normalized > 1.0)
        normalized = 1.0;
    
    return normalized;
}

double cplug_parameterStringToValue(void* ptr, uint32_t index, const char* str)
{
    Plugin *plug = (Plugin*) ptr;
    double value;
    int pType = beepbox::inst_param_info(plug->instType)[index].type;

    if (pType == beepbox::PARAM_INT || pType == beepbox::PARAM_UINT8)
        value = (double)atoi(str);
    else
        value = atof(str);

    return value;
}

void cplug_parameterValueToString(void* ptr, uint32_t index, char* buf, size_t bufsize, double value)
{
    Plugin *plug = (Plugin*) ptr;
    int pType = beepbox::inst_param_info(plug->instType)[index].type;

    if (pType == beepbox::PARAM_INT || pType == beepbox::PARAM_UINT8) {
        snprintf(buf, bufsize, "%d", (int)value);
    } else {
        snprintf(buf, bufsize, "%.2f", value);
    }
}

void cplug_getParameterRange(void* ptr, uint32_t index, double* min, double* max)
{
    Plugin *plug = (Plugin*) ptr;

    if (index < beepbox::inst_param_count(plug->instType)) return;

    const beepbox::inst_param_info_t *info = beepbox::inst_param_info(plug->instType);
    *min = info[index].min_value;
    *max = info[index].max_value;
}

uint32_t cplug_getParameterFlags(void* ptr, uint32_t index)
{
    Plugin *plug = (Plugin*) ptr;
    const beepbox::inst_param_info_t *info = beepbox::inst_param_info(plug->instType);

    uint32_t flags = 0;
    if (info[index].type == beepbox::PARAM_INT || info[index].type == beepbox::PARAM_UINT8)
        flags |= CPLUG_FLAG_PARAMETER_IS_INTEGER;

    if (!info[index].no_modulation)
        flags |= CPLUG_FLAG_PARAMETER_IS_AUTOMATABLE;

    return flags;
}

uint32_t cplug_getLatencyInSamples(void* ptr) { return 0; }
uint32_t cplug_getTailInSamples(void* ptr) { return 0; }

void cplug_setSampleRateAndBlockSize(void* ptr, double sampleRate, uint32_t maxBlockSize)
{
    Plugin *plugin = (Plugin*) ptr;
    plugin->sampleRate = (int) sampleRate;
    plugin->processBlock = new float[maxBlockSize * 2];
    beepbox::inst_set_sample_rate(plugin->synth, (int)sampleRate);
}

void cplug_process(void* ptr, CplugProcessContext* ctx)
{
    DISABLE_DENORMALS

    Plugin *plug = (Plugin*) ptr;
    CPLUG_LOG_ASSERT(plug->processBlock != nullptr);
    CPLUG_LOG_ASSERT(plug->synth != nullptr);

    // Audio thread has chance to respond to incoming GUI events before being sent to the host
    int head = cplug_atomic_load_i32(&plug->mainToAudioHead) & CPLUG_EVENT_QUEUE_MASK;
    int tail = cplug_atomic_load_i32(&plug->mainToAudioTail);

    while (tail != head)
    {
        CplugEvent* event = &plug->mainToAudioQueue[tail];

        if (event->type == CPLUG_EVENT_PARAM_CHANGE_UPDATE)
            cplug_setParameterValue(plug, event->parameter.idx, event->parameter.value);

        ctx->enqueueEvent(ctx, event, 0);

        tail++;
        tail &= CPLUG_EVENT_QUEUE_MASK;
    }
    cplug_atomic_exchange_i32(&plug->mainToAudioTail, tail);
     
    CplugEvent event;
    uint32_t frame = 0;
    while (ctx->dequeueEvent(ctx, &event, frame)) {
        switch (event.type) {
            case CPLUG_EVENT_PARAM_CHANGE_UPDATE:
                cplug_setParameterValue(plug, event.parameter.idx, event.parameter.value);
                break;
            case CPLUG_EVENT_MIDI: {
                constexpr uint8_t MIDI_NOTE_OFF         = 0x80;
                constexpr uint8_t MIDI_NOTE_ON          = 0x90;
                //static const uint8_t MIDI_NOTE_PITCH_WHEEL = 0xe0;

                if ((event.midi.status & 0xf0) == MIDI_NOTE_ON)
                {
                    int note = event.midi.data1;
                    int velocity = event.midi.data2;

                    if (velocity == 0)
                        beepbox::inst_midi_off(plug->synth, note, velocity);
                    else
                        beepbox::inst_midi_on(plug->synth, note, velocity);
                }
                if ((event.midi.status & 0xf0) == MIDI_NOTE_OFF)
                {
                    int note = event.midi.data1;
                    int velocity = event.midi.data2;
                    beepbox::inst_midi_off(plug->synth, note, velocity);
                }
                
                break;
            }

            case CPLUG_EVENT_PROCESS_AUDIO: {
                float **output = ctx->getAudioOutput(ctx, 0);
                CPLUG_LOG_ASSERT(output != NULL)
                CPLUG_LOG_ASSERT(output[0] != NULL);
                CPLUG_LOG_ASSERT(output[1] != NULL);

                uint32_t blockSize = event.processAudio.endFrame - frame;
                beepbox::inst_run(plug->synth, plug->processBlock, (size_t)blockSize);

                int j = 0;
                for (uint32_t i = 0; i < blockSize; i++) {
                    output[0][frame+i] = plug->processBlock[j++];
                    output[1][frame+i] = plug->processBlock[j++];
                }

                frame = event.processAudio.endFrame;

                break;
            }
        }
    }
    
    ENABLE_DENORMALS
}

void cplug_saveState(void* userPlugin, const void* stateCtx, cplug_writeProc writeProc)
{
    
}

void cplug_loadState(void* userPlugin, const void* stateCtx, cplug_readProc readProc)
{
    
}

void sendParamEventFromMain(Plugin *plugin, uint32_t type, uint32_t paramIdx, double value)
{
    int         mainToAudioHead = cplug_atomic_load_i32(&plugin->mainToAudioHead) & CPLUG_EVENT_QUEUE_MASK;
    CplugEvent* paramEvent      = &plugin->mainToAudioQueue[mainToAudioHead];
    paramEvent->parameter.type  = type;
    paramEvent->parameter.idx   = paramIdx;
    paramEvent->parameter.value = value;

    cplug_atomic_fetch_add_i32(&plugin->mainToAudioHead, 1);
    cplug_atomic_fetch_and_i32(&plugin->mainToAudioHead, CPLUG_EVENT_QUEUE_MASK);

    // request_flush from CLAP host? Doesn't seem to be required
}

#ifdef CPLUG_WANT_GUI
#include "platform.hpp"

#define GUI_DEFAULT_WIDTH 240
#define GUI_DEFAULT_HEIGHT 360
#define GUI_RATIO_X 2
#define GUI_RATIO_Y 3

struct PluginGui
{
    Plugin *plugin;
    platform::PlatformData *window;

    int algo;
    int freq[4];
    float vol[4];
    int fdbkType;
    float fdbk;
};

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

void eventHandler(platform::Event ev, platform::PlatformData *window) {
    switch (ev.type) {
        case platform::Event::MouseMove:
            simgui_add_mouse_pos_event((float)ev.x, (float)ev.y);
            break;

        case platform::Event::MouseDown:
            simgui_add_mouse_button_event(ev.button, true);
            break;

        case platform::Event::MouseUp:
            simgui_add_mouse_button_event(ev.button, false);
            break;

        case platform::Event::KeyDown:
            simgui_add_key_event(keyToImgui(ev.key), true);
            break;

        case platform::Event::KeyUp:
            simgui_add_key_event(keyToImgui(ev.key), false);
            break;
    }
}

void drawHandler(platform::PlatformData *window) {
    // operator parameters
    static const char *name[] = {
        "1.", "2.", "3.", "4."
    };

    static const char *freqRatios[] = {
        "0.12×",
        "0.25×",
        "0.5×",
        "0.75×",
        "1×",
        "~1×",
        "2×",
        "~2×",
        "3×",
        "3.5×",
        "4×",
        "~4×",
        "5×",
        "6×",
        "7×",
        "8×",
        "9×",
        "10×",
        "11×",
        "12×",
        "13×",
        "14×",
        "15×",
        //ultrabox
        "16×",
        "17×",
        //ultrabox
        "18×",
        "19×",
        //ultrabox
        "20×",
        "~20×",
        // dogebox (maybe another mod also adds this? I got it from dogebox)
        "25×",
        "50×",
        "75×",
        "100×",
        //50 and 100 are from dogebox
        //128 and 256 from slarmoo's box
        "128x",
        "256x",
    };

    static const char *algoNames[] = {
        "1 <- (2 3 4)",
        "1 <- (2 3 <- 4)",
        "1 <- 2 <- (3 4)",
        "1 <- (2 3) <- 4",
        "1 <- 2 <- 3 <- 4",
        "1 <- 3  2 <- 4",
        "1  2 <- (3 4)",
        "1  2 <- 3 <- 4",
        "(1 2) <- 3 <- 4",
        "(1 2) <- (3 4)",
        "1  2  3 <- 4",
        "(1 2 3) <- 4",
        "1  2  3  4",
    };

    {
        simgui_frame_desc_t desc = {};
        desc.width = platform::getWidth(window);
        desc.height = platform::getHeight(window);
        desc.delta_time = 1.0f / 60.0f; // TODO
        simgui_new_frame(&desc);
    }

    // imgui
    {
        PluginGui *gui = (PluginGui*) platform::getUserdata(window);
        Plugin *plug = gui->plugin;

        gui->algo = (int)       cplug_getParameterValue(plug, FM_PARAM_ALGORITHM);
        gui->freq[0] = (int)  cplug_getParameterValue(plug, FM_PARAM_FREQ1);
        gui->vol[0] = (float)   cplug_getParameterValue(plug, FM_PARAM_VOLUME1) * 15.f;
        gui->freq[1] = (int)  cplug_getParameterValue(plug, FM_PARAM_FREQ2);
        gui->vol[1] = (float)   cplug_getParameterValue(plug, FM_PARAM_VOLUME2) * 15.f;
        gui->freq[2] = (int)  cplug_getParameterValue(plug, FM_PARAM_FREQ3);
        gui->vol[2] = (float)   cplug_getParameterValue(plug, FM_PARAM_VOLUME3) * 15.f;
        gui->freq[3] = (int)  cplug_getParameterValue(plug, FM_PARAM_FREQ4);
        gui->vol[3] = (float)   cplug_getParameterValue(plug, FM_PARAM_VOLUME4) * 15.f;
        gui->fdbkType = (int)   cplug_getParameterValue(plug, FM_PARAM_FEEDBACK_TYPE);
        gui->fdbk = (float)     cplug_getParameterValue(plug, FM_PARAM_FEEDBACK_VOLUME) * 15.f;

        static bool showAbout = false;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Options")) {
                ImGui::MenuItem("Advanced Frequency");
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Presets")) {
                ImGui::MenuItem("Test");
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("About")) {
                showAbout = !showAbout;
            }

            ImGui::EndMainMenuBar();
        }

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
        if (showAbout) {
            ImGui::Begin("about", NULL, winFlags);
            ImGui::Text("emulation of beepbox instruments");
            ImGui::Text("author: pkhead");
            ImGui::Text("original: john nesky (shaktool)");
            ImGui::Text("libraries: CPLUG, Dear ImGui");
            ImGui::End();
        } else {
            if (ImGui::Begin("fm", NULL, winFlags)) {
                // algorithm
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Algorithm");

                ImGui::SameLine();
                float algoEndX = ImGui::GetCursorPosX();
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::Combo("##algo", &gui->algo, algoNames, 13)) {
                    sendParamEventFromMain(plug, CPLUG_EVENT_PARAM_CHANGE_UPDATE, FM_PARAM_ALGORITHM, (double)gui->algo);
                }

                for (int op = 0; op < 4; op++) {
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("%s", name[op]);
                    ImGui::PushID(op);

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(algoEndX - ImGui::GetCursorPosX() - ImGui::GetStyle().ItemSpacing.x);
                    //ImGui::Text("%i", gui->freq[op]);
                    if (ImGui::Combo("##freq", &gui->freq[op], freqRatios, FM_FREQ_COUNT, ImGuiComboFlags_HeightLargest)) {
                        sendParamEventFromMain(plug, CPLUG_EVENT_PARAM_CHANGE_UPDATE, FM_PARAM_FREQ1 + op * 2, gui->freq[op]);
                    }
                    // if (ImGui::BeginCombo("##freq", freqRatios[gui->freq[op]], ImGuiComboFlags_HeightLargest)) {
                    //     for (int i = 0; i < sizeof(freqRatios) / sizeof(*freqRatios); i++) {
                    //         ImGui::Selectable(freqRatios[i]);
                    //     }
                    //     ImGui::EndCombo();
                    // }

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    if (ImGui::SliderFloat("##vol", &gui->vol[op], 0.0f, 15.0f, "%.0f")) {
                        sendParamEventFromMain(plug, CPLUG_EVENT_PARAM_CHANGE_UPDATE, FM_PARAM_VOLUME1 + op * 2, (double)gui->vol[op] / 15.f);
                    }
                    ImGui::PopID();
                }

            } ImGui::End();
        }
    }

    {
        sg_pass pass = {};
        pass.swapchain = platform::sokolSwapchain(window);
        pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
        pass.action.colors[0].clear_value = {0.0f, 0.0f, 0.0f, 1.0f};
        sg_begin_pass(pass);
    }

    simgui_render();

    sg_end_pass();

    sg_commit();
    platform::present(window);
}

void* cplug_createGUI(void* userPlugin)
{
    Plugin *plugin = (Plugin*)userPlugin;
    PluginGui *gui    = new PluginGui;

    gui->plugin = plugin;
    gui->window = platform::init(GUI_DEFAULT_WIDTH, GUI_DEFAULT_HEIGHT, CPLUG_PLUGIN_NAME, eventHandler, drawHandler);
    platform::setUserdata(gui->window, gui);

    sg_desc desc = {};
    desc.environment = platform::sokolEnvironment(gui->window);
    desc.logger.func = slog_func;
    sg_setup(&desc);

    {
        simgui_desc_t desc = {};
        simgui_setup(&desc);
    }

    return gui;
}

void cplug_destroyGUI(void* userGUI)
{
    PluginGui *gui       = (PluginGui*)userGUI;
    simgui_shutdown();
    sg_shutdown();
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