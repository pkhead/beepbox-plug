#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#if __STDC_VERSION__ >= 201112L && !defined (__STDC_NO_THREADS__) && defined (CLAP_HAS_THREADS_H)
#   define CLAP_HAS_THREAD
#   include <threads.h>
#endif

#include <beepbox_synth.h>
#include <clap/clap.h>
#include <plugin_gui.h>
#include "system.h"
#include "plugin_impl.h"

#define CREATE_INSTRUMENT_PLUGIN(inst_id, inst_name, inst_desc) { \
   .clap_version = CLAP_VERSION_INIT, \
   .id = "us.pkhead.beepbox." inst_id, \
   .name = "BeepBox " inst_name, \
   .vendor = "pkhead", \
   .url = "https://github.com/pkhead/beepbox-plug", \
   .manual_url = "", \
   .support_url = "", \
   .version = PLUGIN_VERSION, \
   .description = inst_desc, \
   .features = (const char *[]){CLAP_PLUGIN_FEATURE_INSTRUMENT, CLAP_PLUGIN_FEATURE_STEREO, NULL}, \
}

static const clap_plugin_descriptor_t s_fm_plug_desc =
   CREATE_INSTRUMENT_PLUGIN("fm", "FM", "BeepBox FM synthesizer");

static const clap_plugin_descriptor_t s_chip_plug_desc =
   CREATE_INSTRUMENT_PLUGIN("chip", "Chip", "BeepBox chip wave synthesizer");

static const clap_plugin_descriptor_t s_harmonics_plug_desc =
   CREATE_INSTRUMENT_PLUGIN("harmonics", "Harmonics", "BeepBox additive synthesizer");









/////////////////////////////
// clap_plugin_audio_ports //
/////////////////////////////

static uint32_t plugin_audio_ports_count(const clap_plugin_t *plugin, bool is_input) {
   // no inputs, 1 output
   if (is_input) return 0;
   return 1;
}

static bool plugin_audio_ports_get(const clap_plugin_t *plugin, uint32_t index, bool is_input, clap_audio_port_info_t *info) {
   if (is_input || index > 0)
      return false;

   info->id = 0;
   snprintf(info->name, sizeof(info->name), "%s", "Audio Output");
   info->channel_count = 2;
   info->flags = CLAP_AUDIO_PORT_IS_MAIN;
   info->port_type = CLAP_PORT_STEREO;
   info->in_place_pair = CLAP_INVALID_ID;
   return true;
}

static const clap_plugin_audio_ports_t s_plugin_audio_ports = {
   .count = plugin_audio_ports_count,
   .get = plugin_audio_ports_get,
};

////////////////////////////
// clap_plugin_note_ports //
////////////////////////////

static uint32_t plugin_note_ports_count(const clap_plugin_t *plugin, bool is_input) {
   if (!is_input) return 0;
   return 1;
}

static bool plugin_note_ports_get(const clap_plugin_t *plugin, uint32_t index, bool is_input, clap_note_port_info_t *info) {
   if (!is_input || index > 0)
      return false;

   info->id = 0;
   snprintf(info->name, sizeof(info->name), "%s", "Note port");

   info->supported_dialects =
      CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI;
   info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
   return true;
}

static const clap_plugin_note_ports_t s_plugin_note_ports = {
   .count = plugin_note_ports_count,
   .get = plugin_note_ports_get,
};

//////////////////
// clap_latency //
//////////////////

uint32_t plugin_latency_get(const clap_plugin_t *plugin) {
   return 0;
}

static const clap_plugin_latency_t s_plugin_latency = {
   .get = plugin_latency_get,
};

////////////////
// clap_state //
////////////////

static const clap_plugin_state_t s_plugin_state = {
   .save = plugin_state_save,
   .load = plugin_state_load,
};

/////////////////
// clap_params //
/////////////////

void plugin_params_flush(const clap_plugin_t *plugin, const clap_input_events_t *in, const clap_output_events_t *out) {
   plugin_s *plug = plugin->plugin_data;
   uint32_t size = in->size(in);

   if (plug->gui)
      process_gui_events(plug, out);

   for (uint32_t i = 0; i < size; ++i) {
      const clap_event_header_t *hdr = in->get(in, i);
      plugin_process_event(plug, hdr, out);
   }
}



