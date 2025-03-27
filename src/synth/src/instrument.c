#include <limits.h>
#include <stdint.h>
#include "instrument.h"
#include "util.h"

#define PARTS_PER_BEAT 24
#define TICKS_PER_PART 2
#define VOLUME_LOG_SCALE 0.1428

double calc_samples_per_tick(double bpm, int sample_rate) {
    const double beats_per_sec = bpm / 60.0;
    const double parts_per_sec = PARTS_PER_BEAT * beats_per_sec;
    const double ticks_per_sec = TICKS_PER_PART * parts_per_sec;
    return (double)sample_rate / ticks_per_sec;
}

double note_size_to_volume_mult(double size) {
    return pow(max(0.0, size) / NOTE_SIZE_MAX, 1.5);
}

double inst_volume_to_mult(double inst_volume) {
    if (inst_volume <= -25.0) return 0.0;
    return pow(2.0, VOLUME_LOG_SCALE * inst_volume);
}