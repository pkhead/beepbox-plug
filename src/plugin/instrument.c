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

static inline bool is_effect(instr_module_e module) {
    return    module >= INSTR_FIRST_EFFECT_MODULE
           && module <= INSTR_LAST_EFFECT_MODULE;
}

bpbxsyn_param_info_s control_param_info[INSTR_CPARAM_COUNT];






















bool instr_init(instrument_s *instr, bpbxsyn_context_s *ctx, bpbxsyn_synth_type_e type) {
    *instr = (instrument_s) {
        .type = type,
        .bpm = 150.0,
        .tempo_multiplier = 1.0,
        .tempo_override = 150.0,
        .tempo_use_override = false,
    };

    instr->synth = bpbxsyn_synth_new(ctx, type);
    if (!instr->synth) return false;

    instr->fx.fader = bpbxsyn_effect_new(ctx, BPBXSYN_EFFECT_VOLUME);
    if (!instr->fx.fader) return false;

    instr->fx.panning = bpbxsyn_effect_new(ctx, BPBXSYN_EFFECT_PANNING);
    if (!instr->fx.panning) return false;

    instr->fx.distortion = bpbxsyn_effect_new(ctx, BPBXSYN_EFFECT_DISTORTION);
    if (!instr->fx.distortion) return false;

    instr->fx.bitcrusher = bpbxsyn_effect_new(ctx, BPBXSYN_EFFECT_BITCRUSHER);
    if (!instr->fx.bitcrusher) return false;

    instr->fx.eq = bpbxsyn_effect_new(ctx, BPBXSYN_EFFECT_EQ);
    if (!instr->fx.eq) return false;

    instr->fx.chorus = bpbxsyn_effect_new(ctx, BPBXSYN_EFFECT_CHORUS);
    if (!instr->fx.chorus) return false;

    instr->fx.echo = bpbxsyn_effect_new(ctx, BPBXSYN_EFFECT_ECHO);
    if (!instr->fx.echo) return false;

    instr->fx.reverb = bpbxsyn_effect_new(ctx, BPBXSYN_EFFECT_REVERB);
    if (!instr->fx.reverb) return false;

    return true;
}

void instr_destroy(instrument_s *instr) {
    // caller should have called this before, but do it again just in case.
    instr_deactivate(instr);

    bpbxsyn_synth_destroy(instr->synth);
    for (int i = 0; i < INSTR_EFFECT_MODULE_COUNT; ++i) {
        bpbxsyn_effect_destroy(instr->effect_modules[i]);
    }
}

bool instr_activate(instrument_s *instr, double sample_rate,
                    uint32_t max_frames_count)
{
    instr->sample_rate = sample_rate;

    bpbxsyn_synth_set_sample_rate(instr->synth, sample_rate);

    for (int i = 0; i < INSTR_EFFECT_MODULE_COUNT; ++i) {
        if (instr->effect_modules[i])
            bpbxsyn_effect_set_sample_rate(instr->effect_modules[i], sample_rate);
    }

    // allocate process blocks
    instr->synth_mono_buffer = malloc(max_frames_count * sizeof(float));
    for (int i = 0; i < 2; ++i) {
        free(instr->process_block[i]);
        instr->process_block[i] = malloc(max_frames_count * sizeof(float));
        if (!instr->process_block[i]) return false;
    }

    return true;
}

