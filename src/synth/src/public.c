#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../include/beepbox_synth.h"
#include "instrument.h"
#include "fm.h"
#include "util.h"
#include "envelope.h"

void beepbox_version(uint32_t *major, uint32_t *minor, uint32_t *revision) {
    *major = BEEPBOX_VERSION_MAJOR;
    *minor = BEEPBOX_VERSION_MINOR;
    *revision = BEEPBOX_VERSION_REVISION;
}

static inst_param_info_s base_param_info[BASE_PARAM_COUNT] = {
    {
        .name = "Volume",
        .type = PARAM_DOUBLE,
        .min_value = -25.0,
        .max_value = 25.0,
        .default_value = 0.0
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
        .min_value = FADE_OUT_MIN,
        .max_value = FADE_OUT_MAX,
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
        .sample_rate = 0.0,

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

void inst_set_sample_rate(inst_s *inst, double sample_rate) {
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

uint8_t inst_envelope_count(const inst_s *inst) {
    return inst->envelope_count;
}

envelope_s* inst_get_envelope(inst_s *inst, uint32_t index) {
    // if (index >= inst->envelope_count) return NULL; 
    return inst->envelopes + index;
}

envelope_s* inst_add_envelope(inst_s *inst) {
    if (inst->envelope_count == MAX_ENVELOPE_COUNT)
        return NULL;

    envelope_s *new_env = &inst->envelopes[inst->envelope_count++];
    *new_env = (envelope_s) {
        .index = ENV_INDEX_NONE,
    };

    return new_env;
}

void inst_remove_envelope(inst_s *inst, uint8_t index) {
    if (index >= MAX_ENVELOPE_COUNT) return;

    for (uint8_t i = index; i < MAX_ENVELOPE_COUNT - 1; i++) {
        inst->envelopes[i] = inst->envelopes[i+1];
    }

    inst->envelope_count--;
}

void inst_clear_envelopes(inst_s *inst) {
    inst->envelope_count = 0;
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

double inst_samples_fade_out(double setting, double bpm, double sample_rate) {
    const double samples_per_tick = calc_samples_per_tick(bpm, sample_rate);
    return ticks_fade_out(setting) * samples_per_tick;
}

const char *env_index_names[] = {
    "none",
    "note volume", // ENV_INDEX_NOTE_VOLUME
    "n. filter freqs", // ENV_INDEX_NOTE_FILTER_ALL_FREQS
    "pulse width", // ENV_INDEX_PULSE_WIDTH
    "sustain", // ENV_INDEX_STRING_SUSTAIN
    "unison", // ENV_INDEX_UNISON
    "fm1 freq", // ENV_INDEX_OPERATOR_FREQ0
    "fm2 freq", // ENV_INDEX_OPERATOR_FREQ1
    "fm3 freq", // ENV_INDEX_OPERATOR_FREQ2
    "fm4 freq", // ENV_INDEX_OPERATOR_FREQ3
    "fm1 volume", // ENV_INDEX_OPERATOR_AMP0
    "fm2 volume", // ENV_INDEX_OPERATOR_AMP1
    "fm3 volume", // ENV_INDEX_OPERATOR_AMP2
    "fm4 volume", // ENV_INDEX_OPERATOR_AMP3
    "fm feedback", // ENV_INDEX_FEEDBACK_AMP
    "pitch shift", // ENV_INDEX_PITCH_SHIFT
    "detune", // ENV_INDEX_DETUNE
    "vibrato range", // ENV_INDEX_VIBRATO_DEPTH
    "n. filter 1 freq", // ENV_INDEX_NOTE_FILTER_FREQ0
    "n. filter 2 freq", // ENV_INDEX_NOTE_FILTER_FREQ1
    "n. filter 3 freq", // ENV_INDEX_NOTE_FILTER_FREQ2
    "n. filter 4 freq", // ENV_INDEX_NOTE_FILTER_FREQ3
    "n. filter 5 freq", // ENV_INDEX_NOTE_FILTER_FREQ4
    "n. filter 6 freq", // ENV_INDEX_NOTE_FILTER_FREQ5
    "n. filter 7 freq", // ENV_INDEX_NOTE_FILTER_FREQ6
    "n. filter 8 freq", // ENV_INDEX_NOTE_FILTER_FREQ7
    "n. filter 1 vol", // ENV_INDEX_NOTE_FILTER_GAIN0
    "n. filter 2 vol", // ENV_INDEX_NOTE_FILTER_GAIN1
    "n. filter 3 vol", // ENV_INDEX_NOTE_FILTER_GAIN2
    "n. filter 4 vol", // ENV_INDEX_NOTE_FILTER_GAIN3
    "n. filter 5 vol", // ENV_INDEX_NOTE_FILTER_GAIN4
    "n. filter 6 vol", // ENV_INDEX_NOTE_FILTER_GAIN5
    "n. filter 7 vol", // ENV_INDEX_NOTE_FILTER_GAIN6
    "n. filter 8 vol", // ENV_INDEX_NOTE_FILTER_GAIN7
    "dynamism", // ENV_INDEX_SUPERSAW_DYNAMISM
    "spread", // ENV_INDEX_SUPERSAW_SPREAD
    "saw↔pulse", // ENV_INDEX_SUPERSAW_SHAPE
};

const char* envelope_index_name(envelope_compute_index_e index) {
    if (index < 0 || index >= sizeof(env_index_names)/sizeof(*env_index_names))
        return NULL;

    return env_index_names[index];
}


const char** envelope_curve_preset_names() {
    static int need_init = 1;
    static const char* env_curve_names[ENVELOPE_CURVE_PRESET_COUNT];

    if (need_init) {
        need_init = 0;
        for (int i = 0; i < ENVELOPE_CURVE_PRESET_COUNT; i++) {
            env_curve_names[i] = envelope_curve_presets[i].name;
        }
    }

    return env_curve_names;
}

const envelope_compute_index_e* inst_envelope_targets(inst_type_e type, int *size) {
    switch (type) {
        case INSTRUMENT_FM:
            *size = FM_MOD_COUNT;
            return fm_env_targets;

        default:
            return NULL;
    }
}