static const clap_plugin_params_t s_plugin_params = {
   .count = plugin_params_count,
   .get_info = plugin_params_get_info,
   .get_value = plugin_params_get_value,
   .value_to_text = plugin_params_value_to_text,
   .text_to_value = plugin_params_text_to_value,
   .flush = plugin_params_flush
};

//////////////
// clap_gui //
//////////////

static void gui_show_context_menu(int x, int y, uint32_t param, void *userdata) {
   plugin_s *plug = (plugin_s*) userdata;

   const clap_host_context_menu_t *host_context_menu = plug->host_context_menu;
   if (!host_context_menu) return;

   if (host_context_menu->can_popup(plug->host)) {
      clap_context_menu_target_t target = {
         .kind = CLAP_CONTEXT_MENU_TARGET_KIND_PARAM,
         .id = param
      };

      host_context_menu->popup(plug->host, &target, 0, x, y);
   }
}

bool plugin_gui_is_api_supported(const clap_plugin_t *plugin, const char *api, bool is_floating) {
   return gui_is_api_supported(api, is_floating);
}

bool plugin_gui_get_preferred_api(const clap_plugin_t *plugin, const char **api, bool *is_floating) {
   return gui_get_preferred_api(api, is_floating);
}

bool plugin_gui_create(const clap_plugin_t *plugin, const char *api, bool is_floating) {
   plugin_s *plug = plugin->plugin_data;

   plug->gui = gui_create(&(gui_creation_params_s) {
      .api = api,
      .plugin = plugin,
      .host = plug->host,
      .is_floating = is_floating,
      .instrument = plug->instrument,
      .show_context_menu = gui_show_context_menu,
      .userdata = plug
   });

   if (plug->gui) {
      if (plug->has_track_color) {
         gui_update_color(plug->gui, plug->track_color);
      }
   }

   return plug->gui != NULL;
}

void plugin_gui_destroy(const clap_plugin_t *plugin) {
   plugin_s *plug = plugin->plugin_data;
   assert(plug->gui);
   gui_destroy(plug->gui);
}

bool plugin_gui_set_scale(const clap_plugin_t *plugin, double scale) {
   return false;
}

bool plugin_gui_get_size(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height) {
   plugin_s *plug = plugin->plugin_data;
   assert(plug->gui);
   return gui_get_size(plug->gui, width, height);
}

bool plugin_gui_can_resize(const clap_plugin_t *plugin) {
   return false;
}

bool plugin_gui_get_resize_hints(const clap_plugin_t *plugin, clap_gui_resize_hints_t *hints) {
   hints->can_resize_horizontally = false;
   hints->can_resize_vertically = false;
   hints->aspect_ratio_width = 2;
   hints->aspect_ratio_height = 3;
   hints->preserve_aspect_ratio = true;
   return true;
}

bool plugin_gui_adjust_size(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height) {
   return false;
}

bool plugin_gui_set_size(const clap_plugin_t *plugin, uint32_t width, uint32_t height) {
   return false;
}

bool plugin_gui_set_parent(const clap_plugin_t *plugin, const clap_window_t *window) {
   plugin_s *plug = plugin->plugin_data;
   assert(plug->gui);
   return gui_set_parent(plug->gui, window);
}

bool plugin_gui_set_transient(const clap_plugin_t *plugin, const clap_window_t *window) {
   plugin_s *plug = plugin->plugin_data;
   assert(plug->gui);
   return gui_set_transient(plug->gui, window);
}

void plugin_gui_suggest_title(const clap_plugin_t *plugin, const char *title) {
   plugin_s *plug = plugin->plugin_data;
   assert(plug->gui);
   gui_suggest_title(plug->gui, title);
}

bool plugin_gui_show(const clap_plugin_t *plugin) {
   plugin_s *plug = plugin->plugin_data;
   assert(plug->gui);
   return gui_show(plug->gui);
}

bool plugin_gui_hide(const clap_plugin_t *plugin) {
   plugin_s *plug = plugin->plugin_data;
   assert(plug->gui);
   return gui_hide(plug->gui);
}

