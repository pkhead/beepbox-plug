#include "plugin.hpp"

#include <beepbox_synth.h>
#include <cstdio>
#include <cstdlib>


bbxplug::plugin::plugin(const clap_host_t *host) : host(host) {}

bool bbxplug::plugin::init() {
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
    bbxs_allocator_s alloc {};
    alloc.alloc = bbxs_mem_alloc;
    alloc.free = bbxs_mem_free;
    alloc.userdata = this;

    bbxs_ctx = bbxs_context_new(&alloc, 0xdeadbeef);
#else
    plug->ctx = bbxs_context_new(NULL, 0xdeadbeef);
#endif

    if (!bbxs_ctx) return false;

    /*
    if (!instr_init(&plug->instrument, plug->ctx, plug->instrument.type))
        return false;

    plug->instrument.clap_host = plug->host;
    plug->instrument.clap_host_params = plug->host_params;

    if (plug->host_track_info) {
        plugin_track_info_changed(plug);
    }

    if (plug->host_log) {
        gui_set_log_func(plug->host_log->log, plug->host);
        bbxs_set_log_func(plug->ctx, bpbx_log_cb, plug);
    }
    */

    return true;
}

bbxplug::plugin::~plugin() {
    // instr_destroy(&plug->instrument);
    bbxs_context_destroy(bbxs_ctx);
}


//------------------------------------------------------------------------------
// audio ports
//------------------------------------------------------------------------------
uint32_t bbxplug::plugin::audio_ports_count(bool is_input) {
    // no inputs, 1 output
    if (is_input) return 0;
    return 1;
}

bool bbxplug::plugin::audio_ports_get(uint32_t index, bool is_input,
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
uint32_t bbxplug::plugin::note_ports_count(bool is_input) {
    if (!is_input) return 0;
    return 1;
}

bool bbxplug::plugin::note_ports_get(uint32_t index, bool is_input,
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
uint32_t bbxplug::plugin::latency_get() { return 0; }

//------------------------------------------------------------------------------
// context menu
//------------------------------------------------------------------------------
bool bbxplug::plugin::context_menu_populate(const clap_context_menu_target_t *target,
                                   const clap_context_menu_builder_t *builder) {
    return true;
}

bool bbxplug::plugin::context_menu_perform(const clap_context_menu_target_t *target,
                                  clap_id action_id) {
    return true;
}

//------------------------------------------------------------------------------

#ifdef PLUGIN_TRACK_MEMORY
void *bbxplug::plugin::bbxs_mem_alloc(size_t size, void *userdata) {
    bbxplug::plugin *self = (bbxplug::plugin *)userdata;
    alloc_struct *alloc = (alloc_struct *)std::malloc(sizeof(alloc_struct) + size);
    if (!alloc) return NULL;

    self->mem_allocated += size;
    alloc->size = size;
    return (void *)alloc->data;
}

void bbxplug::plugin::bbxs_mem_free(void *ptr, void *userdata) {
    bbxplug::plugin *self = (bbxplug::plugin *)userdata;
    if (!ptr) return;

    alloc_struct *alloc =
        (alloc_struct *)((uint8_t *)ptr - offsetof(alloc_struct, data));
    self->mem_allocated -= alloc->size;
    std::free(alloc);
}
#endif