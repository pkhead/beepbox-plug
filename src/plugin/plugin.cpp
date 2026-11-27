#include "plugin.hpp"

#include <beepbox_synth.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _MSC_VER
#    define strncpy(DEST, SRC, LEN) strcpy_s(DEST, LEN, SRC)
#endif

using namespace bbxplug;

plugin::plugin(const clap_host_t *host) : host(host) {}

bool plugin::init() {
    // Fetch host's extensions here
    // Make sure to check that the interface functions are not null pointers
    host_log = (const clap_host_log_t *)host->get_extension(host, CLAP_EXT_LOG);
    host_thread_check = (const clap_host_thread_check_t *)host->get_extension(
        host, CLAP_EXT_THREAD_CHECK);
    host_latency = (const clap_host_latency_t *)host->get_extension(
        host, CLAP_EXT_LATENCY);
    host_state =
        (const clap_host_state_t *)host->get_extension(host, CLAP_EXT_STATE);
    host_params =
        (const clap_host_params_t *)host->get_extension(host, CLAP_EXT_PARAMS);
    host_track_info = (const clap_host_track_info_t *)host->get_extension(
        host, CLAP_EXT_TRACK_INFO);
    host_context_menu = (const clap_host_context_menu_t *)host->get_extension(
        host, CLAP_EXT_CONTEXT_MENU);

#ifdef PLUGIN_TRACK_MEMORY
    bbxs_allocator_s alloc{};
    alloc.alloc = bbxs_mem_alloc;
    alloc.free = bbxs_mem_free;
    alloc.userdata = this;

    bbxs_ctx = bbxs_context_new(&alloc, 0xdeadbeef);
#else
    plug->ctx = bbxs_context_new(NULL, 0xdeadbeef);
#endif

    if (!bbxs_ctx) return false;

    instrument = new instr_device(bbxs_ctx);
    instrument->init(BBXS_SYNTH_CHIP, true);

    instrument->clap_host = host;
    instrument->clap_host_params = host_params;

    // if (plug->host_track_info) {
    //     plugin_track_info_changed(plug);
    // }

    if (host_log) {
        // gui_set_log_func(plug->host_log->log, plug->host);
        bbxs_set_log_func(bbxs_ctx, bpbx_log_cb, this);
    }

    return true;
}

plugin::~plugin() {
    delete instrument;
    bbxs_context_destroy(bbxs_ctx);
}

bool plugin::activate(double sample_rate, uint32_t min_frames_count,
                  uint32_t max_frames_count) {
    return instrument->activate(sample_rate, max_frames_count);
}

void plugin::deactivate() {
    instrument->deactivate();
}

bool plugin::start_processing() {
    return true;
}

void plugin::stop_processing() {}

void plugin::reset() {
    // TODO: implement clap reset
}

clap_process_status plugin::process(const clap_process_t *process) {
    return CLAP_PROCESS_ERROR;
}

void plugin::on_main_thread() {
    instrument->on_main_thread();
}

//------------------------------------------------------------------------------
// audio ports
//------------------------------------------------------------------------------
uint32_t plugin::audio_ports_count(bool is_input) {
    // no inputs, 1 output
    if (is_input) return 0;
    return 1;
}

bool plugin::audio_ports_get(uint32_t index, bool is_input,
                             clap_audio_port_info_t *info) {
    if (is_input || index > 0) return false;

    info->id = 0;
    std::snprintf(info->name, sizeof(info->name), "%s", "Audio Output");
    info->channel_count = 2;
    info->flags = CLAP_AUDIO_PORT_IS_MAIN;
    info->port_type = CLAP_PORT_STEREO;
    info->in_place_pair = CLAP_INVALID_ID;
    return true;
}

//------------------------------------------------------------------------------
// note ports
//------------------------------------------------------------------------------
uint32_t plugin::note_ports_count(bool is_input) {
    if (!is_input) return 0;
    return 1;
}

bool plugin::note_ports_get(uint32_t index, bool is_input,
                            clap_note_port_info_t *info) {
    if (!is_input || index > 0) return false;

    info->id = 0;
    std::snprintf(info->name, sizeof(info->name), "%s", "Note port");

    info->supported_dialects = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI;
    info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
    return true;
}

//------------------------------------------------------------------------------
// latency
//------------------------------------------------------------------------------
uint32_t plugin::latency_get() { return 0; }

//------------------------------------------------------------------------------
// state
//------------------------------------------------------------------------------
bool plugin::state_save(const clap_ostream_t *stream) { return false; }

bool plugin::state_load(const clap_istream_t *stream) { return false; }

//------------------------------------------------------------------------------
// params
//------------------------------------------------------------------------------

uint32_t plugin::params_count() { return instrument->param_count(); }

