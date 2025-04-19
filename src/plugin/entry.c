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

// Apparently denormals aren't a problem on ARM & M1?
// https://en.wikipedia.org/wiki/Subnormal_number
// https://www.kvraudio.com/forum/viewtopic.php?t=575799
#if __arm64__
#define fp_env int
static inline fp_env disable_denormals() {};
static inline void enable_denormals(const fp_env *env) {};
#elif defined(_WIN32)
#include <immintrin.h>
#define fp_env uint8_t

static inline fp_env disable_denormals() {
   const uint32_t mask = _MM_DENORMALS_ZERO_MASK | _MM_FLUSH_ZERO_MASK;
   const uint32_t state = _mm_getcsr();
   _mm_setcsr(state & ~mask);
   return state & mask;
}

static inline void enable_denormals(const fp_env env) {
   const uint32_t mask = _MM_DENORMALS_ZERO_MASK | _MM_FLUSH_ZERO_MASK;
   _mm_setcsr((_mm_getcsr() & ~mask) | env);
}

#else
#include <fenv.h>
#define fp_env fenv_t;

static inline fp_env disable_denormals() {
   fenv_t env;
   fegetenv(&env);
   fesetenv(FE_DFL_DISABLE_SSE_DENORMS_ENV);
   return env;
}

static inline void enable_denormals(const fp_env env) {
   fesetenv(&env);
}
#endif

#include <beepbox_synth.h>
#include <clap/clap.h>
#include <plugin_gui.h>
#include "endianness.h"

static const clap_plugin_descriptor_t s_my_plug_desc = {
   .clap_version = CLAP_VERSION_INIT,
   .id = "us.pkhead.beepbox",
   .name = "BeepBox",
   .vendor = "pkhead",
   .url = "https://github.com/pkhead/beepbox-plug",
   .manual_url = "",
   .support_url = "",
   .version = "0.0.1",
   .description = "Port of the BeepBox synthesizers.",
   .features = (const char *[]){CLAP_PLUGIN_FEATURE_INSTRUMENT, CLAP_PLUGIN_FEATURE_STEREO, NULL},
};

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
   
   bpbx_inst_s *instrument;
   float *process_block;

   double sample_rate;
   double bpm;
   double cur_beat;
   bool is_playing;
} plugin_s;

static bool plugin_set_param(plugin_s *plug, int id, double value) {
   bpbx_inst_type_e type = bpbx_inst_type(plug->instrument);
   const bpbx_inst_param_info_s *info = bpbx_param_info(type, id);
   if (!info) return false;
   
   switch (info->type) {
      case BPBX_PARAM_UINT8:
      case BPBX_PARAM_INT:
         bpbx_inst_set_param_int(plug->instrument, id, (int)value);
         break;

      case BPBX_PARAM_DOUBLE:
         bpbx_inst_set_param_double(plug->instrument, id, value);
         break;
   }
   
   return true;
}

static void process_gui_events(plugin_s *plug, const clap_output_events_t *out_events) {
   assert(plug->gui);
   
   gui_event_queue_item_s item;
   while (gui_event_dequeue(plug->gui, &item)) {
      switch (item.type) {
         case GUI_EVENT_PARAM_CHANGE: {
            plugin_set_param(plug, item.param_value.param_id, item.param_value.value);

            clap_event_param_value_t out_ev = {
               .header.space_id = CLAP_CORE_EVENT_SPACE_ID,
               .header.size = sizeof(clap_event_param_value_t),
               .header.type = CLAP_EVENT_PARAM_VALUE,
               .header.time = 0,

               .param_id = item.gesture.param_id,
               .value = item.param_value.value,
               .cookie = NULL,

               .note_id = -1,
               .port_index = -1,
               .channel = -1,
               .key = -1,
            };

            out_events->try_push(out_events, (clap_event_header_t*)&out_ev);
            break;
         }
         
         case GUI_EVENT_PARAM_GESTURE_BEGIN: {
            clap_event_param_gesture_t out_ev = {
               .header.space_id = CLAP_CORE_EVENT_SPACE_ID,
               .header.size = sizeof(clap_event_param_gesture_t),
               .header.type = CLAP_EVENT_PARAM_GESTURE_BEGIN,
               .header.time = 0,
               
               .param_id = item.gesture.param_id
            };

            out_events->try_push(out_events, (clap_event_header_t*)&out_ev);
            break;
         }

         case GUI_EVENT_PARAM_GESTURE_END: {
            clap_event_param_gesture_t out_ev = {
               .header.space_id = CLAP_CORE_EVENT_SPACE_ID,
               .header.size = sizeof(clap_event_param_gesture_t),
               .header.type = CLAP_EVENT_PARAM_GESTURE_END,
               .header.time = 0,
               
               .param_id = item.gesture.param_id
            };

            out_events->try_push(out_events, (clap_event_header_t*)&out_ev);
            break;
         }

         case GUI_EVENT_ADD_ENVELOPE:
            bpbx_inst_add_envelope(plug->instrument);
            break;

         case GUI_EVENT_MODIFY_ENVELOPE:
            *bpbx_inst_get_envelope(plug->instrument, item.modify_envelope.index) = item.modify_envelope.envelope;
            break;
         
         case GUI_EVENT_REMOVE_ENVELOPE:
            bpbx_inst_remove_envelope(plug->instrument, item.envelope_removal.index);
            break;

         default:
            break;
      }
   }
}

