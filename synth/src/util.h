#ifndef _util_h_
#define _util_h_

#include "math.h"

#define PI2 (2.0 * 3.14159265359)
#define PI2f (float)(2.f * 3.14159265359f)

// SINE_WAVE_LENGTH must be a power of 2!
#define SINE_WAVE_LENGTH 256
extern float sine_wave_f[SINE_WAVE_LENGTH];
extern float sine_wave_d[SINE_WAVE_LENGTH];
extern void init_wavetables();

static inline float lerpf(float min, float max, float t) {
    return (max - min) * t + min;
}

static inline float clampf(float v, float min, float max) {
    if (v > max) return max;
    if (v < min) return min;
    return v;
}

static inline int clampi(int v, int min, int max) {
    if (v > max) return max;
    if (v < min) return min;
    return v;
}

static inline int signf(float v) {
    if (v > 0.0f) return 1;
    if (v < 0.0f) return -1;
    return 0;
}

inline float key_to_hz_f(int key) {
    return powf(2.f, (key - 69) / 12.f) * 440.f;
}

inline double key_to_hz_d(double key) {
    return pow(2.0, (key - 69) / 12.0) * 440.0;
}

#endif