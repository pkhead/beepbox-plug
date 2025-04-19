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
#define BPBX_MAX_ENVELOPE_COUNT 12 // 16 in slarmoo's box
#define BPBX_ENVELOPE_CURVE_PRESET_COUNT 26

#define BPBX_VERSION_MAJOR 0
#define BPBX_VERSION_MINOR 0
#define BPBX_VERSION_REVISION 1

typedef enum {
    BPBX_INSTRUMENT_CHIP,
    BPBX_INSTRUMENT_FM,
    BPBX_INSTRUMENT_NOISE,
    BPBX_INSTRUMENT_PULSE_WIDTH,
    BPBX_INSTRUMENT_HARMONICS,
    BPBX_INSTRUMENT_SPECTRUM,
    BPBX_INSTRUMENT_PICKED_STRING,
    BPBX_INSTRUMENT_SUPERSAW,
} bpbx_inst_type_e;

typedef enum {
    BPBX_PARAM_UINT8,
    BPBX_PARAM_INT,
    BPBX_PARAM_DOUBLE
} bpbx_inst_param_type_e;

typedef enum {
    BPBX_PARAM_FLAG_NO_AUTOMATION = 1
} bpbx_inst_param_flags_e;

typedef enum {
    BPBX_ENV_INDEX_NONE,
    BPBX_ENV_INDEX_NOTE_VOLUME,
    BPBX_ENV_INDEX_NOTE_FILTER_ALL_FREQS,
    BPBX_ENV_INDEX_PULSE_WIDTH,
    BPBX_ENV_INDEX_STRING_SUSTAIN,
    BPBX_ENV_INDEX_UNISON,
    BPBX_ENV_INDEX_OPERATOR_FREQ0,
    BPBX_ENV_INDEX_OPERATOR_FREQ1,
    BPBX_ENV_INDEX_OPERATOR_FREQ2,
    BPBX_ENV_INDEX_OPERATOR_FREQ3,
    BPBX_ENV_INDEX_OPERATOR_AMP0,
    BPBX_ENV_INDEX_OPERATOR_AMP1,
    BPBX_ENV_INDEX_OPERATOR_AMP2,
    BPBX_ENV_INDEX_OPERATOR_AMP3,
    BPBX_ENV_INDEX_FEEDBACK_AMP,
    BPBX_ENV_INDEX_PITCH_SHIFT,
    BPBX_ENV_INDEX_DETUNE,
    BPBX_ENV_INDEX_VIBRATO_DEPTH,
    BPBX_ENV_INDEX_NOTE_FILTER_FREQ0,
    BPBX_ENV_INDEX_NOTE_FILTER_FREQ1,
    BPBX_ENV_INDEX_NOTE_FILTER_FREQ2,
    BPBX_ENV_INDEX_NOTE_FILTER_FREQ3,
    BPBX_ENV_INDEX_NOTE_FILTER_FREQ4,
    BPBX_ENV_INDEX_NOTE_FILTER_FREQ5,
    BPBX_ENV_INDEX_NOTE_FILTER_FREQ6,
    BPBX_ENV_INDEX_NOTE_FILTER_FREQ7,
    BPBX_ENV_INDEX_NOTE_FILTER_GAIN0,
    BPBX_ENV_INDEX_NOTE_FILTER_GAIN1,
    BPBX_ENV_INDEX_NOTE_FILTER_GAIN2,
    BPBX_ENV_INDEX_NOTE_FILTER_GAIN3,
    BPBX_ENV_INDEX_NOTE_FILTER_GAIN4,
    BPBX_ENV_INDEX_NOTE_FILTER_GAIN5,
    BPBX_ENV_INDEX_NOTE_FILTER_GAIN6,
    BPBX_ENV_INDEX_NOTE_FILTER_GAIN7,
    BPBX_ENV_INDEX_SUPERSAW_DYNAMISM,
    BPBX_ENV_INDEX_SUPERSAW_SPREAD,
    BPBX_ENV_INDEX_SUPERSAW_SHAPE,
    BPBX_ENV_INDEX_COUNT
} bpbx_envelope_compute_index_e;

typedef struct {
    bpbx_envelope_compute_index_e index;
    uint8_t curve_preset;
} bpbx_envelope_s;

typedef struct {
    bpbx_inst_param_type_e type;
    uint32_t flags; // bpbx_inst_param_flags_e

    const char *name;
    const char *envelope_name;

    double min_value;
    double max_value;
    double default_value;

    const char **enum_values;
} bpbx_inst_param_info_s;

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
} bpbx_run_ctx_s;

typedef struct bpbx_inst_s bpbx_inst_s;

BEEPBOX_API void bpbx_version(uint32_t *major, uint32_t *minor, uint32_t *revision);

BEEPBOX_API const unsigned int bpbx_param_count(bpbx_inst_type_e type);
BEEPBOX_API const bpbx_inst_param_info_s* bpbx_param_info(bpbx_inst_type_e type, unsigned int index);

BEEPBOX_API bpbx_inst_s* bpbx_inst_new(bpbx_inst_type_e inst_type);
BEEPBOX_API void bpbx_inst_destroy(bpbx_inst_s* inst);

BEEPBOX_API bpbx_inst_type_e bpbx_inst_type(const bpbx_inst_s *inst);

BEEPBOX_API void bpbx_inst_set_sample_rate(bpbx_inst_s *inst, double sample_rate);

BEEPBOX_API int bpbx_inst_set_param_int(bpbx_inst_s* inst, int index, int value);
BEEPBOX_API int bpbx_inst_set_param_double(bpbx_inst_s* inst, int index, double value);

BEEPBOX_API int bpbx_inst_get_param_int(const bpbx_inst_s* inst, int index, int *value);
BEEPBOX_API int bpbx_inst_get_param_double(const bpbx_inst_s* inst, int index, double *value);

BEEPBOX_API const char* bpbx_envelope_index_name(bpbx_envelope_compute_index_e index);
BEEPBOX_API const char** bpbx_envelope_curve_preset_names();

BEEPBOX_API const bpbx_envelope_compute_index_e* bpbx_envelope_targets(bpbx_inst_type_e type, int *size);

BEEPBOX_API uint8_t bpbx_inst_envelope_count(const bpbx_inst_s *inst);
// note: envelopes are stored contiguously and in order, so it is valid to treat the return value
// as an array.
BEEPBOX_API bpbx_envelope_s* bpbx_inst_get_envelope(bpbx_inst_s *inst, uint32_t index);
BEEPBOX_API bpbx_envelope_s* bpbx_inst_add_envelope(bpbx_inst_s *inst);
BEEPBOX_API void bpbx_inst_remove_envelope(bpbx_inst_s *inst, uint8_t index);
BEEPBOX_API void bpbx_inst_clear_envelopes(bpbx_inst_s *inst);

BEEPBOX_API void bpbx_inst_midi_on(bpbx_inst_s *inst, int key, int velocity);
BEEPBOX_API void bpbx_inst_midi_off(bpbx_inst_s *inst, int key, int velocity);

// if you know the length of each note, and the result of this is negative,
// call midi_off that positive number of samples before the note actually ends.
BEEPBOX_API double bpbx_samples_fade_out(double setting, double bpm, double sample_rate);

BEEPBOX_API void bpbx_inst_run(bpbx_inst_s* inst, const bpbx_run_ctx_s *const run_ctx);


#ifdef __cplusplus
}
}
#endif

#endif