#ifndef _envelope_h_
#define _envelope_h_

#include <stdint.h>

#define FADE_OUT_RANGE 11
#define FADE_OUT_MIN -4
#define FADE_OUT_MAX 6

double secs_fade_in(double setting);
double ticks_fade_out(double setting);

#endif