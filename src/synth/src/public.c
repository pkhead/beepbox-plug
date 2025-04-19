#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../include/beepbox_synth.h"
#include "instrument.h"
#include "fm.h"
#include "util.h"
#include "envelope.h"

void bpbx_version(uint32_t *major, uint32_t *minor, uint32_t *revision) {
    *major = BPBX_VERSION_MAJOR;
    *minor = BPBX_VERSION_MINOR;
    *revision = BPBX_VERSION_REVISION;
}

static bpbx_inst_param_info_s base_param_info[BPBX_BASE_PARAM_COUNT] = {
    {
        .name = "Volume",
        .type = BPBX_PARAM_DOUBLE,
        .min_value = -25.0,
        .max_value = 25.0,
        .default_value = 0.0
    },
    {
        .name = "Fade In",
        .type = BPBX_PARAM_DOUBLE,
        .min_value = 0.0,
        .max_value = 9.0,
        .default_value = 0.0
    },
    {
        .name = "Fade Out",
        .type = BPBX_PARAM_DOUBLE,
        .min_value = FADE_OUT_MIN,
        .max_value = FADE_OUT_MAX,
        .default_value = 0.0
    }
};

size_t base_param_offsets[BPBX_BASE_PARAM_COUNT] = {
    offsetof(bpbx_inst_s, volume),
    offsetof(bpbx_inst_s, fade_in),
    offsetof(bpbx_inst_s, fade_out)
};

const unsigned int bpbx_param_count(bpbx_inst_type_e type) {
    switch (type) {
        case BPBX_INSTRUMENT_FM:
            return BPBX_BASE_PARAM_COUNT + BPBX_FM_PARAM_COUNT;

        default:
            return 0;
    }
}

const bpbx_inst_param_info_s* bpbx_param_info(bpbx_inst_type_e type, unsigned int index) {
    if (index < BPBX_BASE_PARAM_COUNT)
        return &base_param_info[index];

    switch (type) {
        case BPBX_INSTRUMENT_FM:
            return &fm_param_info[index - BPBX_BASE_PARAM_COUNT];

        default:
            return NULL;
    }
}

bpbx_inst_s* bpbx_inst_new(bpbx_inst_type_e type) {
    init_wavetables();

    bpbx_inst_s *inst = malloc(sizeof(bpbx_inst_s));
    *inst = (bpbx_inst_s) {
        .type = type,
        .sample_rate = 0.0,

        .volume = 0.0,
        .panning = 50.0,
        .fade_in = 0.0,
        .fade_out = 0.0
    };

    switch (type) {
        case BPBX_INSTRUMENT_FM:
            inst->fm = malloc(sizeof(fm_inst_s));
            fm_init(inst->fm);
            break;

        default:
            free(inst);
            return NULL;
    }

    return inst;
}

void bpbx_inst_destroy(bpbx_inst_s* inst) {
    free(inst->fm);
    free(inst);
}

void bpbx_inst_set_sample_rate(bpbx_inst_s *inst, double sample_rate) {
    inst->sample_rate = sample_rate;
}

bpbx_inst_type_e bpbx_inst_type(const bpbx_inst_s *inst) {
    return inst->type;
}

static int param_helper(const bpbx_inst_s *inst, int index, void **addr, bpbx_inst_param_info_s *info) {
    if (index < 0) return 1;

    if (index < BPBX_BASE_PARAM_COUNT) {
        *info = base_param_info[index];
        *addr = (void*)(((uint8_t*)inst) + base_param_offsets[index]);
        return 0;
    }

    index -= BPBX_BASE_PARAM_COUNT;

    switch (inst->type) {
        case BPBX_INSTRUMENT_FM:
            if (index >= BPBX_FM_PARAM_COUNT) return 1;
            *info = fm_param_info[index];
            *addr = (void*)(((uint8_t*)inst->fm) + fm_param_addresses[index]);
            break;

        default:
            return 1;
    }

    return 0;
}

int bpbx_inst_set_param_int(bpbx_inst_s* inst, int index, int value) {
    void *addr;
    bpbx_inst_param_info_s info;

    if (param_helper(inst, index, &addr, &info))
        return 1;

    int val_clamped = value;
    if (info.min_value != info.max_value) {
        val_clamped = (int)clampd((double)value, info.min_value, info.max_value);
    }
    
    switch (info.type) {
        case BPBX_PARAM_UINT8:
            *((uint8_t*)addr) = clampi(val_clamped, 0, UINT8_MAX);
            break;

        case BPBX_PARAM_INT:
            *((int*)addr) = clampi(val_clamped, INT_MIN, INT_MAX);
            break;

        case BPBX_PARAM_DOUBLE:
            *((double*)addr) = (double)val_clamped;
            break;
    }

    return 0;
}