static void plugin_process_transport(plugin_s *plug, const clap_event_transport_t *ev) {
   if (ev->flags & CLAP_TRANSPORT_HAS_TEMPO) {
      plug->bpm = ev->tempo;
   } else {
      plug->bpm = 150.0;
   }

   bool is_playing = (ev->flags & CLAP_TRANSPORT_IS_PLAYING) != 0;

   // if playing state changed, update current beat from transport event.
   // otherwise plugin should increment cur beat on its own using bpm info.
   if (is_playing != plug->is_playing) {
      if (ev->flags & CLAP_TRANSPORT_HAS_BEATS_TIMELINE) {
         int64_t beats_int = ev->song_pos_beats / CLAP_BEATTIME_FACTOR;
         int64_t beats_frac = ev->song_pos_beats % CLAP_BEATTIME_FACTOR;
         plug->cur_beat = (double)beats_int + (double)beats_frac / CLAP_BEATTIME_FACTOR;
      }
   }

   plug->is_playing = is_playing;
}

static void plugin_process_event(plugin_s *plug, const clap_event_header_t *hdr) {
   if (hdr->space_id == CLAP_CORE_EVENT_SPACE_ID) {
      switch (hdr->type) {
      case CLAP_EVENT_NOTE_ON: {
         const clap_event_note_t *ev = (const clap_event_note_t *)hdr;

         bpbx_inst_midi_on(plug->instrument, ev->key, (int)(ev->velocity * 127.0));
         
         break;
      }

      case CLAP_EVENT_NOTE_OFF: {
         const clap_event_note_t *ev = (const clap_event_note_t *)hdr;

         bpbx_inst_midi_off(plug->instrument, ev->key, 0);
         
         break;
      }

      case CLAP_EVENT_NOTE_CHOKE: {
         const clap_event_note_t *ev = (const clap_event_note_t *)hdr;
         // TODO: handle note choke
         break;
      }

      case CLAP_EVENT_NOTE_EXPRESSION: {
         const clap_event_note_expression_t *ev = (const clap_event_note_expression_t *)hdr;
         // TODO: handle note expression
         break;
      }

      case CLAP_EVENT_PARAM_VALUE: {
         const clap_event_param_value_t *ev = (const clap_event_param_value_t *)hdr;
         
         plugin_set_param(plug, ev->param_id, ev->value);

         if (plug->gui) {
            gui_event_queue_item_s item = {
               .type = GUI_EVENT_PARAM_CHANGE,
               .param_value.param_id = ev->param_id,
               .param_value.value = ev->value
            };
            gui_event_enqueue(plug->gui, item);
         }
         break;
      }

      case CLAP_EVENT_PARAM_MOD: {
         const clap_event_param_mod_t *ev = (const clap_event_param_mod_t *)hdr;
         // TODO: handle parameter modulation
         break;
      }

      case CLAP_EVENT_TRANSPORT: {
         const clap_event_transport_t *ev = (const clap_event_transport_t *)hdr;
         plugin_process_transport(plug, ev);
      }

      case CLAP_EVENT_MIDI: {
         const clap_event_midi_t *ev = (const clap_event_midi_t *)hdr;

         uint8_t status = ev->data[0] & 0xF0;

         // off
         if ((status == 0x80) || ((status == 0x90) && ev->data[2] == 0)) {
            bpbx_inst_midi_off(plug->instrument, ev->data[1], ev->data[2]);
         }
         
         // on
         else if (status == 0x90) {
            bpbx_inst_midi_on(plug->instrument, ev->data[1], ev->data[2]);
         }

         break;
      }

      case CLAP_EVENT_MIDI_SYSEX: {
         const clap_event_midi_sysex_t *ev = (const clap_event_midi_sysex_t *)hdr;
         // TODO: handle MIDI Sysex event
         break;
      }

      case CLAP_EVENT_MIDI2: {
         const clap_event_midi2_t *ev = (const clap_event_midi2_t *)hdr;
         // TODO: handle MIDI2 event
         break;
      }
      }
   }
}

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

