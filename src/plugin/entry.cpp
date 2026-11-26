#include <beepbox_synth.h>
#include <cassert>
#include <clap/clap.h>
#include <cstring>

#include "plugin/plugin.hpp"

#define CLAP_HAS_THREADS_H

#ifdef CLAP_HAS_THREADS_H
#    define CLAP_HAS_THREAD
#    include <mutex>
#endif

const char *g_synth_plug_features[] = {CLAP_PLUGIN_FEATURE_INSTRUMENT,
                                       CLAP_PLUGIN_FEATURE_SYNTHESIZER,
                                       CLAP_PLUGIN_FEATURE_STEREO, NULL};
static const clap_plugin_descriptor_t g_synth_plug_desc = {
    CLAP_VERSION_INIT,
    /*id:*/ "us.pkhead.beepbox",
    /*name:*/ "BeepBox",
    /*vendor:*/ "pkhead",
    /*url:*/ "https://github.com/pkhead/beepbox-plug",
    /*manual_url:*/ "",
    /*support_url:*/ "",
    /*version:*/ PLUGIN_VERSION,
    /*description:*/ "BeepBox synthesizers",
    /*features:*/ g_synth_plug_features,
};

//------------------------------------------------------------------------------
// clap_plugin
//------------------------------------------------------------------------------

namespace clap_api::plugin {

static bool init(const clap_plugin_t *plugin) {
    ::plugin *plug = (::plugin *)plugin->plugin_data;
    return plug->init();
}

static void destroy(const clap_plugin_t *plugin) {
    ::plugin *plug = (::plugin *)plugin->plugin_data;
    delete plug;
}

static bool activate(const struct clap_plugin *plugin, double sample_rate,
                     uint32_t min_frames_count, uint32_t max_frames_count) {
    ::plugin *plug = (::plugin *)plugin->plugin_data;
    return plug->activate(sample_rate, min_frames_count, max_frames_count);
}

static void deactivate(const struct clap_plugin *plugin) {
    ::plugin *plug = (::plugin *)plugin->plugin_data;
    plug->deactivate();
}

static bool start_processing(const struct clap_plugin *plugin) {
    ::plugin *plug = (::plugin *)plugin->plugin_data;
    return plug->start_processing();
}

static void stop_processing(const struct clap_plugin *plugin) {
    ::plugin *plug = (::plugin *)plugin->plugin_data;
    return plug->stop_processing();
}

static void reset(const struct clap_plugin *plugin) {
    ::plugin *plug = (::plugin *)plugin->plugin_data;
    return plug->reset();
}

static clap_process_status process(const clap_plugin_t *plugin,
                                   const clap_process_t *process) {
    ::plugin *plug = (::plugin *)plugin->plugin_data;
    return plug->process(process);
}

static const void *get_extension(const struct clap_plugin *plugin,
                                 const char *id) {
    ::plugin *plug = (::plugin *)plugin->plugin_data;
    return plug->get_extension(id);
}

static void on_main_thread(const struct clap_plugin *plugin) {
    ::plugin *plug = (::plugin *)plugin->plugin_data;
    plug->on_main_thread();
}

clap_plugin_t *create(const clap_host_t *host) {
    ::plugin *data = new ::plugin(host);
    clap_plugin_t *clap_plug = &data->clap_plugin;

    clap_plug->desc = &g_synth_plug_desc;
    clap_plug->plugin_data = (void *)data;
    clap_plug->init = init;
    clap_plug->destroy = destroy;
    clap_plug->activate = activate;
    clap_plug->deactivate = deactivate;
    clap_plug->start_processing = start_processing;
    clap_plug->stop_processing = stop_processing;
    clap_plug->reset = reset;
    clap_plug->process = process;
    clap_plug->get_extension = get_extension;
    clap_plug->on_main_thread = on_main_thread;

    // Don't call into the host here
    //    plugin_create(p, BBXS_SYNTH_CHIP);
    return clap_plug;
}
} // namespace clap_api::plugin

//------------------------------------------------------------------------------
// clap_plugin_factory
//------------------------------------------------------------------------------

namespace clap_api::plugin_factory {
static struct {
    const clap_plugin_descriptor_t *desc;
    clap_plugin_t *(CLAP_ABI *create)(const clap_host_t *host);
} g_plugins[] = {
    {
        &g_synth_plug_desc,
        clap_api::plugin::create,
    },
};

static uint32_t get_plugin_count(const struct clap_plugin_factory *factory) {
    return sizeof(g_plugins) / sizeof(*g_plugins);
}

static const clap_plugin_descriptor_t *
get_plugin_descriptor(const struct clap_plugin_factory *factory,
                      uint32_t index) {
    return g_plugins[index].desc;
}

static const clap_plugin_t *
create_plugin(const struct clap_plugin_factory *factory,
              const clap_host_t *host, const char *plugin_id) {
    if (!clap_version_is_compatible(host->clap_version)) {
        return NULL;
    }

    const int N = sizeof(g_plugins) / sizeof(g_plugins[0]);
    for (int i = 0; i < N; ++i)
        if (!std::strcmp(plugin_id, g_plugins[i].desc->id))
            return g_plugins[i].create(host);

    return NULL;
}

static const clap_plugin_factory_t clap_interface = {
    get_plugin_count,
    get_plugin_descriptor,
    create_plugin,
};

} // namespace clap_api::plugin_factory

//------------------------------------------------------------------------------
// entry
//------------------------------------------------------------------------------

#ifdef CLAP_HAS_THREAD
static std::mutex g_entry_mutex;
#endif

static int g_entry_init_counter = 0;

static bool entry_init(const char *pluginPath) {
    // perform the plugin initialization
    return true;
}

static void entry_deinit(void) {
    // perform the plugin de-initialization
}

static const void *entry_get_factory(const char *factory_id) {
    assert(g_entry_init_counter > 0);
    if (g_entry_init_counter <= 0) return NULL;

    if (!std::strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID))
        return &clap_api::plugin_factory::clap_interface;
    return NULL;
}

// Thread safe init counter
static bool entry_init_guard(const char *plugin_path) {
#ifdef CLAP_HAS_THREAD
    std::lock_guard<std::mutex> lock(g_entry_mutex);
#endif

    const int cnt = ++g_entry_init_counter;
    assert(cnt > 0);

    bool succeed = true;
    if (cnt == 1) {
        succeed = entry_init(plugin_path);
        if (!succeed) g_entry_init_counter = 0;
    }

    return succeed;
}

// Thread safe deinit counter
static void entry_deinit_guard(void) {
#ifdef CLAP_HAS_THREAD
    std::lock_guard<std::mutex> lock(g_entry_mutex);
#endif

    const int cnt = --g_entry_init_counter;
    assert(cnt >= 0);

    bool succeed = true;
    if (cnt == 0) entry_deinit();
}

extern "C" {
// This symbol will be resolved by the host
CLAP_EXPORT const clap_plugin_entry_t clap_entry = {
    CLAP_VERSION_INIT,
    entry_init_guard,
    entry_deinit_guard,
    entry_get_factory,
};
}