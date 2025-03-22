#pragma once
#include <cplug.h>
#include <beepbox_synth.h>

struct Plugin {
    int sampleRate;
    float* processBlock;

    beepbox::inst_type_t instType;
    beepbox::inst_t *synth;

    cplug_atomic_i32 mainToAudioHead;
    cplug_atomic_i32 mainToAudioTail;
    CplugEvent       mainToAudioQueue[CPLUG_EVENT_QUEUE_SIZE];
};

extern void sendParamEventFromMain(Plugin *plugin, uint32_t type, uint32_t paramIdx, double value);