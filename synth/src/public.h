#ifndef _public_h_
#define _public_h_

#include <stddef.h>
#include "fm.h"
#include "util.h"
#include "inst_info.h"

typedef struct {
    inst_type_t type;
    int sample_rate;
    int channel_count;

    union {
        fm_inst_t fm;
    };
} inst_t;

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

#endif