int bpbx_inst_set_param_double(bpbx_inst_s* inst, int index, double value) {
    void *addr;
    bpbx_inst_param_info_s info;

    if (param_helper(inst, index, &addr, &info))
        return 1;

    double val_clamped = value;
    if (info.min_value != info.max_value) {
        val_clamped = clampd(value, info.min_value, info.max_value);
    }
    
    switch (info.type) {
        case BPBX_PARAM_UINT8:
        case BPBX_PARAM_INT:
            return 1;

        case BPBX_PARAM_DOUBLE:
            *((double*)addr) = val_clamped;
            break;
    }

    return 0;
}

int bpbx_inst_get_param_int(const bpbx_inst_s* inst, int index, int *value) {
    void *addr;
    bpbx_inst_param_info_s info;

    if (param_helper(inst, index, &addr, &info))
        return 1;

    switch (info.type) {
        case BPBX_PARAM_UINT8:
            *value = (int) *((uint8_t*)addr);
            break;

        case BPBX_PARAM_INT:
            *value = *((int*)addr);
            break;

        case BPBX_PARAM_DOUBLE:
            return 1;
    }

    return 0;
}

int bpbx_inst_get_param_double(const bpbx_inst_s* inst, int index, double *value) {
    void *addr;
    bpbx_inst_param_info_s info;

    if (param_helper(inst, index, &addr, &info))
        return 1;

    switch (info.type) {
        case BPBX_PARAM_UINT8:
            *value = (double) *((uint8_t*)addr);
            break;

        case BPBX_PARAM_INT:
            *value = (double) *((int*)addr);
            break;

        case BPBX_PARAM_DOUBLE:
            *value = *((double*)addr);
            break;
    }

    return 0;
}

uint8_t bpbx_inst_envelope_count(const bpbx_inst_s *inst) {
    return inst->envelope_count;
}

bpbx_envelope_s* bpbx_inst_get_envelope(bpbx_inst_s *inst, uint32_t index) {
    // if (index >= inst->envelope_count) return NULL; 
    return inst->envelopes + index;
}

bpbx_envelope_s* bpbx_inst_add_envelope(bpbx_inst_s *inst) {
    if (inst->envelope_count == BPBX_MAX_ENVELOPE_COUNT)
        return NULL;

    bpbx_envelope_s *new_env = &inst->envelopes[inst->envelope_count++];
    *new_env = (bpbx_envelope_s) {
        .index = BPBX_ENV_INDEX_NONE,
    };

    return new_env;
}

void bpbx_inst_remove_envelope(bpbx_inst_s *inst, uint8_t index) {
    if (index >= BPBX_MAX_ENVELOPE_COUNT) return;

    for (uint8_t i = index; i < BPBX_MAX_ENVELOPE_COUNT - 1; i++) {
        inst->envelopes[i] = inst->envelopes[i+1];
    }

    inst->envelope_count--;
}

void bpbx_inst_clear_envelopes(bpbx_inst_s *inst) {
    inst->envelope_count = 0;
}

void bpbx_inst_midi_on(bpbx_inst_s *inst, int key, int velocity) {
    switch (inst->type) {
        case BPBX_INSTRUMENT_FM:
            fm_midi_on(inst, key, velocity);
            break;

        default: break;
    }
}

void bpbx_inst_midi_off(bpbx_inst_s *inst, int key, int velocity) {
    switch (inst->type) {
        case BPBX_INSTRUMENT_FM:
            fm_midi_off(inst, key, velocity);
            break;

        default: break;
    }
}

void bpbx_inst_run(bpbx_inst_s* inst, const bpbx_run_ctx_s *const run_ctx) {
    switch (inst->type) {
        case BPBX_INSTRUMENT_FM:
            fm_run(inst, run_ctx);
            break;

        default:
            break;
    }
}

double bpbx_samples_fade_out(double setting, double bpm, double sample_rate) {
    const double samples_per_tick = calc_samples_per_tick(bpm, sample_rate);
    return ticks_fade_out(setting) * samples_per_tick;
}

