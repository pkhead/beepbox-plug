#pragma once

#include <atomic>
#include <beepbox_synth.h>
#include <clap/clap.h>
#include <cstdint>

#include "instr_device.hpp"

#ifndef _NDEBUG
#    define PLUGIN_TRACK_MEMORY
#endif

namespace bbxplug {
class plugin {
  public:
    plugin(const clap_host_t *host);
    ~plugin();

    clap_plugin_t clap_plugin;

    bool init();
    bool activate(double sample_rate, uint32_t min_frames_count,
                  uint32_t max_frames_count);
    void deactivate();
    bool start_processing();
    void stop_processing();
    void reset();
    clap_process_status process(const clap_process_t *process);
    void on_main_thread();

    //! EXT:audio_ports
    uint32_t audio_ports_count(bool is_input);
    bool audio_ports_get(uint32_t index, bool is_input,
                         clap_audio_port_info_t *info);
    //! END

    //! EXT:note_ports
    uint32_t note_ports_count(bool is_input);
    bool note_ports_get(uint32_t index, bool is_input,
                        clap_note_port_info_t *info);
    //! END

    //! EXT:latency
    uint32_t latency_get();

    //! EXT:state
    bool state_save(const clap_ostream_t *stream);
    bool state_load(const clap_istream_t *stream);
    //! END

    //! EXT:params
    uint32_t params_count();
    bool params_get_info(uint32_t param_index, clap_param_info_t *param_info);
    bool params_get_value(clap_id param_id, double *out_value);
    bool params_value_to_text(clap_id param_id, double value, char *out_buffer,
                              uint32_t out_buffer_capacity);
    bool params_text_to_value(clap_id param_id, const char *param_value_text,
                              double *out_value);
    void params_flush(const clap_input_events_t *in,
                      const clap_output_events_t *out);
    //! END

    //! EXT:gui
    bool gui_is_api_supported(const char *api, bool is_floating);
    bool gui_get_preferred_api(const char **api, bool *is_floating);
    bool gui_create(const char *api, bool is_floating);
    void gui_destroy();
    bool gui_set_scale(double scale);
    bool gui_get_size(uint32_t *width, uint32_t *height);
    bool gui_can_resize();
    bool gui_get_resize_hints(clap_gui_resize_hints_t *hints);
    bool gui_adjust_size(uint32_t *width, uint32_t *height);
    bool gui_set_size(uint32_t width, uint32_t height);
    bool gui_set_parent(const clap_window_t *window);
    bool gui_set_transient(const clap_window_t *window);
    void gui_suggest_title(const char *title);
    bool gui_show();
    bool gui_hide();
    //! END

    //! EXT:context_menu
    bool context_menu_populate(const clap_context_menu_target_t *target,
                               const clap_context_menu_builder_t *builder);
    bool context_menu_perform(const clap_context_menu_target_t *target,
                              clap_id action_id);
    //! END

    //! EXT:track_info
    void track_info_changed();
    //! END

    const void *get_extension(const char *id);

  protected:
    // plugin_gui_s *gui;
    std::atomic_bool is_active;

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

    bbxs_context_s *bbxs_ctx;
    bbxplug::instr_device *instrument;

    // main->audio CLAP event queue
    // (used when loading state)
    // internal_event_queue_s event_queue;

#ifdef PLUGIN_TRACK_MEMORY
    size_t mem_allocated;
    static void *bbxs_mem_alloc(size_t size, void *userdata);
    static void bbxs_mem_free(void *ptr, void *userdata);
    struct alloc_struct {
        size_t size;
        alignas(16) std::byte data[];
    };
#endif

    static void bpbx_log_cb(bbxs_log_severity_e severity, const char *msg,
                            void *userdata);
};
} // namespace bbxplug