// write primitive type in little-endian
// return -1 on error
static int64_t stream_write_prim(const clap_ostream_t *stream, const void *data, size_t data_size) {
   if (endianness() == LITTLE_ENDIAN) {
      size_t written = 0;

      uint8_t *p = (uint8_t*)data;
      while (written < data_size) {
         int64_t res = stream->write(stream, p, data_size - written);
         if (res <= 0) return -1;
         written += res;
         p += res;
      }

      assert(written == data_size);
      if (written != data_size) return false;
      return written;
   } else {
      size_t written = 0;

      for (uint8_t *p = (uint8_t*)data + data_size - 1; p >= (uint8_t*)data; p--) {
         int64_t res = stream->write(stream, p, sizeof(uint8_t));
         if (res <= 0) return -1;
         written += res;
      }

      assert(written == data_size);
      return written;
   }
}

// read primitive type in little-endian.
// return -1 on error
static int64_t stream_read_prim(const clap_istream_t *stream, void *data, size_t data_size) {
   if (endianness() == LITTLE_ENDIAN) {
      size_t read = 0;

      uint8_t *p = (uint8_t*)data;
      while (read < data_size) {
         int64_t res = stream->read(stream, p, data_size - read);
         if (res <= 0) return -1;
         read += res;
      }

      assert(read == data_size);
      if (read != data_size) return false;
      return read;
   } else {
      int64_t read = 0;

      for (uint8_t *write = (uint8_t*)data + data_size - 1; write >= (uint8_t*)data; write--) {
         int64_t res = stream->read(stream, write, sizeof(uint8_t));
         if (res == 0) break;
         if (res == -1) return -1;
         read += res;
      }

      return read;
   }
}

#define STATE_SAVE_VER 0
#define ERRCHK(v) if ((v) == -1) goto error

bool plugin_state_save(const clap_plugin_t *plugin, const clap_ostream_t *stream) {
   plugin_s *plug = plugin->plugin_data;

   // write save format revision (single number)
   uint32_t save_version = STATE_SAVE_VER;
   ERRCHK(stream_write_prim(stream, &save_version, sizeof(save_version)));

   // write synth version
   uint32_t synth_maj = BPBX_VERSION_MAJOR;
   uint32_t synth_min = BPBX_VERSION_MINOR;
   uint32_t synth_rev = BPBX_VERSION_REVISION;
   ERRCHK(stream_write_prim(stream, &synth_maj, sizeof(synth_maj)));
   ERRCHK(stream_write_prim(stream, &synth_min, sizeof(synth_min)));
   ERRCHK(stream_write_prim(stream, &synth_rev, sizeof(synth_rev)));

   // first, write parameter data
   bpbx_inst_type_e type = bpbx_inst_type(plug->instrument);

   uint8_t param_count = (uint8_t) bpbx_param_count(type);
   ERRCHK(stream_write_prim(stream, &param_count, sizeof(param_count)));

   for (unsigned int i = 0; i < param_count; i++) {
      const bpbx_inst_param_info_s *info = bpbx_param_info(type, i);
      if (info == NULL) return false;

      // store type data - redundant, but whatever
      uint8_t p_type = (uint8_t)info->type;
      ERRCHK(stream_write_prim(stream, &p_type, sizeof(p_type)));

      switch (info->type) {
         case BPBX_PARAM_UINT8: {
            int intv;
            if (bpbx_inst_get_param_int(plug->instrument, i, &intv)) return false;

            uint8_t uintv = (uint8_t)intv;
            ERRCHK(stream_write_prim(stream, &uintv, sizeof(uintv)));
            break;
         }

         case BPBX_PARAM_INT: {
            int v;
            if (bpbx_inst_get_param_int(plug->instrument, i, &v)) return false;
            ERRCHK(stream_write_prim(stream, &v, sizeof(v)));
            break;
         }

         case BPBX_PARAM_DOUBLE: {
            double v;
            if (bpbx_inst_get_param_double(plug->instrument, i, &v)) return false;
            ERRCHK(stream_write_prim(stream, &v, sizeof(v)));
            break;
         }
      }
   }

   // second, write envelope data
   uint8_t envelope_count = bpbx_inst_envelope_count(plug->instrument);
   ERRCHK(stream_write_prim(stream, &envelope_count, sizeof(envelope_count)));

   for (uint8_t i = 0; i < envelope_count; i++) {
      const bpbx_envelope_s *env = bpbx_inst_get_envelope(plug->instrument, i);
      if (env == NULL) return false;

      ERRCHK(stream_write_prim(stream, &env->index, sizeof(env->index)));
      ERRCHK(stream_write_prim(stream, &env->curve_preset, sizeof(env->curve_preset)));
   }

   return true;

   error: return false;
}

