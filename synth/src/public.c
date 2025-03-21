#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include "public.h"
#include "fm.h"
#include "inst_info.h"
#include "util.h"

inst_t* inst_new(inst_type_t inst_type, int sample_rate, int channel_count) {
    init_wavetables();

    inst_t *inst = malloc(sizeof(inst_t));
    inst->type = inst_type;
    inst->sample_rate = sample_rate;
    inst->channel_count = channel_count;

    switch (inst_type) {
        case INSTRUMENT_FM:
            fm_init(&inst->fm);
            break;

        default:
            free(inst);
            return NULL;
    }

    return inst;
}

void inst_destroy(inst_t* inst) {
    free(inst);
}

const inst_param_info_t* inst_list_params(const inst_t* inst, int *count) {
    switch (inst->type) {
        case INSTRUMENT_FM:
            *count = FM_PARAM_COUNT;
            return fm_param_info;

        default:
            return NULL;
    }
}

static int param_helper(inst_t *inst, int index, void **addr, inst_param_info_t *info) {
    switch (inst->type) {
        case INSTRUMENT_FM:
            if (index < 0 || index >= FM_PARAM_COUNT) return 1;
            *info = fm_param_info[index];
            *addr = (void*)(((uint8_t*)&inst->fm) + fm_param_addresses[index]);
            break;

        default:
            return 1;
    }

    return 0;
}

int inst_set_param_int(inst_t* inst, int index, int value) {
    void *addr;
    inst_param_info_t info;

    if (param_helper(inst, index, &addr, &info))
        return 1;

    int val_clamped = value;
    if (info.min_value != info.max_value) {
        val_clamped = (int)clampf((float)value, info.min_value, info.max_value);
    }
    
    switch (info.type) {
        case PARAM_UINT8:
            *((uint8_t*)addr) = clampi(val_clamped, 0, UINT8_MAX);
            break;

        case PARAM_INT:
            *((int*)addr) = clampi(val_clamped, INT_MIN, INT_MAX);
            break;

        case PARAM_FLOAT:
            *((float*)addr) = (float)val_clamped;
            break;
    }

    return 0;
}

int inst_set_param_float(inst_t* inst, int index, float value) {
    void *addr;
    inst_param_info_t info;

    if (param_helper(inst, index, &addr, &info))
        return 1;

    float val_clamped = value;
    if (info.min_value != info.max_value) {
        val_clamped = clampf(value, info.min_value, info.max_value);
    }
    
    switch (info.type) {
        case PARAM_UINT8:
        case PARAM_INT:
            return 1;

        case PARAM_FLOAT:
            *((float*)addr) = val_clamped;
            break;
    }

    return 0;
}

int inst_get_param_int(inst_t* inst, int index, int *value) {
    void *addr;
    inst_param_info_t info;

    if (param_helper(inst, index, &addr, &info))
        return 1;

    switch (info.type) {
        case PARAM_UINT8:
            *value = (int) *((uint8_t*)addr);
            break;

        case PARAM_INT:
            *value = *((int*)addr);
            break;

        case PARAM_FLOAT:
            return 1;
    }

    return 0;
}

float inst_get_param_float(inst_t* inst, int index, float *value) {
    void *addr;
    inst_param_info_t info;

    if (param_helper(inst, index, &addr, &info))
        return 1;

    switch (info.type) {
        case PARAM_UINT8:
            *value = (float) *((uint8_t*)addr);
            break;

        case PARAM_INT:
            *value = (float) *((int*)addr);
            break;

        case PARAM_FLOAT:
            *value = *((float*)addr);
            break;
    }

    return 0;
}

void inst_midi_on(inst_t *inst, int key, int velocity) {
    switch (inst->type) {
        case INSTRUMENT_FM:
            fm_midi_on(&inst->fm, key, velocity);
            break;

        default: break;
    }
}

void inst_midi_off(inst_t *inst, int key, int velocity) {
    switch (inst->type) {
        case INSTRUMENT_FM:
            fm_midi_off(&inst->fm, key, velocity);
            break;

        default: break;
    }
}

void inst_run(inst_t* inst, float *out_samples, size_t frame_count) {
    switch (inst->type) {
        case INSTRUMENT_FM:
            fm_run(&inst->fm, out_samples, frame_count, inst->sample_rate, inst->channel_count);
            break;

        default:
            break;
    }
}