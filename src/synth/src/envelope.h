#ifndef _envelope_h_
#define _envelope_h_

#include <stdint.h>
#include "../include/beepbox_synth.h"

#define FADE_OUT_RANGE 11
#define FADE_OUT_MIN -4
#define FADE_OUT_MAX 6

typedef struct {
    double envelope_starts[BPBX_ENV_INDEX_COUNT];
    double envelope_ends[BPBX_ENV_INDEX_COUNT];
    double tick;
    double note_secs_start;
    double note_secs_end;
} envelope_computer_s;

typedef enum {
    ENV_CURVE_NONE,
    ENV_CURVE_NOTE_SIZE, // aka note size
    ENV_CURVE_PUNCH,
    ENV_CURVE_FLARE,
    ENV_CURVE_TWANG,
    ENV_CURVE_SWELL,
    ENV_CURVE_TREMOLO,
    ENV_CURVE_TREMOLO2,
    ENV_CURVE_DECAY,
    ENV_CURVE_BLIP, // from jummbox

    ENV_CURVE_MOD_X, // probably MIDI CC 1 (Modulation wheel)
    ENV_CURVE_MOD_Y,
} envelope_curve_type_e;

double secs_fade_in(double setting);
double ticks_fade_out(double setting);

typedef struct {
    const char *name;
    envelope_curve_type_e curve_type;
    double speed;
} envelope_curve_preset_s;

void envelope_computer_init(envelope_computer_s *env_computer);

void compute_envelopes(
    envelope_computer_s *env_computer,
    const bpbx_envelope_s *envelopes, unsigned int envelope_count,
    double beat_start, double tick_time_start, double secs_per_tick
);

extern const envelope_curve_preset_s envelope_curve_presets[BPBX_ENVELOPE_CURVE_PRESET_COUNT];

#endif