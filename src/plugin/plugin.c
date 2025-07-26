#include "plugin_impl.h"

#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "system.h"
#include "util.h"

static inline bool voice_match(const plugin_voice_s *voice, int32_t note_id, int16_t channel, int16_t port_index, int16_t key) {
   return (note_id == -1    || note_id == voice->note_id) &&
          (channel == -1    || channel == voice->channel) &&
          (port_index == -1 || port_index == voice->port_index) &&
          (key == -1        || key == voice->key);
}

#define voice_match_event(voice, ev) voice_match(voice, (ev)->note_id, (ev)->channel, (ev)->port_index, (ev)->key)

static void bpbx_voice_end_cb(bpbx_inst_s *inst, bpbx_voice_id id) {
   inst_process_userdata_s *ud = bpbx_inst_get_userdata(inst);
   assert(ud);

   plugin_voice_s *voice = &ud->plugin->voices[id];
   voice->active = false;
   ud->plugin->active_voice_count--;

   clap_event_note_t ev = {
      .header = {
         .size = sizeof(ev),
         .time = ud->cur_sample,
         .type = CLAP_EVENT_NOTE_END,
      },
      .note_id = voice->note_id,
      .port_index = voice->port_index,
      .channel = voice->channel,
      .key = voice->key
   };

   ud->out_events->try_push(ud->out_events, (clap_event_header_t*) &ev);

   // char str[128];
   // snprintf(str, 128, "sample: %i", ud->cur_sample);
   // ud->plugin->host_log->log(ud->plugin->host, CLAP_LOG_DEBUG, str);
}

void plugin_init_inst(plugin_s *plug) {
    bpbx_inst_callbacks_s *inst_cbs = bpbx_inst_get_callback_table(plug->instrument);
    inst_cbs->voice_end = bpbx_voice_end_cb;
}












/*
 __   __   __   __   ___  __   __          __  
|__) |__) /  \ /  ` |__  /__` /__` | |\ | / _` 
|    |  \ \__/ \__, |___ .__/ .__/ | | \| \__> 
*/
static inline void plugin_begin_note(
   plugin_s *plug, int16_t key, double velocity,
   int32_t note_id, int16_t port_index, int16_t channel
) {
   bpbx_voice_id bpbx_id = bpbx_inst_begin_note(plug->instrument, key, velocity);
   plug->voices[bpbx_id] = (plugin_voice_s) {
      .active = true,
      .note_id = note_id,
      .port_index = port_index,
      .channel = channel,
      .key = key
   };
   plug->active_voice_count++;
}

static inline void plugin_end_notes(
   plugin_s *plug, int16_t key,
   int32_t note_id, int16_t port_index, int16_t channel
) {
   for (int i = 0; i < BPBX_INST_MAX_VOICES; i++) {
      plugin_voice_s *voice = &plug->voices[i];
      if (voice->active && voice_match(voice, note_id, channel, port_index, key)) {
         bpbx_inst_end_note(plug->instrument, i);
      }
   }
}