bool plugin_state_load(const clap_plugin_t *plugin, const clap_istream_t *stream) {
   plugin_s *plug = plugin->plugin_data;

   // read versions; do strict version checking for now.
   uint32_t save_version;
   ERRCHK(stream_read_prim(stream, &save_version, sizeof(save_version)));

   if (save_version != STATE_SAVE_VER) return false;

   uint32_t synth_maj;
   uint32_t synth_min;
   uint32_t synth_rev;
   ERRCHK(stream_read_prim(stream, &synth_maj, sizeof(synth_maj)));
   ERRCHK(stream_read_prim(stream, &synth_min, sizeof(synth_min)));
   ERRCHK(stream_read_prim(stream, &synth_rev, sizeof(synth_rev)));

   if (synth_maj != BPBX_VERSION_MAJOR) return false;
   if (synth_min != BPBX_VERSION_MINOR) return false;
   if (synth_rev != BPBX_VERSION_REVISION) return false;

   // read parameters
   bpbx_inst_type_e type = bpbx_inst_type(plug->instrument);

   uint8_t param_count;
   ERRCHK(stream_read_prim(stream, &param_count, sizeof(param_count)));

   if (param_count != (uint8_t)bpbx_param_count(type)) return false;

   for (unsigned int i = 0; i < param_count; i++) {
      const bpbx_inst_param_info_s *info = bpbx_param_info(type, i);
      if (info == NULL) return false;

      uint8_t p_type;
      ERRCHK(stream_read_prim(stream, &p_type, sizeof(p_type)));
      if (p_type != (uint8_t)info->type) return false;

      switch (p_type) {
         case BPBX_PARAM_UINT8: {
            uint8_t v;
            ERRCHK(stream_read_prim(stream, &v, sizeof(v)));
            bpbx_inst_set_param_int(plug->instrument, i, v);
            break;
         }

         case BPBX_PARAM_INT: {
            int v;
            ERRCHK(stream_read_prim(stream, &v, sizeof(v)));
            bpbx_inst_set_param_int(plug->instrument, i, v);
            break;
         }

         case BPBX_PARAM_DOUBLE: {
            double v;
            ERRCHK(stream_read_prim(stream, &v, sizeof(v)));
            bpbx_inst_set_param_double(plug->instrument, i, v);
            break;
         }
      }
   }

   // read envelopes
   uint8_t envelope_count;
   ERRCHK(stream_read_prim(stream, &envelope_count, sizeof(envelope_count)));

   bpbx_inst_clear_envelopes(plug->instrument);
   for (uint8_t i = 0; i < envelope_count; i++) {
      bpbx_envelope_s *env = bpbx_inst_add_envelope(plug->instrument);
      ERRCHK(stream_read_prim(stream, &env->index, sizeof(env->index)));
      ERRCHK(stream_read_prim(stream, &env->curve_preset, sizeof(env->curve_preset)));
   }

   if (plug->gui) gui_sync_state(plug->gui);
   return true;

   error:
      if (plug->gui) gui_sync_state(plug->gui);
      return false;
}
#undef ERRCHK

static const clap_plugin_state_t s_plugin_state = {
   .save = plugin_state_save,
   .load = plugin_state_load,
};

/////////////////
// clap_params //
/////////////////

uint32_t plugin_params_count(const clap_plugin_t *plugin) {
   plugin_s *plug = plugin->plugin_data;
   return (uint32_t) bpbx_param_count(bpbx_inst_type(plug->instrument));
}

