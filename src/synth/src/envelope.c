#include <assert.h>
#include "envelope.h"
#include "instrument.h"
#include "util.h"

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

static double compute_envelope(const envelope_curve_preset_s *curve, double time, double beats, double note_size, double mod_x, double mod_y) {
    switch (curve->curve_type) {
        case ENV_CURVE_NOTE_SIZE:       return note_size_to_volume_mult(note_size);
        case ENV_CURVE_NONE:            return 1.0;
        case ENV_CURVE_TWANG:           return 1.0 / (1.0 + time * curve->speed);
        case ENV_CURVE_SWELL:           return 1.0 - 1.0 / (1.0 + time * curve->speed);
        case ENV_CURVE_TREMOLO:         return 0.5 - cos(beats * 2.0 * PI * curve->speed) * 0.5;
        case ENV_CURVE_TREMOLO2:        return 0.75 - cos(beats * 2.0 * PI * curve->speed) * 0.25;
        case ENV_CURVE_PUNCH:           return max(1.0, 2.0 - time * 10.0);
        case ENV_CURVE_DECAY:           return pow(2.0, -curve->speed * time);
        case ENV_CURVE_BLIP:            return 1.0 * +(time < (0.25 / sqrt(curve->speed)));

        case ENV_CURVE_FLARE: {
            const double attack = 0.25 / sqrt(curve->speed);
            return time < attack ? time / attack : 1.0 / (1.0 + (time - attack) * curve->speed);
        }

        default:
            assert(0 && "unrecognized operator envelope type");
            return 0.0;
    }
}

void envelope_computer_init(envelope_computer_s *env_computer) {
    for (int i = 0; i < BPBX_ENV_INDEX_COUNT; i++) {
        env_computer->envelope_starts[i] = 1.0;
        env_computer->envelope_ends[i] = 1.0;
    }
}

void compute_envelopes(
    envelope_computer_s *env_computer,
    const bpbx_envelope_s *envelopes, unsigned int envelope_count,
    double beat_start, double tick_time_start, double secs_per_tick
) {
    env_computer->note_secs_start = env_computer->note_secs_end;
    env_computer->note_secs_end = env_computer->note_secs_start + secs_per_tick;

    const double tick_time_end = tick_time_start + 1.0;

    const double beats_per_tick = 1.0 / (TICKS_PER_PART * PARTS_PER_BEAT);
    const double beats_time_start = beat_start;
    const double beats_time_end = beat_start + beats_per_tick;

    for (int i = 0; i < BPBX_ENV_INDEX_COUNT; i++) {
        env_computer->envelope_starts[i] = 1.0;
        env_computer->envelope_ends[i] = 1.0;
    }

    for (unsigned int i = 0; i < envelope_count; i++) {
        const bpbx_envelope_s *env = &envelopes[i];
        const envelope_curve_preset_s curve = envelope_curve_presets[env->curve_preset];

        env_computer->envelope_starts[env->index] *= compute_envelope(&curve, env_computer->note_secs_start, beats_time_start, NOTE_SIZE_MAX, 0.0, 0.0);
        env_computer->envelope_ends[env->index] *= compute_envelope(&curve, env_computer->note_secs_end, beats_time_end, NOTE_SIZE_MAX, 0.0, 0.0);
    }
    //env_computer->tick += 1.0;
}




////////////
//  DATA  //
////////////

const envelope_curve_preset_s envelope_curve_presets[BPBX_ENVELOPE_CURVE_PRESET_COUNT] = {
    {
        .name = "none",
        .curve_type = ENV_CURVE_NONE,
    },
    {
        .name = "note size",
        .curve_type = ENV_CURVE_NOTE_SIZE,
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