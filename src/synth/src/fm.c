#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "fm.h"
#include "fm_algo.h"
#include "util.h"
#include "wavetables.h"
#include "instrument.h"
#include "envelope.h"

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

typedef struct {
    double mult;
    double hz_offset;
    double amplitude_sign;
} fm_freq_data_s;

static fm_freq_data_s frequency_data[FM_FREQ_COUNT];
static int algo_associated_carriers[FM_ALGORITHM_COUNT][4];
static double carrier_intervals[FM_OP_COUNT];

#define EXPRESSION_REFERENCE_PITCH 16 // A low "E" as a MIDI pitch.
#define PITCH_DAMPING 48
#define VOICE_BASE_EXPRESSION 0.1 // 0.03 (original value, but i felt like it was too quiet)

static double operator_amplitude_curve(double amplitude) {
    return (pow(16.0, amplitude / 15.0) - 1.0) / 15.0;
}

static void setup_algorithm(fm_inst_s *inst) {
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

void fm_init(fm_inst_s *inst) {    
    memset(inst, 0, sizeof(*inst));

    for (int i = 0; i < INST_MAX_VOICES; i++) {
        inst->voices[i].active = FALSE;
    }

    inst->algorithm = 0;

    inst->amplitudes[0] = 15.0;
    inst->amplitudes[1] = 0.0;
    inst->amplitudes[2] = 0.0;
    inst->amplitudes[3] = 0.0;

    inst->freq_ratios[0] = 4;
    inst->freq_ratios[1] = 4;
    inst->freq_ratios[2] = 4;
    inst->freq_ratios[3] = 4;
    
    inst->feedback = 0;
}

int fm_midi_on(inst_s *inst, int key, int velocity) {
    fm_inst_s *const fm = inst->fm;

    float velocity_f = velocity / 127.f;

    int voice_index = 0;

    for (int i = 0; i < INST_MAX_VOICES; i++) {
        if (fm->voices[i].active) continue;
        voice_index = i;
        break;
    }

    fm_voice_s *voice = fm->voices + voice_index;
    *voice = (fm_voice_s) {
        .active = TRUE,
        .released = FALSE,
        .key = key < 0 ? 0 : (uint16_t)key,
        .volume = velocity_f,
    };

    for (int op = 0; op < FM_OP_COUNT; op++) {
        voice->op_states[op] = (fm_voice_opstate_s) {
            .phase = 0.0,
            .phase_delta = 0.0,
            .expression = 0.0,
            .output = 0.0,
            .prev_pitch_expression = 0.0,
            .has_prev_pitch_expression = FALSE
        };
    }

    return voice_index;
}

void fm_midi_off(inst_s *inst, int key, int velocity) {
    fm_inst_s *const fm = inst->fm;
    
    for (int i = 0; i < INST_MAX_VOICES; i++) {
        fm_voice_s *voice = &fm->voices[i];
        if (voice->active && !voice->released && voice->key == key) {
            voice->released = 1;
            break;
        }
    }
}

typedef struct {
    double fade_in;
    double fade_out;
    double samples_per_tick;
    double sample_rate;
    double cur_beat;

    uint8_t envelope_count;
    envelope_s *envelopes;
} tone_compute_s;

static void compute_voice(const fm_inst_s *const inst, fm_voice_s *const voice, tone_compute_s compute_data) {
    const double sample_len = 1.f / compute_data.sample_rate;
    const double samples_per_tick = compute_data.samples_per_tick;
    const double rounded_samples_per_tick = ceil(samples_per_tick);

    voice->time_ticks = voice->time2_ticks;
    voice->time2_ticks = voice->time_ticks + 1.0;
    voice->time_secs = voice->time2_secs;
    voice->time2_secs = voice->time_secs + samples_per_tick / compute_data.sample_rate;

    compute_envelopes(
        &voice->env_computer,
        compute_data.envelopes, compute_data.envelope_count,
        compute_data.cur_beat, voice->time_secs, samples_per_tick * sample_len
    );

    const double fade_in_secs = secs_fade_in(compute_data.fade_in);

    // precalculation/volume balancing/etc
    double sine_expr_boost = 1.0;
    double total_carrier_expr = 0.0;
    double fade_expr_start = 1.0;
    double fade_expr_end = 1.0;

    const uint8_t released = voice->time_secs >= fade_in_secs && voice->released;
    if (released) {
        const double ticks = fabs(ticks_fade_out(compute_data.fade_out));
        fade_expr_start = note_size_to_volume_mult((1.0 - voice->ticks_since_release / ticks) * NOTE_SIZE_MAX);
        fade_expr_end = note_size_to_volume_mult((1.0 - (voice->ticks_since_release + 1.0) / ticks) * NOTE_SIZE_MAX);

        if (voice->ticks_since_release >= ticks) {
            voice->active = FALSE;
        }
    } else {
        // fade in beginning of note
        if (fade_in_secs > 0) {
            fade_expr_start *= min(1.0, voice->time_secs / fade_in_secs);
            fade_expr_end *= min(1.0, voice->time2_secs / fade_in_secs);
        }
    }

    for (int op = 0; op < FM_OP_COUNT; op++) {
        // john nesky: I'm adding 1000 to the phase to ensure that it's never negative even when modulated
        // by other waves because negative numbers don't work with the modulus operator very well.
        voice->op_states[op].phase = (fmod(voice->op_states[op].phase, 1.0) + 1000);

        fm_freq_data_s *freq_data = &frequency_data[inst->freq_ratios[op]];

        int associated_carrier_idx = algo_associated_carriers[inst->algorithm][op] - 1;
        const double freq_mult = freq_data->mult;
        const double pitch = (double)voice->key + carrier_intervals[associated_carrier_idx];
        const double base_freq = key_to_hz_d(pitch);
        const double hz_offset = freq_data->hz_offset;
        const double target_freq = freq_mult * base_freq + hz_offset;

        const double freq_env_start = voice->env_computer.envelope_starts[ENV_INDEX_OPERATOR_FREQ0 + op];
        const double freq_env_end = voice->env_computer.envelope_ends[ENV_INDEX_OPERATOR_FREQ0 + op];
        double freq_start, freq_end;
        if (freq_env_start != 1.0 || freq_env_end != 1.0) {
            freq_start = pow(2.0, log2(target_freq / base_freq) * freq_env_start) * base_freq;
            freq_end = pow(2.0, log2(target_freq / base_freq) * freq_env_end) * base_freq;
        } else {
            freq_start = target_freq;
            freq_end = target_freq;
        }

        voice->op_states[op].phase_delta = freq_start * sample_len;
        voice->op_states[op].phase_delta_scale = pow(freq_end / freq_start, 1.0 / rounded_samples_per_tick);

        const double amplitude_curve = operator_amplitude_curve((double) inst->amplitudes[op]);
        const double amplitude_mult = amplitude_curve * freq_data->amplitude_sign;
        double expression = amplitude_mult;

        if (op < inst->carrier_count) {
            // carrier
            double pitch_expression;
            if (voice->op_states[op].has_prev_pitch_expression) {
                pitch_expression = voice->op_states[op].prev_pitch_expression;
            } else {
                pitch_expression = pow(2.0, -(pitch - EXPRESSION_REFERENCE_PITCH) / PITCH_DAMPING);
            }
            voice->op_states[op].has_prev_pitch_expression = TRUE;
            voice->op_states[op].prev_pitch_expression = pitch_expression;

            expression *= pitch_expression;
            total_carrier_expr += amplitude_curve;
        } else {
            // modulator
            expression *= SINE_WAVE_LENGTH * 1.5;
            sine_expr_boost *= 1.0 - min(1.0, inst->amplitudes[op] / 15.0);
        }

        double expression_start = expression * voice->env_computer.envelope_starts[ENV_INDEX_OPERATOR_AMP0 + op];
        double expression_end = expression * voice->env_computer.envelope_ends[ENV_INDEX_OPERATOR_AMP0 + op];

        voice->op_states[op].expression = expression_start;
        voice->op_states[op].expression_delta = (expression_end - expression_start) / rounded_samples_per_tick;
    }

    sine_expr_boost *= (pow(2.0, (2.0 - 1.4 * inst->feedback / 15.0)) - 1.0) / 3.0;
    sine_expr_boost *= 1.0 - min(1.0, max(0.0, total_carrier_expr - 1) / 2.0);
    sine_expr_boost = 1.0 + sine_expr_boost * 3.0;

    const double expr_start = VOICE_BASE_EXPRESSION * sine_expr_boost * fade_expr_start * voice->env_computer.envelope_starts[ENV_INDEX_NOTE_VOLUME];
    const double expr_end = VOICE_BASE_EXPRESSION * sine_expr_boost * fade_expr_end * voice->env_computer.envelope_ends[ENV_INDEX_NOTE_VOLUME];
    voice->expression = expr_start;
    voice->expression_delta = (expr_end - expr_start) / rounded_samples_per_tick;

    const double feedback_amplitude = SINE_WAVE_LENGTH * 0.3 * inst->feedback / 15.0;
    const double feedback_start = feedback_amplitude * voice->env_computer.envelope_starts[ENV_INDEX_FEEDBACK_AMP];
    const double feedback_end = feedback_amplitude * voice->env_computer.envelope_ends[ENV_INDEX_FEEDBACK_AMP];
    voice->feedback_mult = feedback_start;
    voice->feedback_delta = (feedback_end - feedback_start) / rounded_samples_per_tick;

    if (released) {
        voice->ticks_since_release += 1.0;
        voice->secs_since_release += samples_per_tick;
    }
}

void fm_run(inst_s *src_inst, const run_ctx_s *const run_ctx) {
    const double sample_rate = src_inst->sample_rate;
    const size_t frame_count = run_ctx->frame_count;
    float *const out_samples = run_ctx->out_samples;
    const double beat = run_ctx->beat;

    // create copy of instrument on stack, for cache optimization purposes
    fm_inst_s inst = *src_inst->fm;
    setup_algorithm(&inst);

    fm_algo_f algo_func = fm_algorithm_table[inst.algorithm * FM_FEEDBACK_TYPE_COUNT + inst.feedback_type];
    const double samples_per_tick = calc_samples_per_tick(run_ctx->bpm, sample_rate);
    const double fade_in = src_inst->fade_in;
    const double fade_out = src_inst->fade_out;

    // zero-initialize sample data
    memset(out_samples, 0, frame_count * 2 * sizeof(float));
    
    double inst_volume = inst_volume_to_mult(src_inst->volume);

    for (int i = 0; i < INST_MAX_VOICES; i++) {
        fm_voice_s *voice = inst.voices + i;
        if (!voice->active) continue;

        size_t buffer_idx = 0;
        size_t frame = 0;

        while (frame < frame_count) {
            if (voice->remaining_samples == 0) {
                voice->remaining_samples += (size_t)ceil(samples_per_tick);

                compute_voice(&inst, voice, (tone_compute_s) {
                    .samples_per_tick = samples_per_tick,
                    .sample_rate = sample_rate,
                    .fade_in = fade_in,
                    .fade_out = fade_out,
                    .envelope_count = src_inst->envelope_count,
                    .envelopes = src_inst->envelopes,
                    .cur_beat = beat
                });
            }

            // convert to operable values
            for (int op = 0; op < FM_OP_COUNT; op++) {
                voice->op_states[op].phase *= SINE_WAVE_LENGTH;
                voice->op_states[op].phase_delta *= SINE_WAVE_LENGTH;
            }
            
            size_t end_frame = frame + voice->remaining_samples;
            if (end_frame > frame_count) end_frame = frame_count;
            //assert((int64_t)end_frame - frame >= 0);
            size_t run_length = end_frame - frame;

            for (; frame < end_frame; frame++) {      
                float sample = (float) (algo_func(voice, voice->feedback_mult) * voice->expression * inst_volume) * voice->volume;

                voice->op_states[0].phase += voice->op_states[0].phase_delta;
                voice->op_states[1].phase += voice->op_states[1].phase_delta;
                voice->op_states[2].phase += voice->op_states[2].phase_delta;
                voice->op_states[3].phase += voice->op_states[3].phase_delta;

                voice->op_states[0].phase_delta *= voice->op_states[0].phase_delta_scale;
                voice->op_states[1].phase_delta *= voice->op_states[1].phase_delta_scale;
                voice->op_states[2].phase_delta *= voice->op_states[2].phase_delta_scale;
                voice->op_states[3].phase_delta *= voice->op_states[3].phase_delta_scale;

                voice->op_states[0].expression += voice->op_states[0].expression_delta;
                voice->op_states[1].expression += voice->op_states[1].expression_delta;
                voice->op_states[2].expression += voice->op_states[2].expression_delta;
                voice->op_states[3].expression += voice->op_states[3].expression_delta;
                
                voice->expression += voice->expression_delta;
                voice->feedback_mult += voice->feedback_delta;

                // assume two channels
                out_samples[buffer_idx++] += sample;
                out_samples[buffer_idx++] += sample;
            }

            voice->remaining_samples -= run_length;
            
            // convert from operable values
            for (int op = 0; op < FM_OP_COUNT; op++) {
                voice->op_states[op].phase /= SINE_WAVE_LENGTH;
                voice->op_states[op].phase_delta /= SINE_WAVE_LENGTH;
            }
        }

    }

    *src_inst->fm = inst;
}








//////////////////
//     DATA     //
//////////////////

static const char *algo_enum_values[] = {
    "1←(2 3 4)",
    "1←(2 3←4)",
    "1←2←(3 4)",
    "1←(2 3)←4",
    "1←2←3←4",
    "1←3 2←4",
    "1 2←(3 4)",
    "1 2←3←4",
    "(1 2)←3←4",
    "(1 2)←(3 4)",
    "1 2 3←4",
    "(1 2 3)←4",
    "1 2 3 4",
};

static const char *freq_enum_values[] = {
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
    "128×",
    "256×",
};

static const char *feedback_enum_values[] = {
    "1⟲",
    "2⟲",
    "3⟲",
    "4⟲",
    "1⟲ 2⟲",
    "3⟲ 4⟲",
    "1⟲ 2⟲ 3⟲",
    "2⟲ 3⟲ 4⟲",
    "1⟲ 2⟲ 3⟲ 4⟲",
    "1→2",
    "1→3",
    "1→4",
    "2→3",
    "2→4",
    "3→4",
    "1→3 2→4",
    "1→4 2→3",
    "1→2→3→4",
};

const inst_param_info_s fm_param_info[FM_PARAM_COUNT] = {
    {
        .type = PARAM_UINT8,
        .flags = PARAM_FLAG_NO_AUTOMATION,

        .name = "Algorithm",
        .min_value = 0,
        .max_value = 12,
        .default_value = 0,

        .enum_values = algo_enum_values
    },

    {
        .type = PARAM_UINT8,
        .flags = PARAM_FLAG_NO_AUTOMATION,
        .name = "Operator 1 Frequency",
        .envelope_name = "fm1 freq",

        .min_value = 0,
        .max_value = FM_FREQ_COUNT-1,
        .default_value = 4,

        .enum_values = freq_enum_values
    },
    {
        .type = PARAM_DOUBLE,
        .name = "Operator 1 Volume",
        .envelope_name = "fm1 volume",

        .min_value = 0,
        .max_value = 15,
        .default_value = 15
    },

    {
        .type = PARAM_UINT8,
        .flags = PARAM_FLAG_NO_AUTOMATION,
        .name = "Operator 2 Frequency",
        .envelope_name = "fm2 freq",
        
        .min_value = 0,
        .max_value = FM_FREQ_COUNT-1,
        .default_value = 4,

        .enum_values = freq_enum_values
    },
    {
        .type = PARAM_DOUBLE,
        .name = "Operator 2 Volume",
        .envelope_name = "fm2 volume",

        .min_value = 0,
        .max_value = 15,
        .default_value = 0
    },

    {
        .type = PARAM_UINT8,
        .flags = PARAM_FLAG_NO_AUTOMATION,
        .name = "Operator 3 Frequency",
        .envelope_name = "fm3 freq",

        .min_value = 0,
        .max_value = FM_FREQ_COUNT-1,
        .default_value = 4,
        
        .enum_values = freq_enum_values
    },
    {
        .type = PARAM_DOUBLE,
        .name = "Operator 3 Volume",
        .envelope_name = "fm3 volume",

        .min_value = 0,
        .max_value = 15,
        .default_value = 0
    },

    {
        .type = PARAM_UINT8,
        .flags = PARAM_FLAG_NO_AUTOMATION,
        .name = "Operator 4 Frequency",
        .envelope_name = "fm4 freq",

        .min_value = 0,
        .max_value = FM_FREQ_COUNT-1,
        .default_value = 4,
        
        .enum_values = freq_enum_values
    },
    {
        .type = PARAM_DOUBLE,
        .name = "Operator 4 Volume",
        .envelope_name = "fm4 volume",

        .min_value = 0,
        .max_value = 15,
        .default_value = 0
    },

    {
        .type = PARAM_UINT8,
        .flags = PARAM_FLAG_NO_AUTOMATION,

        .name = "Feedback Type",
        .min_value = 0,
        .max_value = FM_FEEDBACK_TYPE_COUNT-1,
        .default_value = 0,

        .enum_values = feedback_enum_values
    },

    {
        .type = PARAM_DOUBLE,
        .name = "Feedback Volume",
        .envelope_name = "fm feedback",

        .min_value = 0,
        .max_value = 15,
        .default_value = 0
    }
};

const envelope_compute_index_e fm_env_targets[FM_MOD_COUNT] = {
    ENV_INDEX_OPERATOR_FREQ0,
    ENV_INDEX_OPERATOR_AMP0,
    ENV_INDEX_OPERATOR_FREQ1,
    ENV_INDEX_OPERATOR_AMP1,
    ENV_INDEX_OPERATOR_FREQ2,
    ENV_INDEX_OPERATOR_AMP2,
    ENV_INDEX_OPERATOR_FREQ3,
    ENV_INDEX_OPERATOR_AMP3,
    ENV_INDEX_FEEDBACK_AMP
};

const size_t fm_param_addresses[FM_PARAM_COUNT] = {
    offsetof(fm_inst_s, algorithm),
    offsetof(fm_inst_s, freq_ratios[0]),
    offsetof(fm_inst_s, amplitudes[0]),
    offsetof(fm_inst_s, freq_ratios[1]),
    offsetof(fm_inst_s, amplitudes[1]),
    offsetof(fm_inst_s, freq_ratios[2]),
    offsetof(fm_inst_s, amplitudes[2]),
    offsetof(fm_inst_s, freq_ratios[3]),
    offsetof(fm_inst_s, amplitudes[3]),
    offsetof(fm_inst_s, feedback_type),
    offsetof(fm_inst_s, feedback),
};

static fm_freq_data_s frequency_data[FM_FREQ_COUNT] = {
    { .mult = 0.125,    .hz_offset= 0.0,      .amplitude_sign = 1.0 },
    { .mult= 0.25,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 0.5,       .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 0.75,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 1.0,       .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 1.0,       .hz_offset= 1.5,      .amplitude_sign= -1.0 },
    { .mult= 2.0,       .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 2.0,       .hz_offset= -1.3,     .amplitude_sign= -1.0 },
    { .mult= 3.0,       .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 3.5,       .hz_offset= -0.05,    .amplitude_sign= 1.0 },
    { .mult= 4.0,       .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 4.0,       .hz_offset= -2.4,     .amplitude_sign= -1.0 },
    { .mult= 5.0,       .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 6.0,       .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 7.0,       .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 8.0,       .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 9.0,       .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 10.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 11.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 12.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 13.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 14.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 15.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    //ultrabox
    { .mult= 16.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 17.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    //ultrabox
    { .mult= 18.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 19.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    //ultrabox
    { .mult= 20.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 20.0,      .hz_offset= -5.0,     .amplitude_sign= -1.0 },
    // dogebox (maybe another mod also adds this? I got it from dogebox)
    { .mult= 25.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 50.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 75.0,      .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 100.0,     .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    //50 and 100 are from dogebox
    //128 and 256 from slarmoo's box
    { .mult= 128.0,     .hz_offset= 0.0,      .amplitude_sign= 1.0 },
    { .mult= 250.0,     .hz_offset= 0.0,      .amplitude_sign= 1.0},
};

static int algo_associated_carriers[FM_ALGORITHM_COUNT][4] = {
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

static double carrier_intervals[] = {0.0, 0.04, -0.073, 0.091};