#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "instrument.h"
#include "fm.h"
#include "util.h"

const int inst_param_count(inst_type_e type) {
    switch (type) {
        case INSTRUMENT_FM:
            return FM_PARAM_COUNT;

        default:
            return 0;
    }
}

const inst_param_info_s* inst_param_info(inst_type_e type) {
    switch (type) {
        case INSTRUMENT_FM:
            return fm_param_info;

        default:
            return NULL;
    }
}

inst_s* inst_new(inst_type_e inst_type) {
    init_wavetables();

    inst_s *inst = malloc(sizeof(inst_s));
    *inst = (inst_s) {
        .type = inst_type,
        .sample_rate = 0,
        .fade_in = 0.0,
        .fade_out = 0.0
    };

    switch (inst_type) {
        case INSTRUMENT_FM:
            inst->fm = malloc(sizeof(fm_inst_s));
            fm_init(inst->fm);
            break;

        default:
            free(inst);
            return NULL;
    }

    return inst;
}

void inst_destroy(inst_s* inst) {
    free(inst->fm);
    free(inst);
}

void inst_set_sample_rate(inst_s *inst, int sample_rate) {
    inst->sample_rate = sample_rate;
}

static int param_helper(inst_s *inst, int index, void **addr, inst_param_info_s *info) {
    switch (inst->type) {
        case INSTRUMENT_FM:
            if (index < 0 || index >= FM_PARAM_COUNT) return 1;
            *info = fm_param_info[index];
            *addr = (void*)(((uint8_t*)inst->fm) + fm_param_addresses[index]);
            break;

        default:
            return 1;
    }

    return 0;
}

int inst_set_param_int(inst_s* inst, int index, int value) {
    void *addr;
    inst_param_info_s info;

    if (param_helper(inst, index, &addr, &info))
        return 1;

    int val_clamped = value;
    if (info.min_value != info.max_value) {
        val_clamped = (int)clampd((double)value, info.min_value, info.max_value);
    }
    
    switch (info.type) {
        case PARAM_UINT8:
            *((uint8_t*)addr) = clampi(val_clamped, 0, UINT8_MAX);
            break;

        case PARAM_INT:
            *((int*)addr) = clampi(val_clamped, INT_MIN, INT_MAX);
            break;

        case PARAM_DOUBLE:
            *((double*)addr) = (double)val_clamped;
            break;
    }

    return 0;
}

int inst_set_param_double(inst_s* inst, int index, double value) {
    void *addr;
    inst_param_info_s info;

    if (param_helper(inst, index, &addr, &info))
        return 1;

    float val_clamped = value;
    if (info.min_value != info.max_value) {
        val_clamped = clampd(value, info.min_value, info.max_value);
    }
    
    switch (info.type) {
        case PARAM_UINT8:
        case PARAM_INT:
            return 1;

        case PARAM_DOUBLE:
            *((double*)addr) = val_clamped;
            break;
    }

    return 0;
}

int inst_get_param_int(inst_s* inst, int index, int *value) {
    void *addr;
    inst_param_info_s info;

    if (param_helper(inst, index, &addr, &info))
        return 1;

    switch (info.type) {
        case PARAM_UINT8:
            *value = (int) *((uint8_t*)addr);
            break;

        case PARAM_INT:
            *value = *((int*)addr);
            break;

        case PARAM_DOUBLE:
            return 1;
    }

    return 0;
}

int inst_get_param_double(inst_s* inst, int index, double *value) {
    void *addr;
    inst_param_info_s info;

    if (param_helper(inst, index, &addr, &info))
        return 1;

    switch (info.type) {
        case PARAM_UINT8:
            *value = (double) *((uint8_t*)addr);
            break;

        case PARAM_INT:
            *value = (double) *((int*)addr);
            break;

        case PARAM_DOUBLE:
            *value = *((double*)addr);
            break;
    }

    return 0;
}

void inst_midi_on(inst_s *inst, int key, int velocity) {
    switch (inst->type) {
        case INSTRUMENT_FM:
            fm_midi_on(inst, key, velocity);
            break;

        default: break;
    }
}

void inst_midi_off(inst_s *inst, int key, int velocity) {
    switch (inst->type) {
        case INSTRUMENT_FM:
            fm_midi_off(inst, key, velocity);
            break;

        default: break;
    }
}

void inst_run(inst_s* inst, float *out_samples, size_t frame_count) {
    switch (inst->type) {
        case INSTRUMENT_FM:
            fm_run(inst, out_samples, frame_count, inst->sample_rate);
            break;

        default:
            break;
    }
}