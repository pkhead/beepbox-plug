#include "plugin_impl.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <cbeepsynth/synth/include/beepbox_synth.h>
#include "system.h"
#include "util.h"

static void plugin_track_info_changed(plugin_s *plug) {
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

static void bpbx_log_cb(bpbxsyn_log_severity_e severity, const char *msg, void *userdata) {
   plugin_s *plug = (plugin_s*)userdata;
   clap_log_severity clap_sev = CLAP_LOG_INFO;
   switch (severity) {
      case BPBXSYN_LOG_DEBUG:
         clap_sev = CLAP_LOG_DEBUG;
         break;

      case BPBXSYN_LOG_INFO:
         clap_sev = CLAP_LOG_INFO;
         break;

      case BPBXSYN_LOG_WARNING:
         clap_sev = CLAP_LOG_WARNING;
         break;

      case BPBXSYN_LOG_ERROR:
         clap_sev = CLAP_LOG_ERROR;
         break;

      case BPBXSYN_LOG_FATAL:
         clap_sev = CLAP_LOG_FATAL;
         break;
   }

   plug->host_log->log(plug->host, clap_sev, msg);
}

void plugin_create(plugin_s *plug, bpbxsyn_synth_type_e type) {
    plug->has_track_color = false;
    plug->instrument.type = type; // store type temporarily
}

#ifndef _NDEBUG
static void* testalloc(size_t size, void *ud) {
    plugin_s *plug = ud;
    uint8_t *alloc = malloc(size + sizeof(size_t));
    if (!alloc) return NULL;

    plug->mem_allocated += size;
    *((size_t*)alloc) = size;
    return alloc + sizeof(size_t);
}

static void testfree(void *ptr, void *ud) {
    if (!ptr) return;

    plugin_s *plug = ud;
    uint8_t *base = (uint8_t*)ptr - sizeof(size_t);
    plug->mem_allocated -= *((size_t*)base);
    free(base);
}
#endif

bool plugin_init(plugin_s *plug) {
    // Fetch host's extensions here
    // Make sure to check that the interface functions are not null pointers
    plug->host_log = (const clap_host_log_t *)plug->host->get_extension(plug->host, CLAP_EXT_LOG);
    plug->host_thread_check = (const clap_host_thread_check_t *)plug->host->get_extension(plug->host, CLAP_EXT_THREAD_CHECK);
    plug->host_latency = (const clap_host_latency_t *)plug->host->get_extension(plug->host, CLAP_EXT_LATENCY);
    plug->host_state = (const clap_host_state_t *)plug->host->get_extension(plug->host, CLAP_EXT_STATE);
    plug->host_track_info = (const clap_host_track_info_t*) plug->host->get_extension(plug->host, CLAP_EXT_TRACK_INFO);
    plug->host_context_menu = (const clap_host_context_menu_t*) plug->host->get_extension(plug->host, CLAP_EXT_CONTEXT_MENU);

    #ifndef _NDEBUG
    bpbxsyn_allocator_s alloc = (bpbxsyn_allocator_s) {
        .alloc = testalloc,
        .free = testfree,
        .userdata = plug
    };

    plug->ctx = bpbxsyn_context_new(&alloc);
    #endif

    if (!plug->ctx) return false;

    if (!instr_init(&plug->instrument, plug->ctx, plug->instrument.type))
        return false;

    if (plug->host_track_info) {
        plugin_track_info_changed(plug);
    }
    
    if (plug->host_log) {
        gui_set_log_func(plug->host_log->log, plug->host);
        bpbxsyn_set_log_func(plug->ctx, bpbx_log_cb, plug);
    }

    return true;
}

void plugin_destroy(plugin_s *plug) {
    instr_destroy(&plug->instrument);
    bpbxsyn_context_destroy(plug->ctx);
    plug->ctx = NULL;
}

bool plugin_activate(plugin_s *plug, double sample_rate,
                     uint32_t min_frames_count, uint32_t max_frames_count)
{
    return instr_activate(&plug->instrument, sample_rate, max_frames_count);
}

bool plugin_deactivate(plugin_s *plug) {
    #ifndef _NDEBUG
    char buf[128];
    snprintf(buf, 128, "%llu bytes allocated", plug->mem_allocated);

    plug->host_log->log(plug->host, CLAP_LOG_DEBUG, buf);
    #endif
    
    return instr_deactivate(&plug->instrument);
}

void plugin_process_gui_events(plugin_s *plug,
                               const clap_output_events_t *out_events)
{
    assert(plug->gui);
   
    gui_event_queue_item_s item;
    while (gui_event_dequeue(plug->gui, &item)) {
        switch (item.type) {
            case GUI_EVENT_PARAM_CHANGE: {
                plugin_params_set_value(plug, item.param_value.param_id,
                                        item.param_value.value, SEND_TO_HOST,
                                        out_events);
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
                bpbxsyn_synth_add_envelope(plug->instrument.synth);
                break;

            case GUI_EVENT_MODIFY_ENVELOPE:
                *bpbxsyn_synth_get_envelope(plug->instrument.synth, item.modify_envelope.index)
                    = item.modify_envelope.envelope;
                break;
            
            case GUI_EVENT_REMOVE_ENVELOPE:
                bpbxsyn_synth_remove_envelope(plug->instrument.synth, item.envelope_removal.index);
                break;

            default:
                break;
        }
    }
}

void plugin_process_transport(plugin_s *plug, const clap_event_transport_t *ev) {
    if (ev->flags & CLAP_TRANSPORT_HAS_TEMPO) {
        plug->instrument.bpm = ev->tempo;
    } else {
        plug->instrument.bpm = 150.0;
    }

    bool is_playing = (ev->flags & CLAP_TRANSPORT_IS_PLAYING) != 0;

    // if playing state changed, update current beat from transport event.
    // otherwise plugin should increment cur beat on its own using bpm info.
    if (is_playing != plug->instrument.is_playing) {
        if (ev->flags & CLAP_TRANSPORT_HAS_BEATS_TIMELINE) {
            int64_t beats_int = ev->song_pos_beats / CLAP_BEATTIME_FACTOR;
            int64_t beats_frac = ev->song_pos_beats % CLAP_BEATTIME_FACTOR;
            plug->instrument.cur_beat =
                (double)beats_int + (double)beats_frac / CLAP_BEATTIME_FACTOR;
        }

        if (is_playing)
            bpbxsyn_synth_begin_transport(plug->instrument.synth,
                                          plug->instrument.cur_beat,
                                          plug->instrument.bpm);
    }

    plug->instrument.is_playing = is_playing;
}

void plugin_process_event(plugin_s *plug, const clap_event_header_t *hdr,
                          const clap_output_events_t *out_events)
{
    if (hdr->space_id == CLAP_CORE_EVENT_SPACE_ID) {
        switch (hdr->type) {
        case CLAP_EVENT_NOTE_ON: {
            const clap_event_note_t *ev = (const clap_event_note_t *)hdr;

            instr_begin_note(&plug->instrument, ev->key, ev->velocity,
                             ev->note_id, ev->port_index, ev->channel);
            break;
        }

        case CLAP_EVENT_NOTE_OFF: {
            const clap_event_note_t *ev = (const clap_event_note_t *)hdr;

            instr_end_notes(&plug->instrument, ev->key, ev->note_id,
                            ev->port_index, ev->channel);
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
                instr_end_notes(&plug->instrument, ev->data[1], -1, ev->port_index, channel);
            }
            
            // on
            else if (status == 0x90) {
                instr_begin_note(&plug->instrument, ev->data[1], ev->data[2] / 127.0, -1, ev->port_index, channel);
            }

            // channel mode messages
            else if (status == 0xB0) {
                // all notes off
                if (ev->data[1] == 123 && ev->data[2] == 0) {
                    instr_end_notes(&plug->instrument, -1, -1, ev->port_index, channel);
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

clap_process_status plugin_process(plugin_s *plug,
                                   const clap_process_t *process)
{
    fp_env env = disable_denormals();

    if (plug->gui)
        plugin_process_gui_events(plug, process->out_events);

    if (process->transport) {
        plugin_process_transport(plug, process->transport);
    }

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

            plugin_process_event(plug, hdr, process->out_events);
            ++ev_index;

            if (ev_index == nev) {
                // we reached the end of the event list
                next_ev_frame = nframes;
                break;
            }
        }

        /* process every samples until the next event */
        uint32_t frame_count = next_ev_frame - i;

        float* output[2];
        output[0] = process->audio_outputs->data32[0] + i;
        output[1] = process->audio_outputs->data32[1] + i;
        instr_process(&plug->instrument, output, frame_count, i, process->out_events);

        i += frame_count;
    }

    enable_denormals(env);

    if (plug->instrument.active_voice_count > 0)
        return CLAP_PROCESS_CONTINUE;
    else {
        // if (plug->host_log)
        //    plug->host_log->log(plug->host, CLAP_LOG_DEBUG, "Zzz...");

        return CLAP_PROCESS_SLEEP;
    }
}

uint32_t plugin_params_count(const plugin_s *plug) {
    return instr_params_count(&plug->instrument);
}

bool plugin_params_get_info(const plugin_s *plugin, uint32_t param_index,
                            clap_param_info_t *param_info)
{
    instr_param_id param_id = instr_get_param_id(&plugin->instrument, param_index);
    const bpbxsyn_param_info_s *info =
        instr_get_param_info(&plugin->instrument, param_id);
    if (!info)
        return false;

    const char *module_name = "";
    if (info->group) {
        module_name = info->group;
    }

    param_info->cookie = NULL;
    param_info->id = param_id;
    impl_strcpy_s(param_info->name, 256, info->name);
    impl_strcpy_s(param_info->module, 1024, module_name);
    param_info->default_value = info->default_value;
    param_info->min_value = info->min_value;
    param_info->max_value = info->max_value;

    param_info->flags = CLAP_PARAM_IS_AUTOMATABLE;
    if (info->type == BPBXSYN_PARAM_UINT8 || info->type == BPBXSYN_PARAM_INT) {
        param_info->flags |= CLAP_PARAM_IS_STEPPED;

        if (info->enum_values) param_info->flags |= CLAP_PARAM_IS_ENUM;
    }

    // ignore this property i suppose ...
    // if (!info->no_modulation)
    //    param_info->flags |= CLAP_PARAM_IS_AUTOMATABLE;

    return true;
}

bool plugin_params_get_value(const plugin_s *plug, clap_id param_id,
                             double *out_value)
{
    return instr_get_param(&plug->instrument, param_id, out_value);
}

bool plugin_params_set_value(plugin_s *plug, clap_id id, double value,
                             event_send_flags_e send_flags,
                             const clap_output_events_t *out_events)
{
    if (!instr_set_param(&plug->instrument, id, value))
        return false;

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
         case BPBXSYN_PARAM_VIBRATO_PRESET:
            if ((int)value != BPBXSYN_VIBRATO_PRESET_CUSTOM) {
               bpbxsyn_vibrato_params_s params;
               bpbxsyn_vibrato_preset_params((int)value, &params);

               event_send_flags_e flags = SEND_TO_GUI | SEND_TO_HOST | NO_RECURSION;
               plugin_params_set_value(plug,
                    instr_synth_param(BPBXSYN_PARAM_VIBRATO_DEPTH),
                    params.depth, flags, out_events);
               plugin_params_set_value(plug,
                    instr_synth_param(BPBXSYN_PARAM_VIBRATO_SPEED),
                    params.speed, flags, out_events);
               plugin_params_set_value(plug,
                    instr_synth_param(BPBXSYN_PARAM_VIBRATO_DELAY),
                    (double)params.delay, flags, out_events);
               plugin_params_set_value(plug,
                    instr_synth_param(BPBXSYN_PARAM_VIBRATO_TYPE),
                    (double)params.type, flags, out_events);
            }
            break;

         case BPBXSYN_PARAM_VIBRATO_DEPTH:
         case BPBXSYN_PARAM_VIBRATO_SPEED:
         case BPBXSYN_PARAM_VIBRATO_DELAY:
         case BPBXSYN_PARAM_VIBRATO_TYPE: {
            double vibrato_preset;
            bool s = instr_get_param(&plug->instrument,
                instr_synth_param(BPBXSYN_PARAM_VIBRATO_PRESET), &vibrato_preset);
            
            if (!s && (int)vibrato_preset != BPBXSYN_VIBRATO_PRESET_CUSTOM)
                plugin_params_set_value(plug, BPBXSYN_PARAM_VIBRATO_PRESET,
                    (double)BPBXSYN_VIBRATO_PRESET_CUSTOM,
                    SEND_TO_GUI | SEND_TO_HOST | NO_RECURSION, out_events);

            break;
         }
      }
   }
   
   return true;
}

bool plugin_params_value_to_text(const plugin_s *plug, clap_id param_id,
                                 double value, char *out_buf,
                                 uint32_t out_buf_capacity)
{
    const bpbxsyn_param_info_s *info = instr_get_param_info(&plug->instrument, param_id);
    if (!info)
        return false;

    switch (info->type) {
        case BPBXSYN_PARAM_UINT8:
        case BPBXSYN_PARAM_INT: {
            if (info->enum_values) {
                snprintf(out_buf, out_buf_capacity, "%s", info->enum_values[(int)value]);
            } else {
                snprintf(out_buf, out_buf_capacity, "%i", (int)value);
            }

            break;
        }

        case BPBXSYN_PARAM_DOUBLE: {
            snprintf(out_buf, out_buf_capacity, "%.1f", value);
            break;
        }

        default:
            return false;
    }

    return true;
}

bool plugin_params_text_to_value(const plugin_s *plug, clap_id param_id,
                                 const char *param_value_text,
                                 double *out_value)
{
    const bpbxsyn_param_info_s *info = instr_get_param_info(&plug->instrument, param_id);
    if (!info)
        return false;

    switch (info->type) {
        case BPBXSYN_PARAM_UINT8:
        case BPBXSYN_PARAM_INT:
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

        case BPBXSYN_PARAM_DOUBLE: {
            *out_value = atof(param_value_text);
            break;
        }

        default:
            return false;
   }

   return true;
}





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

bool plugin_state_save(const plugin_s *plug, const clap_ostream_t *stream) {
    // write save format revision (single number)
    uint32_t save_version = STATE_SAVE_VER;
    ERRCHK(stream_write_prim(stream, &save_version, sizeof(save_version)));

    // write synth version
    uint32_t synth_maj = BPBXSYN_VERSION_MAJOR;
    uint32_t synth_min = BPBXSYN_VERSION_MINOR;
    uint32_t synth_rev = BPBXSYN_VERSION_REVISION;
    ERRCHK(stream_write_prim(stream, &synth_maj, sizeof(synth_maj)));
    ERRCHK(stream_write_prim(stream, &synth_min, sizeof(synth_min)));
    ERRCHK(stream_write_prim(stream, &synth_rev, sizeof(synth_rev)));

    // write instrument type
    uint8_t type = (uint8_t)plug->instrument.type;
    ERRCHK(stream_write_prim(stream, &type, sizeof(type)));
    
    // write instrument parameter data
    uint32_t param_count = instr_params_count(&plug->instrument);
    
    // first, get list of parameter ids to write
    instr_param_id *param_ids = malloc(param_count * sizeof(uint32_t));
    int write_param_count = 0;

    for (uint32_t i = 0; i < param_count; ++i) {
        uint32_t param_id = instr_get_param_id(&plug->instrument, i);
        if (param_id == INSTR_INVALID_ID) goto error;

        // don't write this parameter if associated module is inactive
        // uh, nevermind. i need to disable this so that it passes the
        // clap-validator state reproducibiilty state.
        // instr_module_e module;
        // instr_local_id(param_id, &module, NULL);
        // if (!instr_is_module_active(&plug->instrument, module))
        //     continue;

        param_ids[write_param_count++] = param_id;
    }

    // write parameter count
    ERRCHK(stream_write_prim(stream, &write_param_count, sizeof(write_param_count)));

    // write the parameters
    for (uint32_t i = 0; i < write_param_count; ++i) {
        uint32_t param_id = param_ids[i];

        // get param info
        const bpbxsyn_param_info_s *info = instr_get_param_info(&plug->instrument, param_id);
        if (!info)
            goto error;

        // write parameter id
        ERRCHK(stream_write_prim(stream, info->id, sizeof(info->id)));

        // write parameter value
        double value;
        if (!instr_get_param(&plug->instrument, param_id, &value))
            goto error;
        
        ERRCHK(stream_write_prim(stream, &value, sizeof(value)));
    }

    free(param_ids);

    // write envelope data
    uint8_t envelope_count = bpbxsyn_synth_envelope_count(plug->instrument.synth);
    ERRCHK(stream_write_prim(stream, &envelope_count, sizeof(envelope_count)));

    for (uint8_t i = 0; i < envelope_count; ++i) {
        const bpbxsyn_envelope_s *env = bpbxsyn_synth_get_envelope(plug->instrument.synth, i);
        if (env == NULL) goto error;

        ERRCHK(stream_write_prim(stream, &env->index, sizeof(env->index)));
        ERRCHK(stream_write_prim(stream, &env->curve_preset, sizeof(env->curve_preset)));
    }

    return true;
    error:
        free(param_ids);
        return false;
}

bool plugin_state_load(plugin_s *plug, const clap_istream_t *stream) {
    // read versions; do strict version checking for now.
    uint32_t save_version;
    ERRCHK(stream_read_prim(stream, &save_version, sizeof(save_version)));
    if (save_version != STATE_SAVE_VER) return false;

    // read and skip synth version
    uint32_t synth_maj;
    uint32_t synth_min;
    uint32_t synth_rev;
    ERRCHK(stream_read_prim(stream, &synth_maj, sizeof(synth_maj)));
    ERRCHK(stream_read_prim(stream, &synth_min, sizeof(synth_min)));
    ERRCHK(stream_read_prim(stream, &synth_rev, sizeof(synth_rev)));

    // read and verify instrument type
    uint8_t file_inst_type;
    ERRCHK(stream_read_prim(stream, &file_inst_type, sizeof(file_inst_type)));
    if (file_inst_type != (uint8_t)plug->instrument.type)
        goto error;

    uint32_t all_param_count = instr_params_count(&plug->instrument);
    uint32_t param_count;
    ERRCHK(stream_read_prim(stream, &param_count, sizeof(param_count)));

    for (uint32_t i = 0; i < param_count; ++i) {
        // read string id
        char p_id[8];
        ERRCHK(stream_read_prim(stream, p_id, sizeof(p_id)));

        // get id of parameter with this string id
        instr_param_id param_id = INSTR_INVALID_ID;
        const bpbxsyn_param_info_s *info = NULL;
        for (uint32_t j = 0; j < all_param_count; ++j) {
            instr_param_id id = instr_get_param_id(&plug->instrument, j);
            assert(id != INSTR_INVALID_ID);
            if (id == INSTR_INVALID_ID)
                goto error;
            
            const bpbxsyn_param_info_s *vinfo = instr_get_param_info(&plug->instrument, id);
            if (!vinfo)
                goto error;
            
            if (!memcmp(vinfo->id, p_id, sizeof(p_id))) {
                param_id = id;
                break;
            }
        }

        if (param_id == INSTR_INVALID_ID)
            goto error;

        // read value
        double value;
        ERRCHK(stream_read_prim(stream, &value, sizeof(value)));
        if (!instr_set_param(&plug->instrument, param_id, value))
            goto error;
    }

    // read envelopes
    uint8_t envelope_count;
    ERRCHK(stream_read_prim(stream, &envelope_count, sizeof(envelope_count)));

    bpbxsyn_synth_clear_envelopes(plug->instrument.synth);
    for (uint8_t i = 0; i < envelope_count; ++i) {
        bpbxsyn_envelope_s *env = bpbxsyn_synth_add_envelope(plug->instrument.synth);
        ERRCHK(stream_read_prim(stream, &env->index, sizeof(env->index)));
        ERRCHK(stream_read_prim(stream, &env->curve_preset, sizeof(env->curve_preset)));
    }

    if (plug->gui) gui_sync_state(plug->gui);
    return true;

    error:
        if (plug->gui) gui_sync_state(plug->gui);
        return false;
}