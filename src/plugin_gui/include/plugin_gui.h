#ifndef _plugin_gui_public_h_
#define _plugin_gui_public_h_

#include <clap/include/clap/host.h>
#include <clap/include/clap/ext/gui.h>
#include <clap/include/clap/ext/track-info.h>
#include <clap/include/clap/ext/log.h>
#include <cbeepsynth/synth/include/beepbox_synth.h>

#ifdef __cplusplus
#include <cstdint>

extern "C" {
#else
#include <stdint.h>
#endif

#define GUI_EVENT_QUEUE_SIZE 256

typedef enum {
    GUI_EVENT_PARAM_CHANGE,
    GUI_EVENT_PARAM_GESTURE_BEGIN,
    GUI_EVENT_PARAM_GESTURE_END,

    GUI_EVENT_ADD_ENVELOPE,
    GUI_EVENT_REMOVE_ENVELOPE,
    GUI_EVENT_MODIFY_ENVELOPE
} gui_event_queue_item_type_e;

typedef struct {
    gui_event_queue_item_type_e type;

    union {
        struct {
            uint32_t param_id;
            double value;
        } param_value;

        struct {
            uint32_t param_id;
        } gesture;

        struct {
            uint32_t index;
        } envelope_removal;

        struct {
            uint32_t index;
            bpbx_envelope_s envelope;
        } modify_envelope;
    };
} gui_event_queue_item_s;

typedef struct plugin_gui_s plugin_gui_s;
typedef void (*show_context_menu_f)(int x, int y, uint32_t param, void *userdata);

typedef struct {
    const char *api;
    bool is_floating;

    const clap_plugin_t *plugin;
    const clap_host_t *host;
    bpbx_synth_s *instrument;

    show_context_menu_f show_context_menu;
    void *userdata;
} gui_creation_params_s;

void gui_event_enqueue(plugin_gui_s *iface, gui_event_queue_item_s item);
bool gui_event_dequeue(plugin_gui_s *iface, gui_event_queue_item_s *item);
void gui_update_color(plugin_gui_s *gui, clap_color_t color);

bool gui_is_api_supported(const char *api, bool is_floating);
bool gui_get_preferred_api(const char **api, bool *is_floating);

plugin_gui_s* gui_create(const gui_creation_params_s *params);
void gui_destroy(plugin_gui_s *iface);
void gui_sync_state(plugin_gui_s *gui);

bool gui_get_size(const plugin_gui_s *iface, uint32_t *width, uint32_t *height);
bool gui_set_parent(plugin_gui_s *iface, const clap_window_t *window);
bool gui_set_transient(plugin_gui_s *iface, const clap_window_t *window);
void gui_suggest_title(plugin_gui_s *iface, const char *title);

bool gui_show(plugin_gui_s *iface);
bool gui_hide(plugin_gui_s *iface);

void gui_set_log_func(void (*log_func)(const clap_host_t *host, clap_log_severity severity, const char *msg), const clap_host_t *host);

#ifdef __cplusplus
}
#endif

#undef BEEPBOX

#endif