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