#ifndef _fm_h_
#define _fm_h_

#include <stdint.h>
#include <stddef.h>
#include "../include/beepbox_synth.h"
#include "../include/beepbox_instrument_data.h"
#include "wavetables.h"

#define FM_OP_COUNT 4
#define FM_MOD_COUNT 9

typedef struct {
    double phase;
    double phase_delta;
    double expression;
    double prev_pitch_expression;
    double output;

    uint8_t has_prev_pitch_expression;
} fm_voice_opstate_s;

typedef struct {
    uint8_t active;
    uint8_t released;
    uint16_t key;
    
    float volume;
    size_t remaining_samples;
    
    double expression;
    double expression_delta;

    // how long the voice has been active in ticks
    // time2_secs is seconds to end of current run.
    double time_secs;
    double time2_secs;

    // how long the voice has been active in ticks
    // time2_secs is ticks to the end of the current run.
    double time_ticks;
    double time2_ticks;

    double secs_since_release;
    double ticks_since_release;

    fm_voice_opstate_s op_states[FM_OP_COUNT];
} fm_voice_s;

typedef struct {
    uint8_t algorithm;
    uint8_t freq_ratios[FM_OP_COUNT];
    double amplitudes[FM_OP_COUNT];

    uint8_t feedback_type;
    double feedback;

    int carrier_count;
    fm_voice_s voices[INST_MAX_VOICES];
} fm_inst_s;

static inline double fm_calc_op(const double phase_mix) {
    const int phase_int = (int) phase_mix;
    const int index = phase_int & (SINE_WAVE_LENGTH - 1);
    const double sample = sine_wave_d[index];
    return sample + (sine_wave_d[index+1] - sample) * (phase_mix - phase_int);
}

void fm_init(fm_inst_s *inst);
int fm_midi_on(inst_s *inst, int key, int velocity);
void fm_midi_off(inst_s *inst, int key, int velocity);
void fm_run(inst_s *src_inst, const run_ctx_s *const run_ctx);

extern const inst_param_info_s fm_param_info[FM_PARAM_COUNT];
extern const envelope_compute_index_e fm_env_targets[FM_MOD_COUNT];
extern const size_t fm_param_addresses[FM_PARAM_COUNT];

#endif