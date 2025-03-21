#ifndef _public_h_
#define _public_h_

#include <stddef.h>
#include "../include/beepbox_synth.h"
#include "fm.h"

typedef struct inst_t {
    inst_type_t type;
    int sample_rate;

    union {
        fm_inst_t fm;
    };
} inst_t;

#endif