const char *env_index_names[] = {
    "none",
    "note volume", // BPBX_ENV_INDEX_NOTE_VOLUME
    "n. filter freqs", // BPBX_ENV_INDEX_NOTE_FILTER_ALL_FREQS
    "pulse width", // BPBX_ENV_INDEX_PULSE_WIDTH
    "sustain", // BPBX_ENV_INDEX_STRING_SUSTAIN
    "unison", // BPBX_ENV_INDEX_UNISON
    "fm1 freq", // BPBX_ENV_INDEX_OPERATOR_FREQ0
    "fm2 freq", // BPBX_ENV_INDEX_OPERATOR_FREQ1
    "fm3 freq", // BPBX_ENV_INDEX_OPERATOR_FREQ2
    "fm4 freq", // BPBX_ENV_INDEX_OPERATOR_FREQ3
    "fm1 volume", // BPBX_ENV_INDEX_OPERATOR_AMP0
    "fm2 volume", // BPBX_ENV_INDEX_OPERATOR_AMP1
    "fm3 volume", // BPBX_ENV_INDEX_OPERATOR_AMP2
    "fm4 volume", // BPBX_ENV_INDEX_OPERATOR_AMP3
    "fm feedback", // BPBX_ENV_INDEX_FEEDBACK_AMP
    "pitch shift", // BPBX_ENV_INDEX_PITCH_SHIFT
    "detune", // BPBX_ENV_INDEX_DETUNE
    "vibrato range", // BPBX_ENV_INDEX_VIBRATO_DEPTH
    "n. filter 1 freq", // BPBX_ENV_INDEX_NOTE_FILTER_FREQ0
    "n. filter 2 freq", // BPBX_ENV_INDEX_NOTE_FILTER_FREQ1
    "n. filter 3 freq", // BPBX_ENV_INDEX_NOTE_FILTER_FREQ2
    "n. filter 4 freq", // BPBX_ENV_INDEX_NOTE_FILTER_FREQ3
    "n. filter 5 freq", // BPBX_ENV_INDEX_NOTE_FILTER_FREQ4
    "n. filter 6 freq", // BPBX_ENV_INDEX_NOTE_FILTER_FREQ5
    "n. filter 7 freq", // BPBX_ENV_INDEX_NOTE_FILTER_FREQ6
    "n. filter 8 freq", // BPBX_ENV_INDEX_NOTE_FILTER_FREQ7
    "n. filter 1 vol", // BPBX_ENV_INDEX_NOTE_FILTER_GAIN0
    "n. filter 2 vol", // BPBX_ENV_INDEX_NOTE_FILTER_GAIN1
    "n. filter 3 vol", // BPBX_ENV_INDEX_NOTE_FILTER_GAIN2
    "n. filter 4 vol", // BPBX_ENV_INDEX_NOTE_FILTER_GAIN3
    "n. filter 5 vol", // BPBX_ENV_INDEX_NOTE_FILTER_GAIN4
    "n. filter 6 vol", // BPBX_ENV_INDEX_NOTE_FILTER_GAIN5
    "n. filter 7 vol", // BPBX_ENV_INDEX_NOTE_FILTER_GAIN6
    "n. filter 8 vol", // BPBX_ENV_INDEX_NOTE_FILTER_GAIN7
    "dynamism", // BPBX_ENV_INDEX_SUPERSAW_DYNAMISM
    "spread", // BPBX_ENV_INDEX_SUPERSAW_SPREAD
    "saw↔pulse", // BPBX_ENV_INDEX_SUPERSAW_SHAPE
};

const char* bpbx_envelope_index_name(bpbx_envelope_compute_index_e index) {
    if (index < 0 || index >= sizeof(env_index_names)/sizeof(*env_index_names))
        return NULL;

    return env_index_names[index];
}


const char** bpbx_envelope_curve_preset_names() {
    static int need_init = 1;
    static const char* env_curve_names[BPBX_ENVELOPE_CURVE_PRESET_COUNT];

    if (need_init) {
        need_init = 0;
        for (int i = 0; i < BPBX_ENVELOPE_CURVE_PRESET_COUNT; i++) {
            env_curve_names[i] = envelope_curve_presets[i].name;
        }
    }

    return env_curve_names;
}

const bpbx_envelope_compute_index_e* bpbx_envelope_targets(bpbx_inst_type_e type, int *size) {
    switch (type) {
        case BPBX_INSTRUMENT_FM:
            *size = FM_MOD_COUNT;
            return fm_env_targets;

        default:
            return NULL;
    }
}