bool instr_deactivate(instrument_s *instr) {
    // free process blocks
    free(instr->synth_mono_buffer);
    instr->synth_mono_buffer = NULL;

    for (int i = 0; i < 2; ++i) {
        free(instr->process_block[i]);

        instr->process_block[i] = NULL;
    }

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
    // take a (theoretically) infinite amount of time to process a tick.
    // then this means any subsequent ticks will not be processed.
    // also completely freezes processing somehow
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
            bpbxsyn_tick_ctx_s tick_ctx = (bpbxsyn_tick_ctx_s) {
                .bpm = active_bpm,
                .beat = instr->cur_beat,
            };

            bpbxsyn_synth_tick(instr->synth, &tick_ctx);

            // tick panning, eq, and fader
            bpbxsyn_effect_tick(instr->fx.panning, &tick_ctx);
            bpbxsyn_effect_tick(instr->fx.eq, &tick_ctx);
            bpbxsyn_effect_tick(instr->fx.fader, &tick_ctx);

            if (instr->use_distortion)
                bpbxsyn_effect_tick(instr->fx.distortion, &tick_ctx);

            if (instr->use_bitcrusher)
                bpbxsyn_effect_tick(instr->fx.bitcrusher, &tick_ctx);

            if (instr->use_chorus)
                bpbxsyn_effect_tick(instr->fx.chorus, &tick_ctx);

            if (instr->use_echo)
                bpbxsyn_effect_tick(instr->fx.echo, &tick_ctx);
            
            if (instr->use_reverb)
                bpbxsyn_effect_tick(instr->fx.reverb, &tick_ctx);

            instr->frames_until_next_tick =
                (uint32_t)ceil(bpbxsyn_calc_samples_per_tick(active_bpm, instr->sample_rate));

            instr->cur_beat += beats_per_sec * sample_len * instr->frames_until_next_tick;
        }

        // render the mono audio into process_block[1]
        uint32_t frames_to_process = frame_count - i;
        if (instr->frames_until_next_tick < frames_to_process)
            frames_to_process = instr->frames_until_next_tick;

        bpbxsyn_synth_run(instr->synth, instr->synth_mono_buffer + i, frames_to_process);

        // convert mono audio from process_block[1] into process_block[0]
        float *process_block[2];
        process_block[0] = instr->process_block[0] + i;
        process_block[1] = instr->process_block[1] + i;

        for (uint32_t j = 0; j < frames_to_process; ++j) {
            const float v = instr->synth_mono_buffer[i+j];
            process_block[0][j] = v;
            process_block[1][j] = 0.0;
        }

        // perform effect processing

        // mono effects first
        // distortion, bitcrusher
        if (instr->use_distortion)
            bpbxsyn_effect_run(instr->fx.distortion, process_block, frames_to_process);
        if (instr->use_bitcrusher)
            bpbxsyn_effect_run(instr->fx.bitcrusher, process_block, frames_to_process);

        // eq
        bpbxsyn_effect_run(instr->fx.eq, process_block, frames_to_process);

        // then, run panning, which converts to stereo
        bpbxsyn_effect_run(instr->fx.panning, process_block, frames_to_process);

        // then, stereo effects
        // chorus, echo and reverb
        if (instr->use_chorus)
            bpbxsyn_effect_run(instr->fx.chorus, process_block, frames_to_process);

        if (instr->use_echo)
            bpbxsyn_effect_run(instr->fx.echo, process_block, frames_to_process);
        
        if (instr->use_reverb)
            bpbxsyn_effect_run(instr->fx.reverb, process_block, frames_to_process);

        // and last, volume control
        bpbxsyn_effect_run(instr->fx.fader, process_block, frames_to_process);

        i += frames_to_process;
        inst_proc.cur_sample += frames_to_process;
        instr->frames_until_next_tick -= frames_to_process;
    }

    // write output
    float control_gain = (float)instr->linear_gain;
    for (uint32_t i = 0; i < frame_count; ++i) {
        output[0][i] = instr->process_block[0][i] * control_gain;
        output[1][i] = instr->process_block[1][i] * control_gain;
    }

    bpbxsyn_synth_set_userdata(instr->synth, NULL);
}

// bool instr_is_module_active(const instrument_s *instr, instr_module_e module) {
//     // these modules cannot be deactivated
//     if (module == INSTR_MODULE_SYNTH || module == INSTR_MODULE_CONTROL)
//         return true;

//     return instr->active_modules[module];
// }

void instr_set_effect_active(instrument_s *instr, bpbxsyn_effect_type_e effect,
                             bool value)
{
    #define HANDLE_EFFECT(name) \
        if (instr->use_##name != value) { \
            if (!value) { \
                bpbxsyn_effect_stop(instr->fx.name); \
            } \
            instr->use_##name = value; \
        } \

    switch (effect) {
        case BPBXSYN_EFFECT_DISTORTION:
            HANDLE_EFFECT(distortion);
            break;

        case BPBXSYN_EFFECT_BITCRUSHER:
            HANDLE_EFFECT(bitcrusher);
            break;

        case BPBXSYN_EFFECT_CHORUS:
            HANDLE_EFFECT(chorus);
            break;

        case BPBXSYN_EFFECT_ECHO:
            HANDLE_EFFECT(echo);
            break;

        case BPBXSYN_EFFECT_REVERB:
            HANDLE_EFFECT(reverb);
            break;
        
        default: break;
    }

    #undef HANDLE_EFFECT
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
    uint32_t count =
        bpbxsyn_synth_param_count(instr->type)
        + INSTR_CPARAM_COUNT
        + BPBXSYN_VOLUME_PARAM_COUNT
        + BPBXSYN_PANNING_PARAM_COUNT
        + BPBXSYN_EQ_PARAM_COUNT
        + BPBXSYN_DISTORTION_PARAM_COUNT
        + BPBXSYN_BITCRUSHER_PARAM_COUNT
        + BPBXSYN_CHORUS_PARAM_COUNT
        + BPBXSYN_ECHO_PARAM_COUNT
        + BPBXSYN_REVERB_PARAM_COUNT;
    // TODO: add reverb for param count

    // for (int i = 0; i < BPBXSYN_EFFECT_COUNT; ++i) {
    //     count += bpbxsyn_effect_param_count(i);
    // }

    return count;
}