bool plugin_params_get_info(const clap_plugin_t *plugin, uint32_t param_index, clap_param_info_t *param_info) {
   plugin_s *plug = plugin->plugin_data;

   const bpbx_inst_param_info_s *info = bpbx_param_info(bpbx_inst_type(plug->instrument), (unsigned int)param_index);
   if (info == NULL)
      return false;

   param_info->cookie = NULL;
   param_info->id = param_index;
   strcpy_s(param_info->name, 256, info->name);
   strcpy_s(param_info->module, 1024, "FM");
   param_info->default_value = info->default_value;
   param_info->min_value = info->min_value;
   param_info->max_value = info->max_value;

   param_info->flags = CLAP_PARAM_IS_AUTOMATABLE;
   if (info->type == BPBX_PARAM_UINT8 || info->type == BPBX_PARAM_INT) {
      param_info->flags |= CLAP_PARAM_IS_STEPPED;

      if (info->enum_values) param_info->flags |= CLAP_PARAM_IS_ENUM;
   }

   // ignore this property i suppose ...
   // if (!info->no_modulation)
   //    param_info->flags |= CLAP_PARAM_IS_AUTOMATABLE;

   return true;
}

bool plugin_params_get_value(const clap_plugin_t *plugin, clap_id param_id, double *out_value) {
   plugin_s *plug = plugin->plugin_data;

   return !bpbx_inst_get_param_double(plug->instrument, param_id, out_value);
}

bool plugin_params_value_to_text(
      const clap_plugin_t *plugin,
      clap_id param_id, double value,
      char *out_buf, uint32_t out_buf_capacity
) {
   plugin_s *plug = plugin->plugin_data;

   const bpbx_inst_param_info_s *info = bpbx_param_info(bpbx_inst_type(plug->instrument), param_id);
   if (info == NULL)
      return false;

   switch (info->type) {
      case BPBX_PARAM_UINT8:
      case BPBX_PARAM_INT: {
         if (info->enum_values) {
            snprintf(out_buf, out_buf_capacity, "%s", info->enum_values[(int)value]);
         } else {
            snprintf(out_buf, out_buf_capacity, "%i", (int)value);
         }

         break;
      }

      case BPBX_PARAM_DOUBLE: {
         snprintf(out_buf, out_buf_capacity, "%.1f", value);
         break;
      }

      default:
         return false;
   }

   return true;
}

