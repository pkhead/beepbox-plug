#include "include/instrument.h"

#include <assert.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    instrument_s *instr;
    uint32_t cur_sample;
    const clap_output_events_t *out_events;
} inst_process_userdata_s;

#define voice_match_event(voice, ev) voice_match(voice, (ev)->note_id, (ev)->channel, (ev)->port_index, (ev)->key)

static inline bool voice_match(const voice_s *voice, int32_t note_id, int16_t channel, int16_t port_index, int16_t key) {
    return (note_id == -1    || note_id == voice->note_id) &&
           (channel == -1    || channel == voice->channel) &&
           (port_index == -1 || port_index == voice->port_index) &&
           (key == -1        || key == voice->key);
}

static void bpbxsyn_voice_end_cb(bpbxsyn_synth_s *inst, bpbxsyn_voice_id id) {
   inst_process_userdata_s *ud = bpbxsyn_synth_get_userdata(inst);
   assert(ud);

   voice_s *voice = &ud->instr->voices[id];
   voice->active = false;
   ud->instr->active_voice_count--;

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

bpbxsyn_param_info_s control_param_info[INSTR_CPARAM_COUNT];






















bool instr_init(instrument_s *instr, bpbxsyn_synth_type_e type) {
    *instr = (instrument_s) {
        .type = type,
        .bpm = 150.0,
        .tempo_multiplier = 1.0,
        .tempo_override = 150.0,
        .tempo_use_override = false,

        .active_modules[INSTR_MODULE_SYNTH] = true,
        .active_modules[INSTR_MODULE_CONTROL] = true,
    };

    instr->synth = bpbxsyn_synth_new(type);
    if (!instr->synth) return false;

    instr->fx_panning = bpbxsyn_effect_new(BPBXSYN_EFFECT_PANNING);
    if (!instr->fx_panning) return false;

    return true;
}

void instr_destroy(instrument_s *instr) {
    bpbxsyn_synth_destroy(instr->synth);
    bpbxsyn_effect_destroy(instr->fx_panning);
}

bool instr_activate(instrument_s *instr, double sample_rate,
                    uint32_t max_frames_count)
{
    instr->sample_rate = sample_rate;

    bpbxsyn_synth_set_sample_rate(instr->synth, sample_rate);
    bpbxsyn_effect_set_sample_rate(instr->fx_panning, sample_rate);

    free(instr->process_block);
    instr->process_block = malloc(max_frames_count * sizeof(float));
    if (!instr->process_block)
        return false;

    return true;
}

bool instr_deactivate(instrument_s *instr) {
    free(instr->process_block);
    instr->process_block = NULL;
    return true;
}

void instr_process(instrument_s *instr, float **output, uint32_t frame_count,
                   uint32_t start_frame, const clap_output_events_t *out_events)
{
    inst_process_userdata_s inst_proc = {
        .instr = instr,
        .cur_sample = start_frame,
        .out_events = out_events
    };
    bpbxsyn_synth_set_userdata(instr->synth, &inst_proc);

    instr->linear_gain = pow(10.0, instr->gain / 10.0);
    double active_bpm;
    if (instr->tempo_use_override) {
        active_bpm = instr->tempo_override;
    } else {
        active_bpm = instr->bpm;
    }

    active_bpm *= instr->tempo_multiplier;

    // clamp active_bpm because if it is zero then the plugin will
    // take an (essentially) infinite amount of time to process a tick.
    // then this means any subsequent ticks will not be processed.
    // also it will completely freeze processing somehow.
    if (active_bpm < 1.0) {
        active_bpm = 1.0;
    }

    const double beats_per_sec = active_bpm / 60.0;
    const double sample_len = 1.0 / instr->sample_rate;

    float *out_l = output[0];
    float *out_r = output[1];
    for (uint32_t i = 0; i < frame_count;) {
        // instrument needs a tick
        if (instr->frames_until_next_tick == 0) {
            bpbxsyn_synth_tick(instr->synth, &(bpbxsyn_tick_ctx_s) {
                .bpm = active_bpm,
                .beat = instr->cur_beat,
            });

            instr->frames_until_next_tick =
                (uint32_t)ceil(bpbxsyn_calc_samples_per_tick(active_bpm, instr->sample_rate));

            instr->cur_beat += beats_per_sec * sample_len * instr->frames_until_next_tick;
        }

        // render the audio
        uint32_t frames_to_process = frame_count - i;
        if (instr->frames_until_next_tick < frames_to_process)
            frames_to_process = instr->frames_until_next_tick;

        bpbxsyn_synth_run(instr->synth, instr->process_block + i, frames_to_process);
        i += frames_to_process;
        inst_proc.cur_sample += frames_to_process;
        instr->frames_until_next_tick -= frames_to_process;
    }

    for (uint32_t i = 0; i < frame_count; ++i) {
        // store output samples
        const float v = instr->process_block[i] * instr->linear_gain;
        output[0][i] = v;
        output[1][i] = v;
    }

    bpbxsyn_synth_set_userdata(instr->synth, NULL);
}

bool instr_is_module_active(const instrument_s *instr, instr_module_e module) {
    // these modules cannot be deactivated
    if (module == INSTR_MODULE_SYNTH || module == INSTR_MODULE_CONTROL)
        return true;

    return instr->active_modules[module];
}

void instr_activate_module(instrument_s *instr, instr_module_e module) {
    // these modules cannot be interacted with
    if (module == INSTR_MODULE_SYNTH || module == INSTR_MODULE_CONTROL)
        return;

    // this module is already active
    if (instr->active_modules[module]) return;
    instr->active_modules[module] = true;
}

void instr_deactivate_module(instrument_s *instr, instr_module_e module) {
    // these modules cannot be deactivated
    if (module == INSTR_MODULE_SYNTH || module == INSTR_MODULE_CONTROL)
        return;

    // this module is already inactive
    if (!instr->active_modules[module]) return;

    instr->active_modules[module] = false;

    // choke the effect processing
    switch (module) {
        case INSTR_MODULE_PANNING:
            bpbxsyn_effect_stop(instr->fx_panning);
            break;
        
        default:
            break;
    }
}

void instr_begin_note(instrument_s *instr, int16_t key, double velocity,
                      int32_t note_id, int16_t port_index, int16_t channel)
{
    bpbxsyn_voice_id bpbxsyn_id = bpbxsyn_synth_begin_note(
        instr->synth, key, velocity,
        BPBXSYN_NOTE_LENGTH_UNKNOWN);
   
    instr->voices[bpbxsyn_id] = (voice_s) {
        .active = true,
        .note_id = note_id,
        .port_index = port_index,
        .channel = channel,
        .key = key
    };
    
    ++instr->active_voice_count;
}

void instr_end_notes(instrument_s *instr, int16_t key, int32_t note_id,
                     int16_t port_index, int16_t channel)
{
    for (int i = 0; i < BPBXSYN_SYNTH_MAX_VOICES; i++) {
        voice_s *voice = &instr->voices[i];
        if (voice->active && voice_match(voice, note_id, channel, port_index, key)) {
            bpbxsyn_synth_end_note(instr->synth, i);
        }
   }
}

uint32_t instr_params_count(const instrument_s *instr) {
    return bpbxsyn_synth_param_count(instr->type) +
        INSTR_CPARAM_COUNT +
        BPBXSYN_PANNING_PARAM_COUNT;
}

instr_param_id instr_get_param_id(const instrument_s *instr, uint32_t index) {
    #define check(module, count) \
        if (index < count) \
            return instr_global_id(module, index); \
        index -= count

    unsigned int synth_param_count = bpbxsyn_synth_param_count(instr->type);

    check(INSTR_MODULE_SYNTH, synth_param_count);
    check(INSTR_MODULE_CONTROL, INSTR_CPARAM_COUNT);
    check(INSTR_MODULE_PANNING, BPBXSYN_PANNING_PARAM_COUNT);
    return INSTR_INVALID_ID;

    #undef check
}

bool instr_set_param(instrument_s *instr, instr_param_id id, double value) {
    assert(id != INSTR_INVALID_ID);
    if (id == INSTR_INVALID_ID) return false;

    instr_module_e module;
    instr_param_id idx;
    instr_local_id(id, &module, &idx);

    switch (module) {
        case INSTR_MODULE_SYNTH:
            return !bpbxsyn_synth_set_param_double(instr->synth, idx, value);
        
        case INSTR_MODULE_CONTROL:
            switch (idx) {
                case INSTR_CPARAM_GAIN:
                    instr->gain = value;
                    break;
                
                case INSTR_CPARAM_TEMPO_MULTIPLIER:
                    instr->tempo_multiplier = value;
                    break;
                
                case INSTR_CPARAM_TEMPO_USE_OVERRIDE:
                    instr->tempo_use_override = value != 0.0;
                    break;
                
                case INSTR_CPARAM_TEMPO_OVERRIDE:
                    instr->tempo_override = value;
                    break;
                
                default:
                    return false;
            }
            return true;
        
        case INSTR_MODULE_PANNING:
            return !bpbxsyn_effect_set_param_double(instr->fx_panning, idx, value);
        
        default:
            return false;
    }
}

bool instr_get_param(const instrument_s *instr, instr_param_id id, double *value) {
    assert(id != INSTR_INVALID_ID);
    if (id == INSTR_INVALID_ID) return false;

    instr_module_e module;
    instr_param_id idx;
    instr_local_id(id, &module, &idx);

    switch (module) {
        case INSTR_MODULE_SYNTH:
            return !bpbxsyn_synth_get_param_double(instr->synth, idx, value);
        
        case INSTR_MODULE_CONTROL:
            switch (idx) {
                case INSTR_CPARAM_GAIN:
                    *value = instr->gain;
                    break;
                
                case INSTR_CPARAM_TEMPO_MULTIPLIER:
                    *value = instr->tempo_multiplier;
                    break;
                
                case INSTR_CPARAM_TEMPO_USE_OVERRIDE:
                    *value = instr->tempo_use_override ? 1.0 : 0.0;
                    break;
                
                case INSTR_CPARAM_TEMPO_OVERRIDE:
                    *value = instr->tempo_override;
                    break;
                
                default:
                    return false;
            }
            return true;
        
        case INSTR_MODULE_PANNING:
            return !bpbxsyn_effect_get_param_double(instr->fx_panning, idx, value);
        
        default:
            return false;
    }
}

const bpbxsyn_param_info_s* instr_get_param_info(const instrument_s *instr,
                                                 instr_param_id id)
{
    assert(id != INSTR_INVALID_ID);
    if (id == INSTR_INVALID_ID) return NULL;

    instr_module_e module;
    instr_param_id idx;
    instr_local_id(id, &module, &idx);

    switch (module) {
        case INSTR_MODULE_SYNTH:
            return bpbxsyn_synth_param_info(instr->type, idx);
        
        case INSTR_MODULE_CONTROL:
            return &control_param_info[idx];
        
        case INSTR_MODULE_PANNING:
            return bpbxsyn_effect_param_info(BPBXSYN_EFFECT_PANNING, idx);
        
        default:
            return false;
    }
}

// int instr_global_id(instr_module_e module, instr_param_id local_index) {
//     assert(module < INSTR_MODULE_COUNT);
//     assert(INSTR_MODULE_COUNT <= 0xF);

//     return (local_index & 0xFFFF) | (module << 16);
// }

void instr_local_id(instr_param_id global_id, instr_module_e *out_module,
                    instr_param_id *out_local_index)
{
    if (out_local_index)
        *out_local_index = global_id & 0xFFFF;
    if (out_module)
        *out_module = global_id >> 16;
}


const char *bool_enum_values[] = { "Off", "On" };
bpbxsyn_param_info_s control_param_info[INSTR_CPARAM_COUNT] = {
    {
        .group = "Control",
        .name = "Gain",
        .id = "ctGain\0\0",
        .type = BPBXSYN_PARAM_DOUBLE,

        .min_value = -10.0,
        .max_value = 10.0,
        .default_value = 0.0,

        .flags = BPBXSYN_PARAM_FLAG_NO_AUTOMATION,
    },
    {
        .group = "Control",
        .name = "Force Tempo",
        .id = "ctTmpMod",
        .type = BPBXSYN_PARAM_UINT8,

        .min_value = 0,
        .max_value = 1,
        .default_value = 0,

        .flags = BPBXSYN_PARAM_FLAG_NO_AUTOMATION,
        .enum_values = bool_enum_values,
    },
    {
        .group = "Control",
        .name = "Tempo Multiplier",
        .id = "ctTmpMul",
        .type = BPBXSYN_PARAM_DOUBLE,

        .min_value = 0.0,
        .max_value = 10.0,
        .default_value = 1.0,

        .flags = BPBXSYN_PARAM_FLAG_NO_AUTOMATION
    },
    {
        .group = "Control",
        .name = "Tempo Force Value",
        .id = "ctTmpOvr",
        .type = BPBXSYN_PARAM_DOUBLE,

        .min_value = 1.0,
        .max_value = 500.0,
        .default_value = 150.0,

        .flags = BPBXSYN_PARAM_FLAG_NO_AUTOMATION
    },
};