instr_param_id instr_get_param_id(const instrument_s *instr, uint32_t index) {
    #define check(module, count) \
        if (index < count) \
            return instr_global_id(module, index); \
        index -= count
    
    #define check_within(module, start, count) \
        if (index < (count)) \
            return instr_global_id((module), (start) + (index)); \
        index -= (count)
    
    #define check1(module, pindex) \
        if (index == 0) \
            return instr_global_id((module), (pindex)); \
        index--

    unsigned int synth_param_count = bpbxsyn_synth_param_count(instr->type);

    // control parameters
    check_within(INSTR_MODULE_CONTROL, 0, INSTR_CPARAM_ENABLE_DISTORTION);
    
    // volume and panning
    check(INSTR_MODULE_VOLUME, BPBXSYN_VOLUME_PARAM_COUNT);
    check(INSTR_MODULE_PANNING, BPBXSYN_PANNING_PARAM_COUNT);

    // synth general parameters
    const int note_effect_start = BPBXSYN_PARAM_ENABLE_TRANSITION_TYPE;
    check_within(INSTR_MODULE_SYNTH, 0, note_effect_start);

    // synth params
    check_within(INSTR_MODULE_SYNTH, BPBXSYN_BASE_PARAM_COUNT, synth_param_count - BPBXSYN_BASE_PARAM_COUNT);

    // synth note effect parameters
    check_within(INSTR_MODULE_SYNTH, note_effect_start, BPBXSYN_BASE_PARAM_COUNT - note_effect_start);

    // eq
    check(INSTR_MODULE_EQ, BPBXSYN_EQ_PARAM_COUNT);

    // distortion
    check1(INSTR_MODULE_CONTROL, INSTR_CPARAM_ENABLE_DISTORTION);
    check(INSTR_MODULE_DISTORTION, BPBXSYN_DISTORTION_PARAM_COUNT);

    // bitcrusher
    check1(INSTR_MODULE_CONTROL, INSTR_CPARAM_ENABLE_BITCRUSHER);
    check(INSTR_MODULE_BITCRUSHER, BPBXSYN_BITCRUSHER_PARAM_COUNT);

    // chorus
    check1(INSTR_MODULE_CONTROL, INSTR_CPARAM_ENABLE_CHORUS);
    check(INSTR_MODULE_CHORUS, BPBXSYN_CHORUS_PARAM_COUNT);

    // echo
    check1(INSTR_MODULE_CONTROL, INSTR_CPARAM_ENABLE_ECHO);
    check(INSTR_MODULE_ECHO, BPBXSYN_ECHO_PARAM_COUNT);

    // reverb
    check1(INSTR_MODULE_CONTROL, INSTR_CPARAM_ENABLE_REVERB);
    check(INSTR_MODULE_REVERB, BPBXSYN_REVERB_PARAM_COUNT);

    return INSTR_INVALID_ID;

    #undef check
}

