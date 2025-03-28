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
#define MAX_ENVELOPE_COUNT 12 // 16 in slarmoo's box
#define ENVELOPE_CURVE_PRESET_COUNT 26

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

typedef enum {
    PARAM_FLAG_NO_AUTOMATION = 1
} inst_param_flags_e;

typedef enum {
    ENV_INDEX_NONE,
    ENV_INDEX_NOTE_VOLUME,
    ENV_INDEX_NOTE_FILTER_ALL_FREQS,
    ENV_INDEX_PULSE_WIDTH,
    ENV_INDEX_STRING_SUSTAIN,
    ENV_INDEX_UNISON,
    ENV_INDEX_OPERATOR_FREQ0,
    ENV_INDEX_OPERATOR_FREQ1,
    ENV_INDEX_OPERATOR_FREQ2,
    ENV_INDEX_OPERATOR_FREQ3,
    ENV_INDEX_OPERATOR_AMP0,
    ENV_INDEX_OPERATOR_AMP1,
    ENV_INDEX_OPERATOR_AMP2,
    ENV_INDEX_OPERATOR_AMP3,
    ENV_INDEX_FEEDBACK_AMP,
    ENV_INDEX_PITCH_SHIFT,
    ENV_INDEX_DETUNE,
    ENV_INDEX_VIBRATO_DEPTH,
    ENV_INDEX_NOTE_FILTER_FREQ0,
    ENV_INDEX_NOTE_FILTER_FREQ1,
    ENV_INDEX_NOTE_FILTER_FREQ2,
    ENV_INDEX_NOTE_FILTER_FREQ3,
    ENV_INDEX_NOTE_FILTER_FREQ4,
    ENV_INDEX_NOTE_FILTER_FREQ5,
    ENV_INDEX_NOTE_FILTER_FREQ6,
    ENV_INDEX_NOTE_FILTER_FREQ7,
    ENV_INDEX_NOTE_FILTER_GAIN0,
    ENV_INDEX_NOTE_FILTER_GAIN1,
    ENV_INDEX_NOTE_FILTER_GAIN2,
    ENV_INDEX_NOTE_FILTER_GAIN3,
    ENV_INDEX_NOTE_FILTER_GAIN4,
    ENV_INDEX_NOTE_FILTER_GAIN5,
    ENV_INDEX_NOTE_FILTER_GAIN6,
    ENV_INDEX_NOTE_FILTER_GAIN7,
    ENV_INDEX_SUPERSAW_DYNAMISM,
    ENV_INDEX_SUPERSAW_SPREAD,
    ENV_INDEX_SUPERSAW_SHAPE,
    ENV_INDEX_COUNT
} envelope_compute_index_e;

typedef struct {
    envelope_compute_index_e index;
    uint8_t curve_preset;
} envelope_s;

typedef struct {
    inst_param_type_e type;
    uint32_t flags; // inst_param_flags_e

    const char *name;
    const char *envelope_name;

    double min_value;
    double max_value;
    double default_value;

    const char **enum_values;
} inst_param_info_s;

typedef struct {
    float *out_samples;
    size_t frame_count;
    
    // do NOT set this to zero or a very low value!
    // beepbox runs voice computations at a tick rate derived from the tempo.
    // if you don't have bpm information, set it to a dummy value like 60 or 150.
    double bpm;

    // beat must be continuously increasing in order for
    // the tremolo envelopes to work properly!
    // if you don't have that information or the song isn't playing,
    // simply constantly increase this by the bpm.
    double beat;
} run_ctx_s;

typedef struct inst inst_s;

BEEPBOX_API const unsigned int inst_param_count(inst_type_e type);
BEEPBOX_API const inst_param_info_s* inst_param_info(inst_type_e type, unsigned int index);

BEEPBOX_API inst_s* inst_new(inst_type_e inst_type);
BEEPBOX_API void inst_destroy(inst_s* inst);

BEEPBOX_API inst_type_e inst_type(const inst_s *inst);

BEEPBOX_API void inst_set_sample_rate(inst_s *inst, double sample_rate);

BEEPBOX_API int inst_set_param_int(inst_s* inst, int index, int value);
BEEPBOX_API int inst_set_param_double(inst_s* inst, int index, double value);

BEEPBOX_API int inst_get_param_int(const inst_s* inst, int index, int *value);
BEEPBOX_API int inst_get_param_double(const inst_s* inst, int index, double *value);

BEEPBOX_API const char* envelope_index_name(envelope_compute_index_e index);
BEEPBOX_API const char** envelope_curve_preset_names();

BEEPBOX_API const envelope_compute_index_e* inst_envelope_targets(inst_type_e type, int *size);

BEEPBOX_API uint8_t inst_envelope_count(const inst_s *inst);
// note: envelopes are stored contiguously and in order, so it is valid to treat the return value
// as an array.
BEEPBOX_API envelope_s* inst_get_envelope(inst_s *inst, uint32_t index);
BEEPBOX_API envelope_s* inst_add_envelope(inst_s *inst);
BEEPBOX_API void inst_remove_envelope(inst_s *inst, uint8_t index);

BEEPBOX_API void inst_midi_on(inst_s *inst, int key, int velocity);
BEEPBOX_API void inst_midi_off(inst_s *inst, int key, int velocity);

// if you know the length of each note, and the result of this is negative,
// call midi_off that positive number of samples before the note actually ends.
BEEPBOX_API double inst_samples_fade_out(double setting, double bpm, double sample_rate);

BEEPBOX_API void inst_run(inst_s* inst, const run_ctx_s *const run_ctx);


#ifdef __cplusplus
}
}
#endif

#endif