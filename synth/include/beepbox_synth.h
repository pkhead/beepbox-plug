#ifndef _include_inst_h_
#define _include_inst_h_

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

#include "../src/fm_data.h"

typedef enum {
    INSTRUMENT_CHIP,
    INSTRUMENT_FM,
    INSTRUMENT_NOISE,
    INSTRUMENT_PULSE_WIDTH,
    INSTRUMENT_HARMONICS,
    INSTRUMENT_SPECTRUM,
    INSTRUMENT_PICKED_STRING,
    INSTRUMENT_SUPERSAW,
} inst_type_t;

typedef enum {
    PARAM_UINT8,
    PARAM_INT,
    PARAM_DOUBLE
} inst_param_type_t;

typedef struct {
    inst_param_type_t type;
    const char *name;

    uint8_t no_modulation;
    double min_value;
    double max_value;
    double default_value;
} inst_param_info_t;

typedef struct inst_t inst_t;

const int inst_param_count(inst_type_t type);
const inst_param_info_t* inst_param_info(inst_type_t type);

inst_t* inst_new(inst_type_t inst_type);
void inst_destroy(inst_t* inst);

inst_type_t inst_type(inst_t *inst);

void inst_set_sample_rate(inst_t *inst, int sample_rate);

int inst_set_param_int(inst_t* inst, int index, int value);
int inst_set_param_double(inst_t* inst, int index, double value);

int inst_get_param_int(inst_t* inst, int index, int *value);
int inst_get_param_double(inst_t* inst, int index, double *value);

void inst_midi_on(inst_t *inst, int key, int velocity);
void inst_midi_off(inst_t *inst, int key, int velocity);

void inst_run(inst_t* inst, float *out_samples, size_t frame_count);

#ifdef __cplusplus
}
}
#endif

#endif