static clap_plugin_gui_t s_plugin_gui = {
   .is_api_supported = plugin_gui_is_api_supported,
   .get_preferred_api = plugin_gui_get_preferred_api,
   .create = plugin_gui_create,
   .destroy = plugin_gui_destroy,
   .set_scale = plugin_gui_set_scale,
   .get_size = plugin_gui_get_size,
   .can_resize = plugin_gui_can_resize,
   .get_resize_hints = plugin_gui_get_resize_hints,
   .adjust_size = plugin_gui_adjust_size,
   .set_size = plugin_gui_set_size,
   .set_parent = plugin_gui_set_parent,
   .set_transient = plugin_gui_set_transient,
   .suggest_title = plugin_gui_suggest_title,
   .show = plugin_gui_show,
   .hide = plugin_gui_hide
};

///////////////////////
// clap_context_menu //
///////////////////////

bool plugin_context_menu_populate(
   const clap_plugin_t *plugin,
   const clap_context_menu_target_t *target,
   const clap_context_menu_builder_t *builder
) {
   return true;
}

bool plugin_context_menu_perform(
   const clap_plugin_t *plugin,
   const clap_context_menu_target_t *target,
   clap_id action_id
) {
   return true;
}

static clap_plugin_context_menu_t s_plugin_context_menu = {
   .populate = plugin_context_menu_populate,
   .perform = plugin_context_menu_perform
};

/////////////////////
// clap_track_info //
/////////////////////

void plugin_track_info_changed(const clap_plugin_t *plugin) {
   plugin_s *plug = plugin->plugin_data;

   clap_track_info_t track_info;

   // fetch track color info
   if ( plug->host_track_info->get(plug->host, &track_info) ) {
      plug->has_track_color = (track_info.flags & CLAP_TRACK_INFO_HAS_TRACK_COLOR) != 0;
      plug->track_color = track_info.color;
   } else {
      plug->has_track_color = false;
   }

   // update gui
   if (plug->gui) {
      if (plug->has_track_color) {
         gui_update_color(plug->gui, plug->track_color);
      } else {
         gui_update_color(plug->gui, (clap_color_t) { 0, 0, 0, 0 });
      }
   }
}

static const clap_plugin_track_info_t s_plugin_track_info = {
   .changed = plugin_track_info_changed
};

/////////////////
// clap_plugin //
/////////////////

static void bpbx_log_cb(bpbx_log_severity_e severity, const char *msg, void *userdata) {
   plugin_s *plug = (plugin_s*)userdata;
   clap_log_severity clap_sev = CLAP_LOG_INFO;
   switch (severity) {
      case BPBX_LOG_DEBUG:
         clap_sev = CLAP_LOG_DEBUG;
         break;

      case BPBX_LOG_INFO:
         clap_sev = CLAP_LOG_INFO;
         break;

      case BPBX_LOG_WARNING:
         clap_sev = CLAP_LOG_WARNING;
         break;

      case BPBX_LOG_ERROR:
         clap_sev = CLAP_LOG_ERROR;
         break;

      case BPBX_LOG_FATAL:
         clap_sev = CLAP_LOG_FATAL;
         break;
   }

   plug->host_log->log(plug->host, clap_sev, msg);
}

static bool plugin_init(const struct clap_plugin *plugin) {
   plugin_s *plug = plugin->plugin_data;

   // Fetch host's extensions here
   // Make sure to check that the interface functions are not null pointers
   plug->host_log = (const clap_host_log_t *)plug->host->get_extension(plug->host, CLAP_EXT_LOG);
   plug->host_thread_check = (const clap_host_thread_check_t *)plug->host->get_extension(plug->host, CLAP_EXT_THREAD_CHECK);
   plug->host_latency = (const clap_host_latency_t *)plug->host->get_extension(plug->host, CLAP_EXT_LATENCY);
   plug->host_state = (const clap_host_state_t *)plug->host->get_extension(plug->host, CLAP_EXT_STATE);
   plug->host_track_info = (const clap_host_track_info_t*) plug->host->get_extension(plug->host, CLAP_EXT_TRACK_INFO);
   plug->host_context_menu = (const clap_host_context_menu_t*) plug->host->get_extension(plug->host, CLAP_EXT_CONTEXT_MENU);

   plug->instrument = bpbx_synth_new(plug->inst_type);
   plugin_init_inst(plug);

   if (plug->host_track_info) {
      plugin_track_info_changed(plugin);
   }
   
   if (plug->host_log) {
      gui_set_log_func(plug->host_log->log, plug->host);
      bpbx_set_log_func(bpbx_log_cb, plug);
   }

   return true;
}