bool plugin_params_text_to_value(const clap_plugin_t *plugin, clap_id param_id, const char *param_value_text, double *out_value) {
   plugin_s *plug = plugin->plugin_data;

   const bpbx_inst_param_info_s *info = bpbx_param_info(bpbx_inst_type(plug->instrument), param_id);
   if (info == NULL)
      return false;

   switch (info->type) {
      case BPBX_PARAM_UINT8:
      case BPBX_PARAM_INT:
         if (info->enum_values) {
            int end_value = (int)info->max_value;
            for (int i = (int)info->min_value; i <= end_value; i++) {
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

      case BPBX_PARAM_DOUBLE:
         *out_value = atof(param_value_text);
         break;

      default:
         return false;
   }

   return true;
}

void plugin_params_flush(const clap_plugin_t *plugin, const clap_input_events_t *in, const clap_output_events_t *out) {
   plugin_s *plug = plugin->plugin_data;
   uint32_t size = in->size(in);

   if (plug->gui)
      process_gui_events(plug, out);

   for (uint32_t i = 0; i < size; i++) {
      const clap_event_header_t *hdr = in->get(in, i);
      plugin_process_event(plug, hdr);
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

bool plugin_gui_is_api_supported(const clap_plugin_t *plugin, const char *api, bool is_floating) {
   return gui_is_api_supported(api, is_floating);
}

bool plugin_gui_get_preferred_api(const clap_plugin_t *plugin, const char **api, bool *is_floating) {
   return gui_get_preferred_api(api, is_floating);
}

bool plugin_gui_create(const clap_plugin_t *plugin, const char *api, bool is_floating) {
   plugin_s *plug = plugin->plugin_data;
   plug->gui = gui_create(plug->instrument, api, is_floating);

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
   return true;
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
   return true; // yep i definitely resized
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

static bool plugin_init(const struct clap_plugin *plugin) {
   plugin_s *plug = plugin->plugin_data;

   // Fetch host's extensions here
   // Make sure to check that the interface functions are not null pointers
   plug->host_log = (const clap_host_log_t *)plug->host->get_extension(plug->host, CLAP_EXT_LOG);
   plug->host_thread_check = (const clap_host_thread_check_t *)plug->host->get_extension(plug->host, CLAP_EXT_THREAD_CHECK);
   plug->host_latency = (const clap_host_latency_t *)plug->host->get_extension(plug->host, CLAP_EXT_LATENCY);
   plug->host_state = (const clap_host_state_t *)plug->host->get_extension(plug->host, CLAP_EXT_STATE);
   plug->host_track_info = (const clap_host_track_info_t*) plug->host->get_extension(plug->host, CLAP_EXT_TRACK_INFO);

   plug->instrument = bpbx_inst_new(BPBX_INSTRUMENT_FM);

   if (plug->host_track_info) {
      plugin_track_info_changed(plugin);
   }

   return true;
}

static void plugin_destroy(const struct clap_plugin *plugin) {
   plugin_s *plug = plugin->plugin_data;

   if (plug->instrument)
      bpbx_inst_destroy(plug->instrument);

   free(plug);
}

static bool plugin_activate(const struct clap_plugin *plugin, double sample_rate, uint32_t min_frames_count, uint32_t max_frames_count) {
   plugin_s *plug = plugin->plugin_data;
   
   plug->sample_rate = sample_rate;
   bpbx_inst_set_sample_rate(plug->instrument, plug->sample_rate);

   free(plug->process_block);
   plug->process_block = malloc(max_frames_count * sizeof(float) * 2);

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

static clap_process_status plugin_process(const struct clap_plugin *plugin, const clap_process_t *process) {
   fp_env env = disable_denormals();

   plugin_s *plug = plugin->plugin_data;

   if (plug->gui)
      process_gui_events(plug, process->out_events);

   if (process->transport) {
      plugin_process_transport(plug, process->transport);
   }

   const double sample_len = 1.0 / plug->sample_rate;
   const double beats_per_sec = plug->bpm / 60.0;

   const uint32_t nframes = process->frames_count;
   const uint32_t nev = process->in_events->size(process->in_events);
   uint32_t ev_index = 0;
   uint32_t next_ev_frame = nev > 0 ? 0 : nframes;

   for (uint32_t i = 0; i < nframes;) {
      /* handle every events that happrens at the frame "i" */
      while (ev_index < nev && next_ev_frame == i) {
         const clap_event_header_t *hdr = process->in_events->get(process->in_events, ev_index);
         if (hdr->time != i) {
            next_ev_frame = hdr->time;
            break;
         }

         plugin_process_event(plug, hdr);
         ++ev_index;

         if (ev_index == nev) {
            // we reached the end of the event list
            next_ev_frame = nframes;
            break;
         }
      }

      /* process every samples until the next event */
      uint32_t frame_count = next_ev_frame - i;
      bpbx_inst_run(plug->instrument, &(bpbx_run_ctx_s){
         .bpm = plug->bpm,
         .beat = plug->cur_beat,
         
         .frame_count = (size_t)frame_count,
         .out_samples = plug->process_block
      });

      uint32_t buffer_idx = 0;
      for (; i < next_ev_frame; ++i) {
         // store output samples
         process->audio_outputs[0].data32[0][i] = plug->process_block[buffer_idx++];
         process->audio_outputs[0].data32[1][i] = plug->process_block[buffer_idx++];
      }
      
      plug->cur_beat += beats_per_sec * sample_len * frame_count;
   }

   enable_denormals(env);

   return CLAP_PROCESS_CONTINUE;
}

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

   return NULL;
}

static void plugin_on_main_thread(const struct clap_plugin *plugin) {}

clap_plugin_t *plugin_create(const clap_host_t *host) {
   plugin_s *p = malloc(sizeof(plugin_s));
   *p = (plugin_s) {
      .host = host,
      .plugin.desc = &s_my_plug_desc,
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

      .has_track_color = false,

      .bpm = 150.0
   };

   // Don't call into the host here

   return &p->plugin;
}

/////////////////////////
// clap_plugin_factory //
/////////////////////////

static struct {
   const clap_plugin_descriptor_t *desc;
   clap_plugin_t *(CLAP_ABI *create)(const clap_host_t *host);
} s_plugins[] = {
   {
      .desc = &s_my_plug_desc,
      .create = plugin_create,
   },
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