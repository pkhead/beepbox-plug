#ifndef _bpbxclap_plugin_impl_h_
#define _bpbxclap_plugin_impl_h_

#include <beepbox_synth.h>
#include <clap/clap.h>
#include "include/clap_plugin.h"
#include <plugin_gui.h>

typedef struct {
   bool active;

   int32_t note_id;
   int16_t port_index;
   int16_t channel;
   int16_t key;
} plugin_voice_s;

typedef struct {
   clap_plugin_t plugin;
   plugin_gui_s *gui;

   bool has_track_color;
   clap_color_t track_color;

   const clap_host_t *host;
   const clap_host_latency_t *host_latency;
   const clap_host_log_t *host_log;
   const clap_host_thread_check_t *host_thread_check;
   const clap_host_state_t *host_state;
   const clap_host_track_info_t *host_track_info;
   const clap_host_context_menu_t *host_context_menu;
   
   bpbx_inst_type_e inst_type;
   bpbx_inst_s *instrument;
   uint32_t frames_until_next_tick;
   float *process_block;

   double sample_rate;
   double bpm;
   double cur_beat;
   bool is_playing;

   double gain;
   bool tempo_use_override;
   double tempo_multiplier;
   double tempo_override;

   // derived from gain property
   double linear_gain;

   // tracked voices
   plugin_voice_s voices[BPBX_INST_MAX_VOICES];
} plugin_s;

typedef struct {
   plugin_s *plugin;
   uint32_t cur_sample;
   const clap_output_events_t *out_events;
} inst_process_userdata_s;

typedef enum {
   SEND_TO_GUI = 1,
   SEND_TO_HOST = 2,
   NO_RECURSION = 4,
} event_send_flags_e;

typedef struct {
    const char *name;
    const char serialized_id[8];
    double min_value;
    double max_value;
    double default_value;
    uint32_t flags;
} plugin_control_param_info_s;

plugin_control_param_info_s plugin_control_param_info[PLUGIN_CPARAM_COUNT];

void plugin_init_inst(plugin_s *plug);

void process_gui_events(plugin_s *plug, const clap_output_events_t *out_events);

void plugin_process_transport(plugin_s *plug, const clap_event_transport_t *ev);
void plugin_process_event(plugin_s *plug, const clap_event_header_t *hdr, const clap_output_events_t *out_events);
clap_process_status plugin_process(const struct clap_plugin *plugin, const clap_process_t *process);

bool plugin_params_set_value(plugin_s *plug, clap_id id, double value, event_send_flags_e send_flags, const clap_output_events_t *out_events);

bool plugin_params_value_to_text(
      const clap_plugin_t *plugin,
      clap_id param_id, double value,
      char *out_buf, uint32_t out_buf_capacity
);
bool plugin_params_text_to_value(const clap_plugin_t *plugin, clap_id param_id, const char *param_value_text, double *out_value);

bool plugin_state_save(const clap_plugin_t *plugin, const clap_ostream_t *stream);
bool plugin_state_load(const clap_plugin_t *plugin, const clap_istream_t *stream);

#ifdef __cplusplus
}
#endif

#endif