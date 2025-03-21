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
    PARAM_FLOAT
} inst_param_type_t;

typedef struct {
    inst_param_type_t type;
    const char *name;

    float min_value;
    float max_value;
    float default_value;
} inst_param_info_t;

typedef void inst_t;

inst_t* inst_new(inst_type_t inst_type, int sample_rate, int channel_count);
void inst_destroy(inst_t* inst);

const inst_param_info_t* inst_list_params(const inst_t* inst, int *count);

int inst_set_param_int(inst_t* inst, int index, int value);
int inst_set_param_float(inst_t* inst, int index, float value);

int inst_get_param_int(inst_t* inst, int index, int *value);
float inst_get_param_float(inst_t* inst, int index, float *value);

void inst_midi_on(inst_t *inst, int key, int velocity);
void inst_midi_off(inst_t *inst, int key, int velocity);

void inst_run(inst_t* inst, float *out_samples, size_t frame_count);

#ifdef __cplusplus
}
}
#endif

#endif