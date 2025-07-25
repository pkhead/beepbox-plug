#ifndef _bpbxclap_shared_h
#define _bpbxclap_shared_h

#ifdef __cplusplus
extern "C" {
#endif

#include <clap/clap.h>
#include <stdbool.h>

// plugin control parameters, stored in instrument/effect instances
typedef enum {
    PLUGIN_CPARAM_START = 0x1000,

    PLUGIN_CPARAM_GAIN = PLUGIN_CPARAM_START,

    PLUGIN_CPARAM_TEMPO_USE_OVERRIDE,
    PLUGIN_CPARAM_TEMPO_MULTIPLIER,
    PLUGIN_CPARAM_TEMPO_OVERRIDE,

    PLUGIN_CPARAM_END

} plugin_control_param_e;

#define PLUGIN_CPARAM_COUNT (PLUGIN_CPARAM_END - PLUGIN_CPARAM_START)

uint32_t plugin_params_count(const clap_plugin_t *plugin);
bool plugin_params_get_info(const clap_plugin_t *plugin, uint32_t param_index, clap_param_info_t *param_info);
bool plugin_params_get_value(const clap_plugin_t *plugin, clap_id param_id, double *out_value);

#ifdef __cplusplus
}
#endif

#endif