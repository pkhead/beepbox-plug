// Copyright 2025 pkhead
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
// is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
// IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef _beepbox_synth_h_
#define _beepbox_synth_h_

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#ifdef __cplusplus
namespace beepbox {
extern "C" {
#endif

#include "beepbox_instrument_data.h"

#define BEEPBOX_API

typedef enum {
    INSTRUMENT_CHIP,
    INSTRUMENT_FM,
    INSTRUMENT_NOISE,
    INSTRUMENT_PULSE_WIDTH,
    INSTRUMENT_HARMONICS,
    INSTRUMENT_SPECTRUM,
    INSTRUMENT_PICKED_STRING,
    INSTRUMENT_SUPERSAW,
} inst_type_e;

typedef enum {
    PARAM_UINT8,
    PARAM_INT,
    PARAM_DOUBLE
} inst_param_type_e;

typedef struct {
    inst_param_type_e type;
    const char *name;

    uint8_t no_modulation;
    double min_value;
    double max_value;
    double default_value;

    const char **enum_values;
} inst_param_info_s;

typedef struct {
    float *out_samples;
    size_t frame_count;

    double bpm;
} run_ctx_s;

typedef struct inst inst_s;

BEEPBOX_API const unsigned int inst_param_count(inst_type_e type);
BEEPBOX_API const inst_param_info_s* inst_param_info(inst_type_e type, unsigned int index);

BEEPBOX_API inst_s* inst_new(inst_type_e inst_type);
BEEPBOX_API void inst_destroy(inst_s* inst);

BEEPBOX_API inst_type_e inst_type(const inst_s *inst);

BEEPBOX_API void inst_set_sample_rate(inst_s *inst, int sample_rate);

BEEPBOX_API int inst_set_param_int(inst_s* inst, int index, int value);
BEEPBOX_API int inst_set_param_double(inst_s* inst, int index, double value);

BEEPBOX_API int inst_get_param_int(const inst_s* inst, int index, int *value);
BEEPBOX_API int inst_get_param_double(const inst_s* inst, int index, double *value);

BEEPBOX_API void inst_midi_on(inst_s *inst, int key, int velocity);
BEEPBOX_API void inst_midi_off(inst_s *inst, int key, int velocity);

// if you know the length of each note, and the result of this is negative,
// call midi_off that positive number of samples before the note actually ends.
BEEPBOX_API double inst_samples_fade_out(double setting, double bpm, int sample_rate);

BEEPBOX_API void inst_run(inst_s* inst, const run_ctx_s *const run_ctx);


#ifdef __cplusplus
}
}
#endif

#endif