static void plugin_destroy(const struct clap_plugin *plugin) {
   plugin_s *plug = plugin->plugin_data;

   if (plug->instrument)
      bpbx_synth_destroy(plug->instrument);

   free(plug);
}

static bool plugin_activate(const struct clap_plugin *plugin, double sample_rate, uint32_t min_frames_count, uint32_t max_frames_count) {
   plugin_s *plug = plugin->plugin_data;
   
   plug->sample_rate = sample_rate;
   bpbx_synth_set_sample_rate(plug->instrument, plug->sample_rate);

   free(plug->process_block);
   plug->process_block = malloc(max_frames_count * sizeof(float));

   return true;
}

static void plugin_deactivate(const struct clap_plugin *plugin) {
   plugin_s *plug = plugin->plugin_data;
   free(plug->process_block);
   plug->process_block = NULL;
}

static bool plugin_start_processing(const struct clap_plugin *plugin) { return true; }

static void plugin_stop_processing(const struct clap_plugin *plugin) {}

static void plugin_reset(const struct clap_plugin *plugin) {}

static const void *plugin_get_extension(const struct clap_plugin *plugin, const char *id) {
   if (!strcmp(id, CLAP_EXT_LATENCY))
      return &s_plugin_latency;

   if (!strcmp(id, CLAP_EXT_AUDIO_PORTS))
      return &s_plugin_audio_ports;

   if (!strcmp(id, CLAP_EXT_NOTE_PORTS))
      return &s_plugin_note_ports;

   if (!strcmp(id, CLAP_EXT_STATE))
      return &s_plugin_state;

   if (!strcmp(id, CLAP_EXT_PARAMS))
      return &s_plugin_params;

   if (!strcmp(id, CLAP_EXT_GUI))
      return &s_plugin_gui;

   if (!strcmp(id, CLAP_EXT_TRACK_INFO))
      return &s_plugin_track_info;

   if (!strcmp(id, CLAP_EXT_CONTEXT_MENU))
      return &s_plugin_context_menu;

   return NULL;
}

static void plugin_on_main_thread(const struct clap_plugin *plugin) {}

clap_plugin_t *plugin_create(const clap_host_t *host, const clap_plugin_descriptor_t *desc, bpbx_synth_type_e inst_type) {
   plugin_s *p = malloc(sizeof(plugin_s));
   *p = (plugin_s) {
      .host = host,
      .plugin.desc = desc,
      .plugin.plugin_data = p,
      .plugin.init = plugin_init,
      .plugin.destroy = plugin_destroy,
      .plugin.activate = plugin_activate,
      .plugin.deactivate = plugin_deactivate,
      .plugin.start_processing = plugin_start_processing,
      .plugin.stop_processing = plugin_stop_processing,
      .plugin.reset = plugin_reset,
      .plugin.process = plugin_process,
      .plugin.get_extension = plugin_get_extension,
      .plugin.on_main_thread = plugin_on_main_thread,

      .inst_type = inst_type,
      .has_track_color = false,
      .bpm = 150.0,

      .tempo_multiplier = 1.0,
      .tempo_override = 150.0,
      .tempo_use_override = false
   };

   // Don't call into the host here

   return &p->plugin;
}

clap_plugin_t *plugin_create_fm(const clap_host_t *host) {
   return plugin_create(host, &s_fm_plug_desc, BPBX_INSTRUMENT_FM);
}

clap_plugin_t *plugin_create_chip(const clap_host_t *host) {
   return plugin_create(host, &s_chip_plug_desc, BPBX_INSTRUMENT_CHIP);
}

clap_plugin_t *plugin_create_harmonics(const clap_host_t *host) {
   return plugin_create(host, &s_harmonics_plug_desc, BPBX_INSTRUMENT_HARMONICS);
}

