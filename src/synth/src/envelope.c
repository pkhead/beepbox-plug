#include "envelope.h"

double secs_fade_in(double setting) {
    return 0.0125 * (0.95 * setting + 0.05 * setting * setting);
}

// neutral index is at idx4
static double fade_out_ticks[] = { -24.0, -12.0, -6.0, -3.0, -1.0, 6.0, 12.0, 24.0, 48.0, 72.0, 96.0 };

double ticks_fade_out(double setting) {
    setting += 4.0;

    // not possible in normal beepbox; extrapolation
    // using an upward-opening quadratic which intersects with the points (5,6) and (10,96)
    if (setting < 0.0) return fade_out_ticks[0];
    if (setting > FADE_OUT_RANGE - 1) return 1.68 * setting * setting - 7.2 * setting * setting;

    // fractional settings are not possible in normal beepbox; linear interpolation
    int index = (int) setting;
    double result = fade_out_ticks[index];
    return (fade_out_ticks[index+1] - result) * (setting - index) + result;
}




////////////
//  DATA  //
////////////

const envelope_curve_preset_s envelope_curve_presets[ENVELOPE_CURVE_PRESET_COUNT] = {
    {
        .name = "none",
        .curve_type = ENV_CURVE_NONE,
    },
    {
        .name = "note size",
        .curve_type = ENV_CURVE_VOLUME,
    },
    {
        .name = "mod x",
        .curve_type = ENV_CURVE_MOD_X
    },
    {
        .name = "mod y",
        .curve_type = ENV_CURVE_MOD_Y
    },
    {
        .name = "punch",
        .curve_type = ENV_CURVE_PUNCH,
    },
    {
        .name = "flare 1",
        .curve_type = ENV_CURVE_FLARE,
        .speed = 32.0
    },
    {
        .name = "flare 2",
        .curve_type = ENV_CURVE_FLARE,
        .speed = 8.0
    },

    {
        .name = "flare 3",
        .curve_type = ENV_CURVE_FLARE,
        .speed = 2.0
    },
    {
        .name = "twang 1",
        .curve_type = ENV_CURVE_TWANG,
        .speed = 32.0
    },
    {
        .name = "twang 2",
        .curve_type = ENV_CURVE_TWANG,
        .speed = 8.0
    },
    {
        .name = "twang 3",
        .curve_type = ENV_CURVE_TWANG,
        .speed = 2.0
    },
    {
        .name = "swell 1",
        .curve_type = ENV_CURVE_SWELL,
        .speed = 32.0
    },
    {
        .name = "swell 2",
        .curve_type = ENV_CURVE_SWELL,
        .speed = 8.0
    },
    {
        .name = "swell 3",
        .curve_type = ENV_CURVE_SWELL,
        .speed = 2.0
    },
    {
        .name = "tremolo 1",
        .curve_type = ENV_CURVE_TREMOLO,
        .speed = 4.0
    },
    {
        .name = "tremolo 2",
        .curve_type = ENV_CURVE_TREMOLO,
        .speed = 2.0
    },
    {
        .name = "tremolo 3",
        .curve_type = ENV_CURVE_TREMOLO,
        .speed = 1.0,
    },
    {
        .name = "tremolo 4",
        .curve_type = ENV_CURVE_TREMOLO2,
        .speed = 4.0
    },
    {
        .name = "tremolo 5",
        .curve_type = ENV_CURVE_TREMOLO2,
        .speed = 2.0
    },
    {
        .name = "tremolo 6",
        .curve_type = ENV_CURVE_TREMOLO2,
        .speed = 1.0
    },
    {
        .name = "decay 1",
        .curve_type = ENV_CURVE_DECAY,
        .speed = 10.0
    },
    {
        .name = "decay 2",
        .curve_type = ENV_CURVE_DECAY,
        .speed = 7.0
    },
    {
        .name = "decay 3",
        .curve_type = ENV_CURVE_DECAY,
        .speed = 4.0
    },
    {
        .name = "blip 1",
        .curve_type = ENV_CURVE_BLIP,
        .speed = 6.0
    },
    {
        .name = "blip 2",
        .curve_type = ENV_CURVE_BLIP,
        .speed = 16.0
    },
    {
        .name = "blip 3",
        .curve_type = ENV_CURVE_BLIP,
        .speed = 32.0
    },
};