#ifndef _wavetables_h_
#define _wavetables_h_

// SINE_WAVE_LENGTH must be a power of 2!
#define SINE_WAVE_LENGTH 256
extern float sine_wave_f[SINE_WAVE_LENGTH];
extern double sine_wave_d[SINE_WAVE_LENGTH];
extern void init_wavetables();

#endif