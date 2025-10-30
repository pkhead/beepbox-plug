#ifndef _bpbxclapplug_instrument_h_
#define _bpbxclapplug_instrument_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <cbeepsynth/synth/include/beepbox_synth.h>
#include <clap/clap.h>

#define INSTR_INVALID_ID UINT32_MAX
typedef uint32_t instr_param_id;

// maximum target count is 16
typedef enum {
    INSTR_MODULE_SYNTH,
    INSTR_MODULE_CONTROL,
    
    INSTR_MODULE_PANNING,
    INSTR_MODULE_DISTORTION,
    INSTR_MODULE_BITCRUSHER,
    INSTR_MODULE_CHORUS,
    INSTR_MODULE_ECHO,
    INSTR_MODULE_REVERB,
    INSTR_MODULE_EQ,
    INSTR_MODULE_VOLUME,

    INSTR_MODULE_COUNT
} instr_module_e;

#define INSTR_FIRST_EFFECT_MODULE INSTR_MODULE_PANNING
#define INSTR_LAST_EFFECT_MODULE INSTR_MODULE_VOLUME
#define INSTR_EFFECT_MODULE_COUNT ((INSTR_LAST_EFFECT_MODULE - INSTR_FIRST_EFFECT_MODULE + 1))

// plugin control parameters, stored in instrument/effect instances
typedef enum {
    INSTR_CPARAM_SYNTH_TYPE,

    INSTR_CPARAM_GAIN,
    INSTR_CPARAM_TEMPO_USE_OVERRIDE,
    INSTR_CPARAM_TEMPO_MULTIPLIER,
    INSTR_CPARAM_TEMPO_OVERRIDE,

    INSTR_CPARAM_ENABLE_DISTORTION,
    INSTR_CPARAM_ENABLE_BITCRUSHER,
    INSTR_CPARAM_ENABLE_CHORUS,
    INSTR_CPARAM_ENABLE_ECHO,
    INSTR_CPARAM_ENABLE_REVERB,

    INSTR_CPARAM_COUNT
} instr_cparam_e;

typedef struct instrument instrument_s;

bool instr_init(instrument_s *instr, bpbxsyn_context_s *ctx, bpbxsyn_synth_type_e type);
void instr_destroy(instrument_s *instr);
void instr_process(instrument_s *instr, float **output, uint32_t frame_count,
                   uint32_t start_frame, const clap_output_events_t *out_events);
void instr_on_main_thread(instrument_s *instr);
bpbxsyn_synth_s* instr_get_synth(const instrument_s *instr);

bool instr_activate(instrument_s *instr, bpbxsyn_context_s *ctx,
                    double sample_rate, uint32_t max_frames_count);
bool instr_deactivate(instrument_s *instr);

void instr_set_effect_active(instrument_s *instr, bpbxsyn_effect_type_e effect,
                             bool value);
// bool instr_is_module_active(const instrument_s *instr, instr_module_e module);

void instr_begin_note(instrument_s *instr, int16_t key, double velocity,
                      int32_t note_id, int16_t port_index, int16_t channel);

void instr_end_notes(instrument_s *instr, int16_t key, int32_t note_id,
                     int16_t port_index, int16_t channel);

uint32_t instr_params_count(const instrument_s *instr);
instr_param_id instr_get_param_id(const instrument_s *instr, uint32_t index,
                                  bool *is_inactive);

bool instr_set_param(instrument_s *instr, instr_param_id id, double *value);
bool instr_get_param(const instrument_s *instr, instr_param_id id, double *value);

const bpbxsyn_param_info_s* instr_get_param_info(const instrument_s *instr,
                                                 instr_param_id id);

// calculate type index of a given synth. type index uses different values than the values
// for the type enums.
int instr_synth_type_index(bpbxsyn_synth_type_e type);

extern const bpbxsyn_synth_type_e instr_synth_type_values[BPBXSYN_SYNTH_COUNT];

// int instr_global_id(instr_module_e target, instr_param_id local_index);
#define instr_global_id(module, local_index) \
    (((local_index) & 0xFFFF) | ((module) << 16))

void instr_local_id(instr_param_id global_index, instr_module_e *out_target,
                    instr_param_id *out_local_index);

#define instr_synth_param(index) (instr_global_id(INSTR_MODULE_SYNTH, index))

#ifdef __cplusplus
}
#endif

#endif