void plugin_process_event(plugin_s *plug, const clap_event_header_t *hdr, const clap_output_events_t *out_events) {
   if (hdr->space_id == CLAP_CORE_EVENT_SPACE_ID) {
      switch (hdr->type) {
      case CLAP_EVENT_NOTE_ON: {
         const clap_event_note_t *ev = (const clap_event_note_t *)hdr;

         plugin_begin_note(plug, ev->key, ev->velocity, ev->note_id, ev->port_index, ev->channel);
         break;
      }

      case CLAP_EVENT_NOTE_OFF: {
         const clap_event_note_t *ev = (const clap_event_note_t *)hdr;

         plugin_end_notes(plug, ev->key, ev->note_id, ev->port_index, ev->channel);
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
         
         plugin_params_set_value(plug, ev->param_id, ev->value, SEND_TO_GUI, out_events);
         
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
         uint8_t channel = ev->data[0] & 0x0F;

         // off
         if ((status == 0x80) || ((status == 0x90) && ev->data[2] == 0)) {
            plugin_end_notes(plug, ev->data[1], -1, ev->port_index, channel);
         }
         
         // on
         else if (status == 0x90) {
            plugin_begin_note(plug, ev->data[1], ev->data[2] / 127.0, -1, ev->port_index, channel);
         }

         // channel mode messages
         else if (status == 0xB0) {
            // all notes off
            if (ev->data[1] == 123 && ev->data[2] == 0) {
               plugin_end_notes(plug, -1, -1, ev->port_index, channel);
            }
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

void process_gui_events(plugin_s *plug, const clap_output_events_t *out_events) {
   assert(plug->gui);
   
   gui_event_queue_item_s item;
   while (gui_event_dequeue(plug->gui, &item)) {
      switch (item.type) {
         case GUI_EVENT_PARAM_CHANGE: {
            plugin_params_set_value(plug, item.param_value.param_id, item.param_value.value, SEND_TO_HOST, out_events);
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

void plugin_process_transport(plugin_s *plug, const clap_event_transport_t *ev) {
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

      if (is_playing)
         bpbx_inst_begin_transport(plug->instrument, plug->cur_beat, plug->bpm);
   }

   plug->is_playing = is_playing;
}

clap_process_status plugin_process(const struct clap_plugin *plugin, const clap_process_t *process) {
   fp_env env = disable_denormals();

   plugin_s *plug = plugin->plugin_data;

   if (plug->gui)
      process_gui_events(plug, process->out_events);

   if (process->transport) {
      plugin_process_transport(plug, process->transport);
   }

   const double sample_len = 1.0 / plug->sample_rate;

   const uint32_t nframes = process->frames_count;
   const uint32_t nev = process->in_events->size(process->in_events);
   uint32_t ev_index = 0;
   uint32_t next_ev_frame = nev > 0 ? 0 : nframes;

   inst_process_userdata_s inst_proc = {
      .plugin = plug,
      .out_events = process->out_events
   };
   bpbx_inst_set_userdata(plug->instrument, &inst_proc);

   for (uint32_t i = 0; i < nframes;) {
      /* handle every events that happrens at the frame "i" */
      while (ev_index < nev && next_ev_frame == i) {
         const clap_event_header_t *hdr = process->in_events->get(process->in_events, ev_index);
         if (hdr->time != i) {
            next_ev_frame = hdr->time;
            break;
         }

         plugin_process_event(plug, hdr, process->out_events);
         ++ev_index;

         if (ev_index == nev) {
            // we reached the end of the event list
            next_ev_frame = nframes;
            break;
         }
      }
      
      // update extra plugin values
      plug->linear_gain = pow(10.0, plug->gain / 10.0);
      double active_bpm;
      if (plug->tempo_use_override) {
         active_bpm = plug->tempo_override;
      } else {
         active_bpm = plug->bpm;
      
      }

      active_bpm *= plug->tempo_multiplier;

      // clamp active_bpm because if it is zero then the plugin will
      // take an (essentially) infinite amount of time to process a tick.
      // then this means any subsequent ticks will not be processed.
      // also it will completely freeze processing somehow.
      if (active_bpm < 1.0) {
         active_bpm = 1.0;
      }

      const double beats_per_sec = active_bpm / 60.0;

      /* process every samples until the next event */
      uint32_t frame_count = next_ev_frame - i;
      for (uint32_t j = 0; j < frame_count;) {
         // instrument needs a tick
         inst_proc.cur_sample = i + j;
         if (plug->frames_until_next_tick == 0) {
            bpbx_inst_tick(plug->instrument, &(bpbx_tick_ctx_s) {
               .bpm = active_bpm,
               .beat = plug->cur_beat,
            });

            plug->frames_until_next_tick =
               (uint32_t)ceil(bpbx_calc_samples_per_tick(active_bpm, plug->sample_rate));

            plug->cur_beat += beats_per_sec * sample_len * plug->frames_until_next_tick;
         }

         // render the audio
         uint32_t frames_to_process = frame_count - j;
         if (plug->frames_until_next_tick < frames_to_process)
            frames_to_process = plug->frames_until_next_tick;

         bpbx_inst_run(plug->instrument, plug->process_block + j, frames_to_process);
         j += frames_to_process;
         plug->frames_until_next_tick -= frames_to_process;
      }

      uint32_t buffer_idx = 0;
      for (; i < next_ev_frame; ++i) {
         // store output samples
         const float v = plug->process_block[buffer_idx] * plug->linear_gain;
         process->audio_outputs[0].data32[0][i] = v;
         process->audio_outputs[0].data32[1][i] = v;
         ++buffer_idx;
      }

   }

   bpbx_inst_set_userdata(plug->instrument, NULL);

   enable_denormals(env);

   if (plug->active_voice_count > 0)
      return CLAP_PROCESS_CONTINUE;
   else {
      // if (plug->host_log)
      //    plug->host_log->log(plug->host, CLAP_LOG_DEBUG, "Zzz...");

      return CLAP_PROCESS_SLEEP;
   }
}











/*


 __        __              ___ ___  ___  __   __  
|__)  /\  |__)  /\   |\/| |__   |  |__  |__) /__` 
|    /~~\ |  \ /~~\  |  | |___  |  |___ |  \ .__/ 
                                                  


*/
bool plugin_inst_params_get_info(const clap_plugin_t *plugin, uint32_t param_index, clap_param_info_t *param_info) {
   plugin_s *plug = plugin->plugin_data;

   const bpbx_inst_param_info_s *info = bpbx_param_info(bpbx_inst_type(plug->instrument), (unsigned int)param_index);
   if (info == NULL)
      return false;

   const char *module_name = "";
   if (info->group) {
      module_name = info->group;
   }

   param_info->cookie = NULL;
   param_info->id = param_index;
   impl_strcpy_s(param_info->name, 256, info->name);
   impl_strcpy_s(param_info->module, 1024, module_name);
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

bool plugin_inst_params_get_value(const clap_plugin_t *plugin, clap_id param_id, double *out_value) {
   plugin_s *plug = plugin->plugin_data;

   return !bpbx_inst_get_param_double(plug->instrument, param_id, out_value);
}

uint32_t plugin_inst_params_count(const clap_plugin_t *plugin) {
   plugin_s *plug = plugin->plugin_data;
   return (uint32_t) bpbx_param_count(bpbx_inst_type(plug->instrument));
}

bool plugin_inst_params_value_to_text(
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

bool plugin_inst_params_text_to_value(const clap_plugin_t *plugin, clap_id param_id, const char *param_value_text, double *out_value) {
   plugin_s *plug = plugin->plugin_data;

   const bpbx_inst_param_info_s *info = bpbx_param_info(bpbx_inst_type(plug->instrument), param_id);
   if (info == NULL)
      return false;

   switch (info->type) {
      case BPBX_PARAM_UINT8:
      case BPBX_PARAM_INT:
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

      case BPBX_PARAM_DOUBLE:
         *out_value = atof(param_value_text);
         break;

      default:
         return false;
   }

   return true;
}

uint32_t plugin_params_count(const clap_plugin_t *plugin) {
   return plugin_inst_params_count(plugin) + PLUGIN_CPARAM_COUNT;
}

bool plugin_params_get_value(const clap_plugin_t *plugin, clap_id param_id, double *out_value) {
    plugin_s *plug = plugin->plugin_data;

    if (param_id >= PLUGIN_CPARAM_START) {
        switch (param_id) {
            case PLUGIN_CPARAM_GAIN:
                *out_value = plug->gain;
                break;

            case PLUGIN_CPARAM_TEMPO_USE_OVERRIDE:
                *out_value = plug->tempo_override ? 1.0 : 0.0;
                break;

            case PLUGIN_CPARAM_TEMPO_MULTIPLIER:
                *out_value = plug->tempo_multiplier;
                break;

            case PLUGIN_CPARAM_TEMPO_OVERRIDE:
                *out_value = plug->tempo_override;
                break;

            default:
                return false;
        }

        return true;
    } else {
        return plugin_inst_params_get_value(plugin, param_id, out_value);
    }
}

bool plugin_params_set_value(plugin_s *plug, clap_id id, double value, event_send_flags_e send_flags, const clap_output_events_t *out_events) {
    if (id >= PLUGIN_CPARAM_START) {
        switch (id) {
            case PLUGIN_CPARAM_GAIN:
                plug->gain = value;
                break;

            case PLUGIN_CPARAM_TEMPO_USE_OVERRIDE:
                plug->tempo_use_override = value != 0.0;
                break;

            case PLUGIN_CPARAM_TEMPO_MULTIPLIER:
                plug->tempo_multiplier = value;
                break;

            case PLUGIN_CPARAM_TEMPO_OVERRIDE:
                plug->tempo_override = value;
                break;

            default:
                return false;
        }
    } else {
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
    }

   if (out_events && (send_flags & SEND_TO_HOST)) {
      clap_event_param_value_t out_ev = {
         .header.space_id = CLAP_CORE_EVENT_SPACE_ID,
         .header.size = sizeof(clap_event_param_value_t),
         .header.type = CLAP_EVENT_PARAM_VALUE,
         .header.time = 0,

         .param_id = id,
         .value = value,
         .cookie = NULL,

         .note_id = -1,
         .port_index = -1,
         .channel = -1,
         .key = -1,
      };

      out_events->try_push(out_events, (clap_event_header_t*)&out_ev);
   }

   if (plug->gui && (send_flags & SEND_TO_GUI)) {
      gui_event_queue_item_s item = {
         .type = GUI_EVENT_PARAM_CHANGE,
         .param_value.param_id = id,
         .param_value.value = value
      };

      gui_event_enqueue(plug->gui, item);
   }

   // when changing vibrato preset, change vibrato paramaters as well.
   // and when changing vibrato parameters, change preset to custom.
   // Kind of has to be done plugin-side so that the host knows that those
   // parameters changed...
   if (!(send_flags & NO_RECURSION)) {
      switch (id) {
         case BPBX_PARAM_VIBRATO_PRESET:
            if ((int)value != BPBX_VIBRATO_PRESET_CUSTOM) {
               bpbx_vibrato_params_s params;
               bpbx_vibrato_preset_params((int)value, &params);

               event_send_flags_e flags = SEND_TO_GUI | SEND_TO_HOST | NO_RECURSION;
               plugin_params_set_value(plug, BPBX_PARAM_VIBRATO_DEPTH, params.depth, flags, out_events);
               plugin_params_set_value(plug, BPBX_PARAM_VIBRATO_SPEED, params.speed, flags, out_events);
               plugin_params_set_value(plug, BPBX_PARAM_VIBRATO_DELAY, (double)params.delay, flags, out_events);
               plugin_params_set_value(plug, BPBX_PARAM_VIBRATO_TYPE, (double)params.type, flags, out_events);
            }
            break;

         case BPBX_PARAM_VIBRATO_DEPTH:
         case BPBX_PARAM_VIBRATO_SPEED:
         case BPBX_PARAM_VIBRATO_DELAY:
         case BPBX_PARAM_VIBRATO_TYPE: {
            int vibrato_preset;
            if (!bpbx_inst_get_param_int(plug->instrument, BPBX_PARAM_VIBRATO_PRESET, &vibrato_preset) && vibrato_preset != BPBX_VIBRATO_PRESET_CUSTOM)
               plugin_params_set_value(plug, BPBX_PARAM_VIBRATO_PRESET, (double)BPBX_VIBRATO_PRESET_CUSTOM, SEND_TO_GUI | SEND_TO_HOST | NO_RECURSION, out_events);
            break;
         }
      }
   }
   
   return true;
}

bool plugin_params_get_info(const clap_plugin_t *plugin, uint32_t param_index, clap_param_info_t *param_info) {
   plugin_s *plug = plugin->plugin_data;
   uint32_t cparam_start = plugin_inst_params_count(plugin);

   if (param_index >= cparam_start) {
      const plugin_control_param_info_s *info = &plugin_control_param_info[param_index - cparam_start];

      param_info->cookie = NULL;
      param_info->id = PLUGIN_CPARAM_START + (param_index - cparam_start);
      impl_strcpy_s(param_info->name, 256, info->name);
      impl_strcpy_s(param_info->module, 1024, "Plugin Control");
      param_info->default_value = info->default_value;
      param_info->min_value = info->min_value;
      param_info->max_value = info->max_value;
      param_info->flags = info->flags;

      return true;
   } else {
      return plugin_inst_params_get_info(plugin, param_index, param_info);
   }
}

bool plugin_params_value_to_text(
      const clap_plugin_t *plugin,
      clap_id param_id, double value,
      char *out_buf, uint32_t out_buf_capacity
) {
   if (param_id >= PLUGIN_CPARAM_START) {
      switch (param_id) {
         case PLUGIN_CPARAM_GAIN:
         case PLUGIN_CPARAM_TEMPO_MULTIPLIER:
         case PLUGIN_CPARAM_TEMPO_OVERRIDE:
            snprintf(out_buf, out_buf_capacity, "%.3f", value);
            return true;

         case PLUGIN_CPARAM_TEMPO_USE_OVERRIDE:
            if (value != 0.0)
               impl_strcpy_s(out_buf, out_buf_capacity, "On");
            else
               impl_strcpy_s(out_buf, out_buf_capacity, "Off");

            return true;

         default:
            return false;
      }
   } else {
      return plugin_inst_params_value_to_text(plugin, param_id, value, out_buf, out_buf_capacity);
   }
}

bool plugin_params_text_to_value(const clap_plugin_t *plugin, clap_id param_id, const char *param_value_text, double *out_value) {
   if (param_id >= PLUGIN_CPARAM_START) {
      switch (param_id) {
         case PLUGIN_CPARAM_GAIN:
         case PLUGIN_CPARAM_TEMPO_MULTIPLIER:
         case PLUGIN_CPARAM_TEMPO_OVERRIDE:
            *out_value = atof(param_value_text);
            return true;

         case PLUGIN_CPARAM_TEMPO_USE_OVERRIDE:
            if (!strcmp(param_value_text, "On"))
               *out_value = 1.0;
            else if (!strcmp(param_value_text, "Off"))
               *out_value = 0.0;
            else
               return false;

            return true;

         default:
            return false;
      }
   } else {
      return plugin_inst_params_text_to_value(plugin, param_id, param_value_text, out_value);
   }
}

















/*


 __  ___      ___  ___ 
/__`  |   /\   |  |__  
.__/  |  /~~\  |  |___ 
                       


*/
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

   // first, write instrument parameter data
   bpbx_inst_type_e type = bpbx_inst_type(plug->instrument);

   uint8_t param_count = (uint8_t) bpbx_param_count(type);
   ERRCHK(stream_write_prim(stream, &param_count, sizeof(param_count)));

   for (unsigned int i = 0; i < param_count; ++i) {
      const bpbx_inst_param_info_s *info = bpbx_param_info(type, i);
      if (info == NULL) return false;

      // parameter id
      ERRCHK(stream_write_prim(stream, info->id, sizeof(info->id)));

      // store type data - redundant, but whatever
      uint8_t p_type = (uint8_t)info->type;
      ERRCHK(stream_write_prim(stream, &p_type, sizeof(p_type)));

      // parameter value
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

   for (uint8_t i = 0; i < envelope_count; ++i) {
      const bpbx_envelope_s *env = bpbx_inst_get_envelope(plug->instrument, i);
      if (env == NULL) return false;

      ERRCHK(stream_write_prim(stream, &env->index, sizeof(env->index)));
      ERRCHK(stream_write_prim(stream, &env->curve_preset, sizeof(env->curve_preset)));
   }

   // third, store plugin control parameters
   param_count = (uint8_t) PLUGIN_CPARAM_COUNT;
   ERRCHK(stream_write_prim(stream, &param_count, sizeof(param_count)));
   for (clap_id i = PLUGIN_CPARAM_START; i < PLUGIN_CPARAM_END; i++) {
      double param_value;
      plugin_control_param_info_s param_info = plugin_control_param_info[i - PLUGIN_CPARAM_START];

      if (!plugin_params_get_value(plugin, i, &param_value))
         return false;
      
      ERRCHK(stream_write_prim(stream, param_info.serialized_id, sizeof(param_info.serialized_id)));
      ERRCHK(stream_write_prim(stream, &param_value, sizeof(param_value)));
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

   for (unsigned int i = 0; i < param_count; ++i) {
      char p_id[8];
      ERRCHK(stream_read_prim(stream, p_id, sizeof(p_id)));

      // get index of parameter with this id
      uint8_t param_index;
      const bpbx_inst_param_info_s *info = NULL;
      for (param_index = 0; param_index < param_count; param_index++) {
         const bpbx_inst_param_info_s *vinfo = bpbx_param_info(type, param_index);
         if (vinfo && !memcmp(vinfo->id, p_id, sizeof(p_id))) {
            info = vinfo;
            break;
         }
      }
      if (info == NULL) return false;

      uint8_t p_type;
      ERRCHK(stream_read_prim(stream, &p_type, sizeof(p_type)));
      if (p_type != (uint8_t)info->type) return false;

      switch (p_type) {
         case BPBX_PARAM_UINT8: {
            uint8_t v;
            ERRCHK(stream_read_prim(stream, &v, sizeof(v)));
            bpbx_inst_set_param_int(plug->instrument, param_index, v);
            break;
         }

         case BPBX_PARAM_INT: {
            int v;
            ERRCHK(stream_read_prim(stream, &v, sizeof(v)));
            bpbx_inst_set_param_int(plug->instrument, param_index, v);
            break;
         }

         case BPBX_PARAM_DOUBLE: {
            double v;
            ERRCHK(stream_read_prim(stream, &v, sizeof(v)));
            bpbx_inst_set_param_double(plug->instrument, param_index, v);
            break;
         }
      }
   }

   // read envelopes
   uint8_t envelope_count;
   ERRCHK(stream_read_prim(stream, &envelope_count, sizeof(envelope_count)));

   bpbx_inst_clear_envelopes(plug->instrument);
   for (uint8_t i = 0; i < envelope_count; ++i) {
      bpbx_envelope_s *env = bpbx_inst_add_envelope(plug->instrument);
      ERRCHK(stream_read_prim(stream, &env->index, sizeof(env->index)));
      ERRCHK(stream_read_prim(stream, &env->curve_preset, sizeof(env->curve_preset)));
   }

   // read plugin control parameters
   ERRCHK(stream_read_prim(stream, &param_count, sizeof(param_count)));
   if (param_count != PLUGIN_CPARAM_COUNT) return false;
   for (uint8_t i = 0; i < param_count; i++) {
      char p_id[8];
      ERRCHK(stream_read_prim(stream, p_id, sizeof(p_id)));

      // find control parameter with this id
      clap_id param_id = CLAP_INVALID_ID;
      double param_value;
      for (clap_id j = PLUGIN_CPARAM_START; j < PLUGIN_CPARAM_END; j++) {
         if (!memcmp(plugin_control_param_info[j - PLUGIN_CPARAM_START].serialized_id, p_id, 8)) {
            param_id = j;
            break;
         }
      }
      if (param_id == CLAP_INVALID_ID) return false;

      // set the parameter value
      ERRCHK(stream_read_prim(stream, &param_value, sizeof(param_value)));
      plugin_params_set_value(plug, param_id, param_value, NO_RECURSION, NULL);
   }

   if (plug->gui) gui_sync_state(plug->gui);
   return true;

   error:
      if (plug->gui) gui_sync_state(plug->gui);
      return false;
}
#undef ERRCHK








plugin_control_param_info_s plugin_control_param_info[PLUGIN_CPARAM_COUNT] = {
    {
        .name = "Gain",
        .serialized_id = "ctGain\0\0",
        .min_value = -10.0,
        .max_value = 10.0,
        .default_value = 0.0,
    },

    {
        .name = "Force Tempo",
        .serialized_id = "ctTmpMod",
        .min_value = 0,
        .max_value = 1,
        .default_value = 0,
        .flags = CLAP_PARAM_IS_STEPPED
    },
    {
        .name = "Tempo Multiplier",
        .serialized_id = "ctTmpMul",
        .min_value = 0.0,
        .max_value = 10.0,
        .default_value = 1.0,
    },
    {
        .name = "Tempo Force Value",
        .serialized_id = "ctTmpOvr",
        .min_value = 1.0,
        .max_value = 500.0,
        .default_value = 150.0,
    },
};