/////////////////////////
// clap_plugin_factory //
/////////////////////////

static struct {
   const clap_plugin_descriptor_t *desc;
   clap_plugin_t *(CLAP_ABI *create)(const clap_host_t *host);
} s_plugins[] = {
   {
      .desc = &s_fm_plug_desc,
      .create = plugin_create_fm,
   },
   {
      .desc = &s_chip_plug_desc,
      .create = plugin_create_chip,
   },
   {
      .desc = &s_harmonics_plug_desc,
      .create = plugin_create_harmonics
   }
};

static uint32_t plugin_factory_get_plugin_count(const struct clap_plugin_factory *factory) {
   return sizeof(s_plugins) / sizeof(s_plugins[0]);
}

static const clap_plugin_descriptor_t *
plugin_factory_get_plugin_descriptor(const struct clap_plugin_factory *factory, uint32_t index) {
   return s_plugins[index].desc;
}

static const clap_plugin_t *plugin_factory_create_plugin(const struct clap_plugin_factory *factory,
                                                         const clap_host_t                *host,
                                                         const char *plugin_id) {
   if (!clap_version_is_compatible(host->clap_version)) {
      return NULL;
   }

   const int N = sizeof(s_plugins) / sizeof(s_plugins[0]);
   for (int i = 0; i < N; ++i)
      if (!strcmp(plugin_id, s_plugins[i].desc->id))
         return s_plugins[i].create(host);

   return NULL;
}

static const clap_plugin_factory_t s_plugin_factory = {
   .get_plugin_count = plugin_factory_get_plugin_count,
   .get_plugin_descriptor = plugin_factory_get_plugin_descriptor,
   .create_plugin = plugin_factory_create_plugin,
};

////////////////
// clap_entry //
////////////////

static bool entry_init(const char *plugin_path) {
   // perform the plugin initialization
   return true;
}

static void entry_deinit(void) {
   // perform the plugin de-initialization
}

#ifdef CLAP_HAS_THREAD
static mtx_t g_entry_lock;
static once_flag g_entry_once = ONCE_FLAG_INIT;
#endif

static int g_entry_init_counter = 0;

#ifdef CLAP_HAS_THREAD
// Initializes the necessary mutex for the entry guard
static void entry_init_guard_init(void) {
   mtx_init(&g_entry_lock, mtx_plain);
}
#endif

// Thread safe init counter
static bool entry_init_guard(const char *plugin_path) {
#ifdef CLAP_HAS_THREAD
   call_once(&g_entry_once, entry_init_guard_init);

   mtx_lock(&g_entry_lock);
#endif

   const int cnt = ++g_entry_init_counter;
   assert(cnt > 0);

   bool succeed = true;
   if (cnt == 1) {
      succeed = entry_init(plugin_path);
      if (!succeed)
         g_entry_init_counter = 0;
   }

#ifdef CLAP_HAS_THREAD
   mtx_unlock(&g_entry_lock);
#endif

   return succeed;
}

// Thread safe deinit counter
static void entry_deinit_guard(void) {
#ifdef CLAP_HAS_THREAD
   call_once(&g_entry_once, entry_init_guard_init);

   mtx_lock(&g_entry_lock);
#endif

   const int cnt = --g_entry_init_counter;
   assert(cnt >= 0);

   bool succeed = true;
   if (cnt == 0)
      entry_deinit();

#ifdef CLAP_HAS_THREAD
   mtx_unlock(&g_entry_lock);
#endif
}

static const void *entry_get_factory(const char *factory_id) {
#ifdef CLAP_HAS_THREAD
   call_once(&g_entry_once, entry_init_guard_init);
#endif

   assert(g_entry_init_counter > 0);
   if (g_entry_init_counter <= 0)
      return NULL;

   if (!strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID))
      return &s_plugin_factory;
   return NULL;
}

// This symbol will be resolved by the host
CLAP_EXPORT const clap_plugin_entry_t clap_entry = {
   .clap_version = CLAP_VERSION_INIT,
   .init = entry_init_guard,
   .deinit = entry_deinit_guard,
   .get_factory = entry_get_factory,
};