#include <math.h>
#include "util.h"

float sine_wave_f[SINE_WAVE_LENGTH];
float sine_wave_d[SINE_WAVE_LENGTH];
static int need_init_wavetables = 1;

void init_wavetables() {
    if (need_init_wavetables) {
        need_init_wavetables = 0;

        for (int i = 0; i < SINE_WAVE_LENGTH; i++) {
            sine_wave_d[i] = sin((double)i / SINE_WAVE_LENGTH * PI2);
            sine_wave_f[i] = sinf((float)i / SINE_WAVE_LENGTH * PI2f);
        }
    }
}