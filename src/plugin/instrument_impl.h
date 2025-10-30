#ifndef _instrument_impl_h_
#define _instrument_impl_h_

#include "include/instrument.h"
#include <stdint.h>
#include "atomic_bool.h"

typedef struct {
   bool active;

   int32_t note_id;
   int16_t port_index;
   int16_t channel;
   int16_t key;
} voice_s;

typedef struct instrument {
    bpbxsyn_synth_type_e type;
    uint8_t type_index;
    uint8_t new_type_index;

    bpbxsyn_synth_s *synth;

    bool use_distortion;
    bool use_bitcrusher;
    bool use_chorus;
    bool use_echo;
    bool use_reverb;

    // when effects are added, they must wait until the next tick to begin
    // processing. these bools determine whether or not said effect is fully
    // active or not.
    bool run_distortion;
    bool run_bitcrusher;
    bool run_chorus;
    bool run_echo;
    bool run_reverb;

    union {
        bpbxsyn_effect_s *effect_modules[INSTR_EFFECT_MODULE_COUNT];
        struct {
            bpbxsyn_effect_s *panning;
            bpbxsyn_effect_s *distortion;
            bpbxsyn_effect_s *bitcrusher;
            bpbxsyn_effect_s *chorus;
            bpbxsyn_effect_s *echo;
            bpbxsyn_effect_s *reverb;
            bpbxsyn_effect_s *eq;
            bpbxsyn_effect_s *fader;
        } fx;
    };

    uint32_t frames_until_next_tick;
    
    // mono output buffer for the synth
    float *synth_mono_buffer;
    
    // stereo processing block (effect processing is done in-place)
    // process_block[0] is the left channel, and process_block[1] is the right.
    float *process_block[2];

    double sample_rate;
    double bpm;
    double cur_beat;
    bool is_playing;
    atomic_bool need_param_rescan;

    double gain;
    bool tempo_use_override;
    double tempo_multiplier;
    double tempo_override;

    // derived from gain property
    double linear_gain;

    // tracked voices
    voice_s voices[BPBXSYN_SYNTH_MAX_VOICES];
    int8_t active_voice_count;

    const clap_host_t *clap_host;
    const clap_host_params_t *clap_host_params;
} instrument_s;

extern const bpbxsyn_synth_type_e instr_synth_type_values[BPBXSYN_SYNTH_COUNT];

#endif