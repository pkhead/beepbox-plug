#ifndef _fm_h_
#define _fm_h_

#include <stdint.h>
#include <stddef.h>
#include "inst_info.h"
#include "util.h"

#define FM_MAX_VOICES 8
#define FM_OP_COUNT 4

typedef struct {
    double phase;
    double phase_delta;
    double expression;
    double output;
} fm_voice_opstate_t;

typedef struct {
    uint8_t active;
    uint8_t released;

    int key;
    float volume;
    
    fm_voice_opstate_t op_states[FM_OP_COUNT];

    float last_sample;
} fm_voice_t;

typedef struct {
    uint8_t algorithm;
    float freq_ratios[FM_OP_COUNT];
    float amplitudes[FM_OP_COUNT];

    uint8_t feedback_type;
    uint8_t feedback;

    int carrier_count;
    fm_voice_t voices[FM_MAX_VOICES];
} fm_inst_t;

static inline double fm_calc_op(const double phase_mix, const double expression) {
    const int phase_int = (int) phase_mix;
    const int index = phase_int & (SINE_WAVE_LENGTH - 1);
    const double sample = sine_wave_d[index];
    return expression * (sample + (sine_wave_d[index+1] - sample) * (phase_mix - phase_int));
}

void fm_init(fm_inst_t *inst);
int fm_midi_on(fm_inst_t *inst, int key, int velocity);
void fm_midi_off(fm_inst_t *inst, int key, int velocity);
void fm_run(fm_inst_t *src_inst, float *out_samples, size_t frame_count, int sample_rate, int channel_count);

#define FM_PARAM_COUNT 11
extern inst_param_info_t fm_param_info[FM_PARAM_COUNT];
extern size_t fm_param_addresses[FM_PARAM_COUNT];

#endif