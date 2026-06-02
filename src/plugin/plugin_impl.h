#ifndef _bpbxclap_plugin_impl_h_
#define _bpbxclap_plugin_impl_h_

#include <beepbox_synth.h>
#include <clap/clap.h>
#include "include/instrument.h"
#include "instrument_impl.h"
#include <plugin_gui.h>
#include <threads.h>

#define PLUGIN_EVENT_QUEUE_CAPACITY 512

typedef enum {
    INTERNAL_PLUGIN_EVENT_SET_PARAMETER,
    INTERNAL_PLUGIN_EVENT_RESIZE_ENVELOPES,
    INTERNAL_PLUGIN_EVENT_MODIFY_ENVELOPE,
    INTERNAL_PLUGIN_EVENT_GUI_RESYNC,
} internal_plugin_event_e;

typedef struct internal_event_queue_item {
    uint8_t type;
    union {
        struct {
            instr_param_id id;
            double value;
        } set_parameter;

        struct {
            uint8_t envelope_count;
        } resize_envelopes;

        struct {
            uint8_t envelope_index;
            bpbxsyn_envelope_compute_index_e compute_index;
            uint8_t curve_preset;
        } modify_envelope;
    };
} internal_event_queue_item_s;

typedef struct internal_event_queue {
    // write_ptr == read_ptr : empty
    // write_ptr == read_ptr - 1 : full
    volatile unsigned int write_ptr;
    volatile unsigned int read_ptr;

    volatile internal_event_queue_item_s data[PLUGIN_EVENT_QUEUE_CAPACITY];
} internal_event_queue_s;

typedef struct {
    clap_plugin_t plugin;
    plugin_gui_s *gui;
    volatile bool is_active;

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
    bool has_mcalloc_ref;

    // main->audio CLAP event queue
    // (used when loading state)
    internal_event_queue_s event_queue;

    #ifndef _NDEBUG
    size_t mem_allocated;
    #endif
} plugin_s;

typedef enum {
    SEND_TO_GUI = 1,
    SEND_TO_HOST = 2,
    NO_RECURSION = 4,
} event_send_flags_e;

struct g_bb_mcalloc {
    bpbxsyn_mcode_allocator_s data;
    mtx_t mutex;
    size_t ref_count;
} extern g_bb_mcalloc;

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
void plugin_process_internal_event(plugin_s *plug,
                                   const internal_event_queue_item_s *ev,
                                   event_send_flags_e param_send_flags,
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

bool plugin_enqueue_event(plugin_s *plugin, const internal_event_queue_item_s *event);
bool plugin_dequeue_event(plugin_s *plugin, internal_event_queue_item_s *event);

#endif