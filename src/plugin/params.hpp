#pragma once

#include <beepbox_synth.h>

namespace bbxplug {

typedef uint32_t param_id;

// each valid parameter ID belongs to one of these modules. and each parameter
// ID has a specific meaning.
enum param_module {
    // Synth type
    MODULE_SYNTH_CONTROL,
    // Gain control
    MODULE_GENERAL_CONTROL,
    // Tempo control for synth and echo effect
    MODULE_TEMPO_CONTROL,
    // Effect controls
    MODULE_EFFECT_CONTROL,

    // Synthesizer
    MODULE_SYNTH,

    // Effect modules
    MODULE_PANNING,
    MODULE_DISTORTION,
    MODULE_BITCRUSHER,
    MODULE_CHORUS,
    MODULE_ECHO,
    MODULE_REVERB,
    MODULE_EQ,
    MODULE_VOLUME,

    // Not an actual module: merely the number of them.
    MODULE_COUNT
};

enum synth_control_module_param {
    SYNTH_CONTROL_PARAM_TYPE,

    SYNTH_CONTROL_PARAM__COUNT,
};

enum general_control_module_param {
    GENERAL_CONTROL_PARAM_GAIN,

    GENERAL_CONTROL_PARAM__COUNT,
};

enum tempo_control_module_param {
    TEMPO_CONTROL_PARAM_USE_OVERRIDE,
    TEMPO_CONTROL_PARAM_MULTIPLIER,
    TEMPO_CONTROL_PARAM_OVERRIDE,

    TEMPO_CONTROL_PARAM__COUNT,
};

enum effect_control_module_param {
    EFFECT_CONTROL_PARAM_ENABLE_DISTORTION,
    EFFECT_CONTROL_PARAM_ENABLE_BITCRUSHER,
    EFFECT_CONTROL_PARAM_ENABLE_CHORUS,
    EFFECT_CONTROL_PARAM_ENABLE_ECHO,
    EFFECT_CONTROL_PARAM_ENABLE_REVERB,

    EFFECT_CONTROL_PARAM__COUNT,
};

struct param_info {
    bool active;
    void *cookie;
    const bbxs_param_info_s *info;
};

static constexpr param_module MODULE_FIRST_EFFECT = MODULE_PANNING;
static constexpr param_module MODULE_LAST_EFFECT = MODULE_VOLUME;
static constexpr int MODULE_EFFECT_COUNT =
    (int)(MODULE_LAST_EFFECT - MODULE_FIRST_EFFECT + 1);
static constexpr param_id INVALID_PARAMETER_ID = UINT32_MAX;

static constexpr uint32_t PARAM_LOCAL_INDEX_MASK = 0xFF;
static constexpr uint32_t PARAM_GLOBAL_INDEX_SHIFT = 8;

const bbxs_param_info_s *get_param_info_with_type(bbxs_synth_type_e synth_type,
                                                  param_id id);

static constexpr param_id param_global_id(param_module module,
                                          param_id local_index) noexcept {
    return (uint32_t)(((local_index & PARAM_LOCAL_INDEX_MASK) |
                       ((int)module << PARAM_GLOBAL_INDEX_SHIFT)));
}

static constexpr void param_local_id(param_id global_id,
                                     param_module *out_module,
                                     param_id *out_local_index) noexcept {
    if (out_local_index) *out_local_index = global_id & PARAM_LOCAL_INDEX_MASK;
    if (out_module)
        *out_module = (param_module)(global_id >> PARAM_GLOBAL_INDEX_SHIFT);
}

#define BBXPLUG_CTLPARAM(MODULE, NAME)                                         \
    param_global_id(bbxplug::MODULE_ ## MODULE ## _CONTROL,                        \
                    bbxplug::MODULE ## _CONTROL_PARAM_ ## NAME)
#define BBXPLUG_SYNPARAM(SYNTYPE, NAME)                                        \
    param_global_id(bbxplug::MODULE_SYNTH, BBXS_ ## SYNTYPE ## _PARAM_ ## NAME)

extern const bbxs_param_info_s
    synth_control_param_info[SYNTH_CONTROL_PARAM__COUNT];
extern const bbxs_param_info_s
    general_control_param_info[GENERAL_CONTROL_PARAM__COUNT];
extern const bbxs_param_info_s
    tempo_control_param_info[TEMPO_CONTROL_PARAM__COUNT];
extern const bbxs_param_info_s
    effect_control_param_info[EFFECT_CONTROL_PARAM__COUNT];
extern const bbxs_param_info_s unused_param_info;

} // namespace bbxplug