bool plugin::params_get_info(uint32_t param_index,
                             clap_param_info_t *clap_param_info) {
    param_id pid = instrument->get_param_id(param_index);
    if (pid == INVALID_PARAMETER_ID) return false;

    param_info info = instrument->get_param_info(pid);
    clap_param_info->id = (clap_id)pid;

    if (!info.active) {
        clap_param_info->cookie = NULL;
        strncpy(clap_param_info->name, "-", 256);
        strncpy(clap_param_info->module, "", 1024);
        clap_param_info->default_value = 0.0;
        clap_param_info->min_value = 0.0;
        clap_param_info->max_value = 1.0;
        clap_param_info->flags = CLAP_PARAM_IS_HIDDEN;
        return true;
    }

    const bbxs_param_info_s *bbxs_info = info.info;

    const char *module_name = "";
    if (bbxs_info->group) {
        module_name = bbxs_info->group;
    }

    clap_param_info->cookie = info.cookie;
    strncpy(clap_param_info->name, bbxs_info->name, 256);
    strncpy(clap_param_info->module, module_name, 1024);
    clap_param_info->default_value = bbxs_info->default_value;
    clap_param_info->min_value = bbxs_info->min_value;
    clap_param_info->max_value = bbxs_info->max_value;

    clap_param_info->flags = CLAP_PARAM_IS_AUTOMATABLE;
    if (bbxs_info->type == BBXS_PARAM_UINT8 ||
        bbxs_info->type == BBXS_PARAM_INT) {
        clap_param_info->flags |= CLAP_PARAM_IS_STEPPED;

        if (bbxs_info->enum_values)
            clap_param_info->flags |= CLAP_PARAM_IS_ENUM;
    }

    // ignore this property i suppose ...
    // if (!info->no_modulation)
    //    param_info->flags |= CLAP_PARAM_IS_AUTOMATABLE;

    return true;
}

bool plugin::params_get_value(clap_id id, double *out_value) {
    return instrument->get_param((param_id)id, out_value, nullptr);
}

bool plugin::params_value_to_text(clap_id param_id, double value,
                                  char *out_buffer,
                                  uint32_t out_buffer_capacity) {
    param_info pdata = instrument->get_param_info(param_id);
    const bbxs_param_info_s *info = pdata.info;
    if (!info) return false;

    switch (info->type) {
    case BBXS_PARAM_UINT8:
    case BBXS_PARAM_INT: {
        if (info->enum_values) {
            snprintf(out_buffer, out_buffer_capacity, "%s",
                     info->enum_values[(int)value]);
        } else {
            snprintf(out_buffer, out_buffer_capacity, "%i", (int)value);
        }

        break;
    }

    case BBXS_PARAM_DOUBLE: {
        snprintf(out_buffer, out_buffer_capacity, "%.1f", value);
        break;
    }

    default:
        return false;
    }

    return true;
}

bool plugin::params_text_to_value(clap_id param_id,
                                  const char *param_value_text,
                                  double *out_value) {
    param_info pdata = instrument->get_param_info(param_id);
    const bbxs_param_info_s *info = pdata.info;
    if (!info) return false;

    switch (info->type) {
    case BBXS_PARAM_UINT8:
    case BBXS_PARAM_INT:
        if (info->enum_values) {
            int end_value = (int)info->max_value;
            for (int i = (int)info->min_value; i <= end_value; ++i) {
                if (!strcmp(param_value_text, info->enum_values[i])) {
                    *out_value = (double)i;
                    return true;
                }
            }

            *out_value = 0.0;
            return false;
        } else {
            *out_value = (double)atoi(param_value_text);
        }

        break;

    case BBXS_PARAM_DOUBLE: {
        *out_value = atof(param_value_text);
        break;
    }

    default:
        return false;
    }

    return true;
}

void plugin::params_flush(const clap_input_events_t *in,
                          const clap_output_events_t *out) {
    // TODO: plugin::params_flush
}

//------------------------------------------------------------------------------
// context menu
//------------------------------------------------------------------------------
bool plugin::context_menu_populate(const clap_context_menu_target_t *target,
                                   const clap_context_menu_builder_t *builder) {
    return true;
}

bool plugin::context_menu_perform(const clap_context_menu_target_t *target,
                                  clap_id action_id) {
    return true;
}

//------------------------------------------------------------------------------

#ifdef PLUGIN_TRACK_MEMORY
void *plugin::bbxs_mem_alloc(size_t size, void *userdata) {
    plugin *self = (plugin *)userdata;
    alloc_struct *alloc =
        (alloc_struct *)std::malloc(sizeof(alloc_struct) + size);
    if (!alloc) return NULL;

    self->mem_allocated += size;
    alloc->size = size;
    return (void *)alloc->data;
}

void plugin::bbxs_mem_free(void *ptr, void *userdata) {
    plugin *self = (plugin *)userdata;
    if (!ptr) return;

    alloc_struct *alloc =
        (alloc_struct *)((uint8_t *)ptr - offsetof(alloc_struct, data));
    self->mem_allocated -= alloc->size;
    std::free(alloc);
}
#endif

void plugin::bpbx_log_cb(bbxs_log_severity_e severity, const char *msg,
                         void *userdata) {
    plugin *plug = (plugin *)userdata;
    clap_log_severity clap_sev = CLAP_LOG_INFO;
    switch (severity) {
    case BBXS_LOG_DEBUG:
        clap_sev = CLAP_LOG_DEBUG;
        break;

    case BBXS_LOG_INFO:
        clap_sev = CLAP_LOG_INFO;
        break;

    case BBXS_LOG_WARNING:
        clap_sev = CLAP_LOG_WARNING;
        break;

    case BBXS_LOG_ERROR:
        clap_sev = CLAP_LOG_ERROR;
        break;

    case BBXS_LOG_FATAL:
        clap_sev = CLAP_LOG_FATAL;
        break;
    }

    plug->host_log->log(plug->host, clap_sev, msg);
}