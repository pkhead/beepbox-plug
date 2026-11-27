#include <cstring>

#include "plugin.hpp"

#define REGISTER(RET, NAME, ...)                                               \
    static RET clap_##NAME(const struct clap_plugin *plugin, ##__VA_ARGS__) {  \
        bbxplug::plugin *plug = (bbxplug::plugin *)plugin->plugin_data;        \
        return plug->NAME(
#define WITH_CALL_ARGS(...) __VA_ARGS__ );                                     \
    }
#define EXTENSION(LOWER, UPPER)                                                \
    static const clap_plugin_##LOWER##_t g_plugin_##UPPER =
#define EXT_FUNC(EXT, NAME) .NAME = clap_##EXT##_##NAME
#define QUERY_EXTENSION(EXT)                                                   \
    if (!std::strcmp(id, CLAP_EXT_##EXT)) return &g_plugin_##EXT;

// clang-format off
REGISTER(uint32_t, audio_ports_count, bool is_input)
    WITH_CALL_ARGS(is_input)
REGISTER(bool, audio_ports_get, uint32_t index, bool is_input, clap_audio_port_info_t *info)
    WITH_CALL_ARGS(index, is_input, info)
REGISTER(uint32_t, note_ports_count, bool is_input)
    WITH_CALL_ARGS(is_input)
REGISTER(bool, note_ports_get, uint32_t index, bool is_input, clap_note_port_info_t *info)
    WITH_CALL_ARGS(index, is_input, info)
REGISTER(uint32_t, latency_get)
    WITH_CALL_ARGS()
REGISTER(bool, state_save, const clap_ostream_t *stream)
    WITH_CALL_ARGS(stream)
REGISTER(bool, state_load, const clap_istream_t *stream)
    WITH_CALL_ARGS(stream)
REGISTER(uint32_t, params_count)
    WITH_CALL_ARGS()
REGISTER(bool, params_get_info, uint32_t param_index, clap_param_info_t *param_info)
    WITH_CALL_ARGS(param_index, param_info)
REGISTER(bool, params_get_value, clap_id param_id, double *out_value)
    WITH_CALL_ARGS(param_id, out_value)
REGISTER(bool, params_value_to_text, clap_id param_id, double value, char *out_buffer, uint32_t out_buffer_capacity)
    WITH_CALL_ARGS(param_id, value, out_buffer, out_buffer_capacity)
REGISTER(bool, params_text_to_value, clap_id param_id, const char *param_value_text, double *out_value)
    WITH_CALL_ARGS(param_id, param_value_text, out_value)
REGISTER(void, params_flush, const clap_input_events_t *in, const clap_output_events_t *out)
    WITH_CALL_ARGS(in, out)
REGISTER(bool, gui_is_api_supported, const char *api, bool is_floating)
    WITH_CALL_ARGS(api, is_floating)
REGISTER(bool, gui_get_preferred_api, const char **api, bool *is_floating)
    WITH_CALL_ARGS(api, is_floating)
REGISTER(bool, gui_create, const char *api, bool is_floating)
    WITH_CALL_ARGS(api, is_floating)
REGISTER(void, gui_destroy)
    WITH_CALL_ARGS()
REGISTER(bool, gui_set_scale, double scale)
    WITH_CALL_ARGS(scale)
REGISTER(bool, gui_get_size, uint32_t *width, uint32_t *height)
    WITH_CALL_ARGS(width, height)
REGISTER(bool, gui_can_resize)
    WITH_CALL_ARGS()
REGISTER(bool, gui_get_resize_hints, clap_gui_resize_hints_t *hints)
    WITH_CALL_ARGS(hints)
REGISTER(bool, gui_adjust_size, uint32_t *width, uint32_t *height)
    WITH_CALL_ARGS(width, height)
REGISTER(bool, gui_set_size, uint32_t width, uint32_t height)
    WITH_CALL_ARGS(width, height)
REGISTER(bool, gui_set_parent, const clap_window_t *window)
    WITH_CALL_ARGS(window)
REGISTER(bool, gui_set_transient, const clap_window_t *window)
    WITH_CALL_ARGS(window)
REGISTER(void, gui_suggest_title, const char *title)
    WITH_CALL_ARGS(title)
REGISTER(bool, gui_show)
    WITH_CALL_ARGS()
REGISTER(bool, gui_hide)
    WITH_CALL_ARGS()
REGISTER(bool, context_menu_populate, const clap_context_menu_target_t *target, const clap_context_menu_builder_t *builder)
    WITH_CALL_ARGS(target, builder)
REGISTER(bool, context_menu_perform, const clap_context_menu_target_t *target, clap_id action_id)
    WITH_CALL_ARGS(target, action_id)
REGISTER(void, track_info_changed)
    WITH_CALL_ARGS( )

// clang-format off
EXTENSION(audio_ports, AUDIO_PORTS) {
    EXT_FUNC(audio_ports, count),
    EXT_FUNC(audio_ports, get),
};

EXTENSION(note_ports, NOTE_PORTS) {
    EXT_FUNC(note_ports, count),
    EXT_FUNC(note_ports, get),
};

EXTENSION(state, STATE) {
    EXT_FUNC(state, save),
    EXT_FUNC(state, load),
};

EXTENSION(params, PARAMS) {
    EXT_FUNC(params, count),
    EXT_FUNC(params, get_info),
    EXT_FUNC(params, get_value),
    EXT_FUNC(params, value_to_text),
    EXT_FUNC(params, text_to_value),
    EXT_FUNC(params, flush),
};

EXTENSION(gui, GUI) {
   EXT_FUNC(gui, is_api_supported),
   EXT_FUNC(gui, get_preferred_api),
   EXT_FUNC(gui, create),
   EXT_FUNC(gui, destroy),
   EXT_FUNC(gui, set_scale),
   EXT_FUNC(gui, get_size),
   EXT_FUNC(gui, can_resize),
   EXT_FUNC(gui, get_resize_hints),
   EXT_FUNC(gui, adjust_size),
   EXT_FUNC(gui, set_size),
   EXT_FUNC(gui, set_parent),
   EXT_FUNC(gui, set_transient),
   EXT_FUNC(gui, suggest_title),
   EXT_FUNC(gui, show),
   EXT_FUNC(gui, hide),
};

EXTENSION(context_menu, CONTEXT_MENU) {
   EXT_FUNC(context_menu, populate),
   EXT_FUNC(context_menu, perform),
};

EXTENSION(track_info, TRACK_INFO) {
    EXT_FUNC(track_info, changed),
};
// clang-format on

const void *bbxplug::plugin::get_extension(const char *id) {
    QUERY_EXTENSION(AUDIO_PORTS)
    QUERY_EXTENSION(NOTE_PORTS)
    // QUERY_EXTENSION(STATE)
    QUERY_EXTENSION(PARAMS)
    // QUERY_EXTENSION(GUI)
    // QUERY_EXTENSION(CONTEXT_MENU)
    // QUERY_EXTENSION(TRACK_INFO)
    return nullptr;
}