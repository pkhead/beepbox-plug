#ifndef _public_h_
#define _public_h_

#include <stddef.h>
#include "../include/beepbox_synth.h"
#include "envelope.h"
#include "fm.h"

#define NOTE_SIZE_MAX 3

typedef struct inst {
    inst_type_e type;
    double sample_rate;

    double volume;
    double panning;
    double fade_in; // double in range of 0-9
    double fade_out;

    union {
        fm_inst_s *fm;
    };

    // envelopes
    uint8_t envelope_count;
    envelope_s envelopes[MAX_ENVELOPE_COUNT];

    envelope_computer_s env_computer;
} inst_s;

double calc_samples_per_tick(double bpm, double sample_rate);
double note_size_to_volume_mult(double size);
double inst_volume_to_mult(double inst_volume);

#endif