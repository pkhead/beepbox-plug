#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "instrument.h"
#include "fm.h"
#include "util.h"

#define PARTS_PER_BEAT 24
#define TICKS_PER_PART 2
#define VOLUME_LOG_SCALE 0.1428

static inst_param_info_s base_param_info[] = {
    {
        .name = "Volume",
        .type = PARAM_DOUBLE,
        .min_value = -25.0,
        .max_value = 25.0,
        .default_value = 0.0
    },
    {
        .name = "Panning",
        .type = PARAM_DOUBLE,
        .min_value = 0.0,
        .max_value = 100.0,
        .default_value = 50.0
    },
    {
        .name = "Fade In",
        .type = PARAM_DOUBLE,
        .min_value = 0.0,
        .max_value = 9.0,
        .default_value = 0.0
    },
    {
        .name = "Fade Out",
        .type = PARAM_DOUBLE,
        .min_value = 0.0,
        .max_value = 7.0,
        .default_value = 0.0
    }
};

size_t base_param_offsets[BASE_PARAM_COUNT] = {
    offsetof(inst_s, volume),
    offsetof(inst_s, fade_in),
    offsetof(inst_s, fade_out)
};

const unsigned int inst_param_count(inst_type_e type) {
    switch (type) {
        case INSTRUMENT_FM:
            return BASE_PARAM_COUNT + FM_PARAM_COUNT;

        default:
            return 0;
    }
}

const inst_param_info_s* inst_param_info(inst_type_e type, unsigned int index) {
    if (index < BASE_PARAM_COUNT)
        return &base_param_info[index];

    switch (type) {
        case INSTRUMENT_FM:
            return &fm_param_info[index - BASE_PARAM_COUNT];

        default:
            return NULL;
    }
}

inst_s* inst_new(inst_type_e type) {
    init_wavetables();

    inst_s *inst = malloc(sizeof(inst_s));
    *inst = (inst_s) {
        .type = type,
        .sample_rate = 0,

        .volume = 0.0,
        .panning = 50.0,
        .fade_in = 0.0,
        .fade_out = 0.0
    };

    switch (type) {
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

inst_type_e inst_type(const inst_s *inst) {
    return inst->type;
}

static int param_helper(const inst_s *inst, int index, void **addr, inst_param_info_s *info) {
    if (index < 0) return 1;

    if (index < BASE_PARAM_COUNT) {
        *info = base_param_info[index];
        *addr = (void*)(((uint8_t*)inst) + base_param_offsets[index]);
        return 0;
    }

    index -= BASE_PARAM_COUNT;

    switch (inst->type) {
        case INSTRUMENT_FM:
            if (index >= FM_PARAM_COUNT) return 1;
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

    double val_clamped = value;
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

int inst_get_param_int(const inst_s* inst, int index, int *value) {
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

int inst_get_param_double(const inst_s* inst, int index, double *value) {
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

void inst_run(inst_s* inst, const run_ctx_s *const run_ctx) {
    switch (inst->type) {
        case INSTRUMENT_FM:
            fm_run(inst, run_ctx);
            break;

        default:
            break;
    }
}

double calc_samples_per_tick(double bpm, int sample_rate) {
    const double beats_per_sec = bpm / 60.0;
    const double parts_per_sec = PARTS_PER_BEAT * beats_per_sec;
    const double ticks_per_sec = TICKS_PER_PART * parts_per_sec;
    return (double)sample_rate / ticks_per_sec;
}

double note_size_to_volume_mult(double size) {
    return pow(max(0.0, size) / NOTE_SIZE_MAX, 1.5);
}

double inst_volume_to_mult(double inst_volume) {
    if (inst_volume <= -25.0) return 0.0;
    return pow(2.0, VOLUME_LOG_SCALE * inst_volume);
}