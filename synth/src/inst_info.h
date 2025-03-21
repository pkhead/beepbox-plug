#ifndef _inst_info_h_
#define _inst_info_h_

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

#endif 