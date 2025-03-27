#ifndef _plugin_gui_public_h_
#define _plugin_gui_public_h_

#include <synth/include/beepbox_synth.h>
#include <clap/include/clap/ext/gui.h>

#ifdef __cplusplus
#include <cstdint>
using namespace beepbox;
extern "C" {
#else
#include <stdint.h>
#endif

typedef struct gui_data_s gui_data_s;

#define GUI_EVENT_QUEUE_SIZE 256

typedef enum {
    GUI_EVENT_PARAM_CHANGE,
    GUI_EVENT_PARAM_GESTURE_BEGIN,
    GUI_EVENT_PARAM_GESTURE_END,
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
    };
} gui_event_queue_item_s;

typedef struct gui_plugin_interface_s gui_plugin_interface_s;

void gui_event_enqueue(gui_plugin_interface_s *iface, gui_event_queue_item_s item);
bool gui_event_dequeue(gui_plugin_interface_s *iface, gui_event_queue_item_s *item);

bool gui_is_api_supported(const char *api, bool is_floating);
bool gui_get_preferred_api(const char **api, bool *is_floating);

gui_plugin_interface_s* gui_create(inst_s *instrument, const char *api, bool is_floating);
void gui_destroy(gui_plugin_interface_s *iface);

bool gui_get_size(const gui_plugin_interface_s *iface, uint32_t *width, uint32_t *height);
bool gui_set_parent(gui_plugin_interface_s *iface, const clap_window_t *window);
bool gui_set_transient(gui_plugin_interface_s *iface, const clap_window_t *window);
void gui_suggest_title(gui_plugin_interface_s *iface, const char *title);

bool gui_show(gui_plugin_interface_s *iface);
bool gui_hide(gui_plugin_interface_s *iface);

#ifdef __cplusplus
}
#endif

#endif