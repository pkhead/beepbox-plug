#ifndef _util_h_
#define _util_h_

#include "math.h"

#define PI 3.14159265359
#define PI2 (2.0 * 3.14159265359)
#define PI2f (float)(2.f * 3.14159265359f)

#define FALSE 0
#define TRUE 1

static inline float lerpf(float min, float max, float t) {
    return (max - min) * t + min;
}

static inline float clampf(float v, float min, float max) {
    if (v > max) return max;
    if (v < min) return min;
    return v;
}

static inline double clampd(double v, double min, double max) {
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

// wtf i'm not even including any windows headers
#undef min
#undef max

static inline double min(double a, double b) {
    return a < b ? a : b;
}

static inline double max(double a, double b) {
    return a > b ? a : b;
}

inline float key_to_hz_f(int key) {
    return powf(2.f, (key - 69) / 12.f) * 440.f;
}

inline double key_to_hz_d(double key) {
    return pow(2.0, (key - 69) / 12.0) * 440.0;
}

#endif