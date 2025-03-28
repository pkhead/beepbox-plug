#ifndef _envelope_h_
#define _envelope_h_

#include <stdint.h>
#include "../include/beepbox_synth.h"

#define FADE_OUT_RANGE 11
#define FADE_OUT_MIN -4
#define FADE_OUT_MAX 6

typedef struct {
    double envelope_starts[ENV_INDEX_COUNT];
    double envelope_ends[ENV_INDEX_COUNT];
} envelope_computer_s;

typedef enum {
    ENV_CURVE_NONE,
    ENV_CURVE_VOLUME, // aka note size
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

extern const envelope_curve_preset_s envelope_curve_presets[ENVELOPE_CURVE_PRESET_COUNT];

#endif