bool instr_set_param(instrument_s *instr, instr_param_id id, double *value) {
    #define HANDLE_EFFECT(e) \
        case INSTR_CPARAM_ENABLE_##e: \
            instr_set_effect_active(instr, BPBXSYN_EFFECT_##e, *value != 0.0); \
            *value = *value != 0.0 ? 1.0 : 0.0; \
            break;

    assert(id != INSTR_INVALID_ID);
    if (id == INSTR_INVALID_ID) return false;

    instr_module_e module;
    instr_param_id idx;
    instr_local_id(id, &module, &idx);

    switch (module) {
        case INSTR_MODULE_SYNTH: {
            const bpbxsyn_param_info_s *info =
                bpbxsyn_synth_param_info(instr->type, idx);
            assert(info);
            if (!info) return false;

            switch (info->type) {
                case BPBXSYN_PARAM_DOUBLE:
                    return !bpbxsyn_synth_set_param_double(instr->synth, idx, *value);
                
                case BPBXSYN_PARAM_INT:
                case BPBXSYN_PARAM_UINT8:
                    *value = round(*value);
                    return !bpbxsyn_synth_set_param_int(instr->synth, idx, (int)*value);
            }
        }
        
        case INSTR_MODULE_CONTROL:
            switch (idx) {
                case INSTR_CPARAM_GAIN:
                    instr->gain = *value;
                    break;
                
                case INSTR_CPARAM_TEMPO_MULTIPLIER:
                    instr->tempo_multiplier = *value;
                    break;
                
                case INSTR_CPARAM_TEMPO_USE_OVERRIDE:
                    instr->tempo_use_override = *value != 0.0;
                    *value = *value != 0.0 ? 1.0 : 0.0;
                    break;
                
                case INSTR_CPARAM_TEMPO_OVERRIDE:
                    instr->tempo_override = *value;
                    break;
                
                HANDLE_EFFECT(DISTORTION)
                HANDLE_EFFECT(BITCRUSHER)
                HANDLE_EFFECT(CHORUS)
                HANDLE_EFFECT(ECHO)
                HANDLE_EFFECT(REVERB)
                
                default:
                    return false;
            }
            return true;
        
        default:
            if (is_effect(module) && instr->effect_modules[module - INSTR_FIRST_EFFECT_MODULE]) {
                bpbxsyn_effect_s *effect =
                    instr->effect_modules[module - INSTR_FIRST_EFFECT_MODULE];
                
                const bpbxsyn_param_info_s *info =
                    bpbxsyn_effect_param_info(module - INSTR_FIRST_EFFECT_MODULE, idx);
                assert(info);
                if (!info) return false;

                switch (info->type) {
                    case BPBXSYN_PARAM_DOUBLE:
                        return !bpbxsyn_effect_set_param_double(effect, idx, *value);
                    
                    case BPBXSYN_PARAM_INT:
                    case BPBXSYN_PARAM_UINT8:
                        *value = round(*value);
                        return !bpbxsyn_effect_set_param_int(effect, idx, (int)*value);
                }
            }

            return false;
    }

    #undef HANDLE_EFFECT
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
                
                case INSTR_CPARAM_ENABLE_DISTORTION:
                    *value = instr->use_distortion ? 1.0 : 0.0;
                    break;
                case INSTR_CPARAM_ENABLE_BITCRUSHER:
                    *value = instr->use_bitcrusher ? 1.0 : 0.0;
                    break;
                case INSTR_CPARAM_ENABLE_CHORUS:
                    *value = instr->use_chorus ? 1.0 : 0.0;
                    break;
                case INSTR_CPARAM_ENABLE_ECHO:
                    *value = instr->use_echo ? 1.0 : 0.0;
                    break;
                case INSTR_CPARAM_ENABLE_REVERB:
                    *value = instr->use_reverb ? 1.0 : 0.0;
                    break;
                
                default:
                    return false;
            }
            return true;
        
        default:
            if (is_effect(module) && instr->effect_modules[module - INSTR_FIRST_EFFECT_MODULE]) {
                return !bpbxsyn_effect_get_param_double(
                    instr->effect_modules[module - INSTR_FIRST_EFFECT_MODULE], idx, value);
            }

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
        
        default:
            if (is_effect(module))
                return bpbxsyn_effect_param_info(
                    module - INSTR_FIRST_EFFECT_MODULE, idx);
            
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


static const char *bool_enum_values[] = { "Off", "On" };
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
    
    {
        .group = "Effects/Distortion",
        .name = "Distortion Toggle",
        .id = "ctDistor",
        .type = BPBXSYN_PARAM_UINT8,
        .flags = BPBXSYN_PARAM_FLAG_NO_AUTOMATION,

        .min_value = 0,
        .max_value = 1,
        .default_value = 0,

        .enum_values = bool_enum_values
    },
    {
        .group = "Effects/Bitcrusher",
        .name = "Bitcrusher Toggle",
        .id = "ctBitcru",
        .type = BPBXSYN_PARAM_UINT8,
        .flags = BPBXSYN_PARAM_FLAG_NO_AUTOMATION,

        .min_value = 0,
        .max_value = 1,
        .default_value = 0,

        .enum_values = bool_enum_values
    },
    {
        .group = "Effects/Chorus",
        .name = "Chorus Toggle",
        .id = "ctChorus",
        .type = BPBXSYN_PARAM_UINT8,
        .flags = BPBXSYN_PARAM_FLAG_NO_AUTOMATION,

        .min_value = 0,
        .max_value = 1,
        .default_value = 0,

        .enum_values = bool_enum_values
    },
    {
        .group = "Effects/Echo",
        .name = "Echo Toggle",
        .id = "ctEcho\0\0",
        .type = BPBXSYN_PARAM_UINT8,
        .flags = BPBXSYN_PARAM_FLAG_NO_AUTOMATION,

        .min_value = 0,
        .max_value = 1,
        .default_value = 0,

        .enum_values = bool_enum_values
    },
    {
        .group = "Effects/Reverb",
        .name = "Reverb Toggle",
        .id = "ctReverb",
        .type = BPBXSYN_PARAM_UINT8,
        .flags = BPBXSYN_PARAM_FLAG_NO_AUTOMATION,

        .min_value = 0,
        .max_value = 1,
        .default_value = 0,

        .enum_values = bool_enum_values
    },
};