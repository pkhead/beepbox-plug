#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "fm.h"
#include "fm_algo.h"
#include "inst_info.h"
#include "util.h"

/*
algorithms:
0.  1 <- (2 3 4)
1.  1 <- (2 3 <- 4)
2.  1 <- 2 <- (3 4)
3.  1 <- (2 3) <- 4
4.  1 <- 2 <- 3 <- 4
5.  1 <- 3  2 <- 4
6.  1  2 <- (3 4)
7.  1  2 <- 3 <- 4
8.  (1 2) <- 3 <- 4
9.  (1 2) <- (3 4)
10. 1  2  3 <- 4
11. (1 2 3) <- 4
12. 1  2  3  4


feedback types:
0.  1 G
1.  2 G
2.  3 G
3.  4 G
4.  1 G  2 G
5.  3 G  4 G
6.  1 G  2 G  3 G
7.  2 G  3 G  4 G
8.  1 G  2 G  3 G  4 G
9.  1 -> 2
10. 1 -> 3
11. 1 -> 4
12. 2 -> 3
13. 2 -> 4
14. 3 -> 4
15. 1 -> 3  2 -> 4
16. 1 -> 4  2 -> 3
17. 1 -> 2 -> 3 -> 4
*/

// static float lu_sinf(float v) {
//     static float m = 1.f / PI2 * SIN_LOOKUP_LENGTH;

//     float indexf = fmodf(v, PI2) * m;
//     int index = (int)indexf;
//     float t = indexf - index;

//     return lerpf(sin_table[index], sin_table[index+1], t);
// }

static double operator_amplitude_curve(double amplitude) {
    return (pow(16.0, amplitude) - 1.0) / 15.0;
}

static void setup_algorithm(fm_inst_t *inst) {
    switch (inst->algorithm) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            inst->carrier_count = 1;
            break;

        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            inst->carrier_count = 2;
            break;

        case 10:
        case 11:
            inst->carrier_count = 3;
            break;

        case 12:
            inst->carrier_count = 4;
            break;

        default:
            inst->carrier_count = 0;
    }
}

static int algo_associated_carriers[][4] = {
    { 1, 1, 1, 1 },
    { 1, 1, 1, 1 },
    { 1, 1, 1, 1 },
    { 1, 1, 1, 1 },
    { 1, 1, 1, 1 },
    { 1, 2, 1, 2 },
    { 1, 2, 2, 2 },
    { 1, 2, 2, 2 },
    { 1, 2, 2, 2 },
    { 1, 2, 2, 2 },
    { 1, 2, 3, 3 },
    { 1, 2, 3, 3 },
    { 1, 2, 3, 4 }
};

void fm_init(fm_inst_t *inst) {    
    memset(inst, 0, sizeof(*inst));

    for (int i = 0; i < FM_MAX_VOICES; i++) {
        inst->voices[i].active = 0;
    }

    inst->algorithm = 12;

    inst->amplitudes[0] = 1;
    inst->amplitudes[1] = 0;
    inst->amplitudes[2] = 0;
    inst->amplitudes[3] = 0;

    inst->freq_ratios[0] = 1.f;
    inst->freq_ratios[1] = 1.f;
    inst->freq_ratios[2] = 1.f;
    inst->freq_ratios[3] = 1.f;
    
    inst->feedback = 0;
}

int fm_midi_on(fm_inst_t *inst, int key, int velocity) {
    float velocity_f = velocity / 127.f;

    int voice_index = 0;

    for (int i = 0; i < FM_MAX_VOICES; i++) {
        if (inst->voices[i].active) continue;
        voice_index = i;
        break;
    }

    fm_voice_t *voice = inst->voices + voice_index;
    *voice = (fm_voice_t) {
        .active = 1,
        .released = 0,
        .key = key,
        .volume = velocity_f,
        .last_sample = 0.f,
    };

    for (int op = 0; op < FM_OP_COUNT; op++) {
        voice->op_states[op] = (fm_voice_opstate_t) {
            .phase = 0.f,
            .phase_delta = 0.f,
            .expression = 0.f,
            .output = 0.f
        };
    }

    return voice_index;
}

void fm_midi_off(fm_inst_t *inst, int key, int velocity) {
    for (int i = 0; i < FM_MAX_VOICES; i++) {
        fm_voice_t *voice = &inst->voices[i];
        if (voice->active && !voice->released && voice->key == key) {
            voice->released = 1;
            break;
        }
    }
}

