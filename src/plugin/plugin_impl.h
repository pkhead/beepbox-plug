#ifndef _bpbxclap_plugin_impl_h_
#define _bpbxclap_plugin_impl_h_

#include <beepbox_synth.h>
#include <clap/clap.h>
#include "include/instrument.h"
#include <plugin_gui.h>

typedef struct {
    clap_plugin_t plugin;
    plugin_gui_s *gui;

    bool has_track_color;
    clap_color_t track_color;

    const clap_host_t *host;
    const clap_host_latency_t *host_latency;
    const clap_host_log_t *host_log;
    const clap_host_thread_check_t *host_thread_check;
    const clap_host_params_t *host_params;
    const clap_host_state_t *host_state;
    const clap_host_track_info_t *host_track_info;
    const clap_host_context_menu_t *host_context_menu;

    bpbxsyn_context_s *ctx;
    instrument_s instrument;

    #ifndef _NDEBUG
    size_t mem_allocated;
    #endif
} plugin_s;

typedef enum {
    SEND_TO_GUI = 1,
    SEND_TO_HOST = 2,
    NO_RECURSION = 4,
} event_send_flags_e;

void plugin_static_init(void);
void plugin_static_deinit(void);

void plugin_create(plugin_s *plug, bpbxsyn_synth_type_e type);
bool plugin_init(plugin_s *plug);
void plugin_destroy(plugin_s *plug);

bool plugin_activate(plugin_s *plug, double sample_rate,
                     uint32_t min_frames_count, uint32_t max_frames_count);
bool plugin_deactivate(plugin_s *plug);

void plugin_process_gui_events(plugin_s *plug,
                               const clap_output_events_t *out_events);

void plugin_process_transport(plugin_s *plug, const clap_event_transport_t *ev);
void plugin_process_event(plugin_s *plug, const clap_event_header_t *hdr,
                          const clap_output_events_t *out_events);

clap_process_status plugin_process(plugin_s *plug,
                                   const clap_process_t *process);

uint32_t plugin_params_count(const plugin_s *plug);
bool plugin_params_get_info(const plugin_s *plugin, uint32_t param_index,
                            clap_param_info_t *param_info);
bool plugin_params_get_value(const plugin_s *plugin, clap_id param_id,
                             double *out_value);
bool plugin_params_set_value(plugin_s *plug, clap_id id, double value,
                             event_send_flags_e send_flags,
                             const clap_output_events_t *out_events);
bool plugin_params_value_to_text(const plugin_s *plugin, clap_id param_id,
                                 double value, char *out_buf,
                                 uint32_t out_buf_capacity);
bool plugin_params_text_to_value(const plugin_s *plugin, clap_id param_id,
                                 const char *param_value_text,
                                 double *out_value);

bool plugin_state_save(const plugin_s *plugin, const clap_ostream_t *stream);
bool plugin_state_load(plugin_s *plugin, const clap_istream_t *stream);

#endif