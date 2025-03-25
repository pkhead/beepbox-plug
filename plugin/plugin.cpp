#include <cstring>
#include "plugin.hpp"

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

    const beepbox::inst_param_info_s *info = beepbox::inst_param_info(plug->instType);
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

    const beepbox::inst_param_info_s *info = beepbox::inst_param_info(plug->instType);
    *min = info[index].min_value;
    *max = info[index].max_value;
}

uint32_t cplug_getParameterFlags(void* ptr, uint32_t index)
{
    Plugin *plug = (Plugin*) ptr;
    const beepbox::inst_param_info_s *info = beepbox::inst_param_info(plug->instType);

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
    delete[] plugin->processBlock;
    
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