void fm_run(fm_inst_t *src_inst, float *out_samples, size_t frame_count, int sample_rate, int channel_count) {
    const float sample_len = 1.f / sample_rate;

    static double carrier_intervals[] = {0.0, 0.04, -0.073, 0.091};

    // create copy of instrument on stack, for cache optimization purposes
    fm_inst_t inst = *src_inst;

    setup_algorithm(&inst);
    fm_algo_func_t algo_func = fm_algorithm_table[inst.algorithm];

    // convert to operable values
    for (int i = 0; i < FM_MAX_VOICES; i++) {
        fm_voice_t *voice = inst.voices + i;
        if (!voice->active) continue;

        for (int op = 0; op < FM_OP_COUNT; op++) {
            int associated_carrier = algo_associated_carriers[inst.algorithm][op] - 1;
            double voice_freq = key_to_hz_d(voice->key + carrier_intervals[associated_carrier]);

            voice->op_states[op].phase = (fmod(voice->op_states[op].phase, 1.0) + 1000) * SINE_WAVE_LENGTH;
            voice->op_states[op].phase_delta = (voice_freq * inst.freq_ratios[op] * sample_len) * SINE_WAVE_LENGTH;
            voice->op_states[op].expression = operator_amplitude_curve((double) inst.amplitudes[op]);

            if (op >= inst.carrier_count) {
                voice->op_states[op].expression *= SINE_WAVE_LENGTH * 1.5f;
            }
            // TODO: beepbox seems to adjust volume of carrier based on its key pitch
        }
    }

    float feedback_amplitude = 0.3f * SINE_WAVE_LENGTH * ((float)inst.feedback / 15.f);

    const float modulator_expression_mult = SINE_WAVE_LENGTH * 1.5f;

    for (size_t frame = 0; frame < frame_count; frame++) {
        float final_sample = 0.f;

        for (int i = 0; i < FM_MAX_VOICES; i++) {
            fm_voice_t *voice = inst.voices + i;
            if (!voice->active) continue;

            // calculate operator 1
            float sample = algo_func(voice, feedback_amplitude);
            // voice->op_states[1].output = fm_calc_op(
            //     voice->op_states[1].phase,
            //     voice->op_states[1].expression
            // );

            // // calculate operator 0
            // voice->op_states[0].output = fm_calc_op(
            //     voice->op_states[0].phase + voice->op_states[1].output + feedback_amplitude * voice->op_states[0].output,
            //     voice->op_states[0].expression
            // );

            // float sample = voice->op_states[0].output;

            voice->op_states[0].phase += voice->op_states[0].phase_delta;
            voice->op_states[1].phase += voice->op_states[1].phase_delta;
            voice->op_states[2].phase += voice->op_states[2].phase_delta;
            voice->op_states[3].phase += voice->op_states[3].phase_delta;
            
            //float sample = inst.amplitudes[0] * lu_sinf(voice->phase[0] + inst.amplitudes[1] * lu_sinf(voice->phase[1]));

            // when released, end voice on zero crossing
            if (voice->released && signf(sample) != signf(voice->last_sample)) {
                voice->active = 0;
            } else {
                final_sample += sample * voice->volume;
                voice->last_sample = sample;
            }
        }

        for (int c = 0; c < channel_count; c++)
            *out_samples++ = final_sample;
    }

    // convert from operable values
    for (int i = 0; i < FM_MAX_VOICES; i++) {
        fm_voice_t *voice = inst.voices + i;
        if (!voice->active) continue;

        for (int op = 0; op < FM_OP_COUNT; op++) {
            voice->op_states[op].phase /= SINE_WAVE_LENGTH;
            voice->op_states[op].phase_delta /= SINE_WAVE_LENGTH;
        }
    }

    *src_inst = inst;
}

inst_param_info_t fm_param_info[FM_PARAM_COUNT] = {
    {
        .type = PARAM_UINT8,
        .name = "Algorithm",
        .min_value = 0,
        .max_value = 12,
        .default_value = 0
    },

    {
        .type = PARAM_FLOAT,
        .name = "Operator 1 Frequency",
        .min_value = 1,
        .max_value = 20,
        .default_value = 1
    },
    {
        .type = PARAM_FLOAT,
        .name = "Operator 1 Volume",
        .min_value = 0,
        .max_value = 1,
        .default_value = 1
    },

    {
        .type = PARAM_FLOAT,
        .name = "Operator 2 Frequency",
        .min_value = 1,
        .max_value = 20,
        .default_value = 1
    },
    {
        .type = PARAM_FLOAT,
        .name = "Operator 2 Volume",
        .min_value = 0,
        .max_value = 1,
        .default_value = 0
    },

    {
        .type = PARAM_FLOAT,
        .name = "Operator 3 Frequency",
        .min_value = 1,
        .max_value = 20,
        .default_value = 1
    },
    {
        .type = PARAM_FLOAT,
        .name = "Operator 3 Volume",
        .min_value = 0,
        .max_value = 1,
        .default_value = 0
    },

    {
        .type = PARAM_FLOAT,
        .name = "Operator 4 Frequency",
        .min_value = 1,
        .max_value = 20,
        .default_value = 1
    },
    {
        .type = PARAM_FLOAT,
        .name = "Operator 4 Volume",
        .min_value = 0,
        .max_value = 1,
        .default_value = 0
    },

    {
        .type = PARAM_UINT8,
        .name = "Feedback Type",
        .min_value = 0,
        .max_value = 17,
        .default_value = 0
    },

    {
        .type = PARAM_FLOAT,
        .name = "Feedback Volume",
        .min_value = 0,
        .max_value = 1,
        .default_value = 0
    }
};

size_t fm_param_addresses[FM_PARAM_COUNT] = {
    offsetof(fm_inst_t, algorithm),
    offsetof(fm_inst_t, freq_ratios[0]),
    offsetof(fm_inst_t, amplitudes[0]),
    offsetof(fm_inst_t, freq_ratios[1]),
    offsetof(fm_inst_t, amplitudes[1]),
    offsetof(fm_inst_t, freq_ratios[2]),
    offsetof(fm_inst_t, amplitudes[2]),
    offsetof(fm_inst_t, freq_ratios[3]),
    offsetof(fm_inst_t, amplitudes[3]),
    offsetof(fm_inst_t, feedback_type),
    offsetof(fm_inst_t, feedback),
};