#include "include/instrument.h"
#include "instrument_impl.h"

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

static void bbxs_voice_end_cb(bbxs_synth_s *inst, bbxs_voice_id id) {
   inst_process_userdata_s *ud = bbxs_synth_get_userdata(inst);
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

static const bbxs_param_info_s control_param_info[INSTR_CPARAM_COUNT];
static const bbxs_param_info_s unused_param_info;






















int instr_synth_type_index(bbxs_synth_type_e type) {
    int type_idx = -1;
    for (int i = 0; i < BBXS_SYNTH_COUNT; ++i) {
        if (instr_synth_type_values[i] == type)
            type_idx = i;
    }

    return type_idx;
}

bbxs_synth_s* instr_get_synth(const instrument_s *instr) {
    assert(instr);
    return instr->synth;
}

bool instr_init(instrument_s *instr, bbxs_context_s *ctx, bbxs_synth_type_e type) {
    // calculate type index. type index uses different values than the values
    // for the type enums.
    int type_idx = instr_synth_type_index(type);

    // could not calculate type index for whatever reason, so signal error.
    // (probably that the type is unimplemented, but this case should be
    // impossible in v1.0.0 and later)
    assert(type_idx != -1);
    if (type_idx == -1) {
        return false;
    }

    assert(type_idx >= 0 && type_idx <= UINT8_MAX);

    *instr = (instrument_s) {
        .type = type,
        .type_index = (uint8_t)type_idx,
        .new_type_index = (uint8_t)type_idx,
        .bpm = 150.0,
        .tempo_multiplier = 1.0,
        .tempo_override = 150.0,
        .tempo_use_override = false,
    };

    instr->synth = bbxs_synth_new(ctx, type);
    if (!instr->synth) return false;

    instr->fx.fader = bbxs_effect_new(ctx, BBXS_EFFECT_VOLUME);
    if (!instr->fx.fader) return false;

    instr->fx.panning = bbxs_effect_new(ctx, BBXS_EFFECT_PANNING);
    if (!instr->fx.panning) return false;

    instr->fx.distortion = bbxs_effect_new(ctx, BBXS_EFFECT_DISTORTION);
    if (!instr->fx.distortion) return false;

    instr->fx.bitcrusher = bbxs_effect_new(ctx, BBXS_EFFECT_BITCRUSHER);
    if (!instr->fx.bitcrusher) return false;

    instr->fx.eq = bbxs_effect_new(ctx, BBXS_EFFECT_EQ);
    if (!instr->fx.eq) return false;

    instr->fx.chorus = bbxs_effect_new(ctx, BBXS_EFFECT_CHORUS);
    if (!instr->fx.chorus) return false;

    instr->fx.echo = bbxs_effect_new(ctx, BBXS_EFFECT_ECHO);
    if (!instr->fx.echo) return false;

    instr->fx.reverb = bbxs_effect_new(ctx, BBXS_EFFECT_REVERB);
    if (!instr->fx.reverb) return false;

    return true;
}

void instr_destroy(instrument_s *instr) {
    // caller should have called this before, but do it again just in case.
    instr_deactivate(instr);

    bbxs_synth_destroy(instr->synth);
    for (int i = 0; i < INSTR_EFFECT_MODULE_COUNT; ++i) {
        bbxs_effect_destroy(instr->effect_modules[i]);
    }
}

static bool get_synth_unison(const bbxs_synth_s *syn, int *value) {
    bool s;
    switch (bbxs_synth_type(syn)) {
        case BBXS_SYNTH_CHIP:
            if (!value) return true;
            s = bbxs_synth_get_param_int(syn,
                                            BBXS_CHIP_PARAM_UNISON,
                                            value);
            break;
        case BBXS_SYNTH_HARMONICS:
            if (!value) return true;
            s = bbxs_synth_get_param_int(syn,
                                            BBXS_HARMONICS_PARAM_UNISON,
                                            value);
            break;
        case BBXS_SYNTH_PICKED_STRING:
            if (!value) return true;
            s = bbxs_synth_get_param_int(syn,
                                            BBXS_PICKED_STRING_PARAM_UNISON,
                                            value);
            break;
        case BBXS_SYNTH_NOISE:
            if (!value) return true;
            s = bbxs_synth_get_param_int(syn,
                                            BBXS_NOISE_PARAM_UNISON,
                                            value);
            break;
        
        default: return false;
    }

    return s == 0;
}

static bool set_synth_unison(bbxs_synth_s *syn, int value) {
    bool s;
    switch (bbxs_synth_type(syn)) {
        case BBXS_SYNTH_CHIP:
            s = bbxs_synth_set_param_int(syn,
                                            BBXS_CHIP_PARAM_UNISON,
                                            value);
            break;
        case BBXS_SYNTH_HARMONICS:
            s = bbxs_synth_set_param_int(syn,
                                            BBXS_HARMONICS_PARAM_UNISON,
                                            value);
            break;
        case BBXS_SYNTH_PICKED_STRING:
            s = bbxs_synth_set_param_int(syn,
                                            BBXS_PICKED_STRING_PARAM_UNISON,
                                            value);
            break;
        case BBXS_SYNTH_NOISE:
            s = bbxs_synth_set_param_int(syn,
                                            BBXS_NOISE_PARAM_UNISON,
                                            value);
            break;
        
        default: return false;
    }

    return s == 0;
}

bool copy_synth_config(const bbxs_synth_s *src, bbxs_synth_s *dst) {
    bbxs_synth_type_e src_type = bbxs_synth_type(src);
    bbxs_synth_type_e dst_type = bbxs_synth_type(dst);

    // copy base parameters
    for (uint32_t i = 0; i < BBXS_BASE_PARAM_COUNT; ++i) {
        double v;
        if (bbxs_synth_get_param_double(src, i, &v))
            return false;

        if (bbxs_synth_set_param_double(dst, i, v))
            return false;
    }
    
    // retain unison parameter
    int src_unison;
    if (get_synth_unison(src, &src_unison) && get_synth_unison(dst, NULL))
        set_synth_unison(dst, src_unison);

    // retain harmonics data between harmonic and picked string instrument types
    if ((src_type == BBXS_SYNTH_HARMONICS && dst_type == BBXS_SYNTH_PICKED_STRING))
    {
        for (int i = 0; i < BBXS_HARMONICS_CONTROL_COUNT; ++i)
        {
            int v;
            int s = bbxs_synth_get_param_int(
                src, BBXS_HARMONICS_PARAM_CONTROL_FIRST + i, &v);

            assert(s == 0);
            if (s != 0) return false;
            
            s = bbxs_synth_set_param_int(
                dst, BBXS_PICKED_STRING_PARAM_CONTROL_FIRST + i, v);
            
            assert(s == 0);
            if (s != 0) return false;
        }
    }
    else if ((src_type == BBXS_SYNTH_PICKED_STRING && dst_type == BBXS_SYNTH_HARMONICS))
    {
        for (int i = 0; i < BBXS_HARMONICS_CONTROL_COUNT; ++i)
        {
            int v;
            int s = bbxs_synth_get_param_int(
                src, BBXS_PICKED_STRING_PARAM_CONTROL_FIRST + i, &v);

            assert(s == 0);
            if (s != 0) return false;
            
            s = bbxs_synth_set_param_int(
                dst, BBXS_HARMONICS_PARAM_CONTROL_FIRST + i, v);
            
            assert(s == 0);
            if (s != 0) return false;
        }
    }

    // copy compatible envelope data
    const bbxs_envelope_s *src_envelopes =
        bbxs_synth_get_envelope((bbxs_synth_s*)src, 0);
    
    // get both src and dst envelope targets. voice effect targets are
    // not included in the list, so i need to determine if it's a generator
    // envelope target by seeing if it's in the source target list.
    int src_env_target_count;
    const bbxs_envelope_compute_index_e *src_env_targets =
        bbxs_synth_envelope_targets(src_type, &src_env_target_count);
    
    int dst_env_target_count;
    const bbxs_envelope_compute_index_e *dst_env_targets =
        bbxs_synth_envelope_targets(dst_type,  &dst_env_target_count);
    
    for (uint8_t i = 0; i < bbxs_synth_envelope_count(src); ++i) {
        const bbxs_envelope_s *src_envelope = src_envelopes + i;
        bbxs_envelope_s *dst_envelope = bbxs_synth_add_envelope(dst);
        *dst_envelope = *src_envelope;

        bool is_src_target = false;
        bool is_dst_target = false;

        for (int i = 0; i < src_env_target_count; ++i) {
            if (src_env_targets[i] == src_envelope->index) {
                is_src_target = true;
                break;
            }
        }

        for (int i = 0; i < dst_env_target_count; ++i) {
            if (dst_env_targets[i] == src_envelope->index) {
                is_dst_target = true;
                break;
            }
        }

        if (is_src_target && !is_dst_target) {
            dst_envelope->index = BBXS_ENV_INDEX_NONE;
        }
    }

    return true;
}

void instr_on_main_thread(instrument_s *instr) {
    assert(instr->clap_host);
    assert(instr->clap_host_params);

    if (atomic_exchange(&instr->need_param_rescan, false)) {
        if (instr->clap_host_params)
            instr->clap_host_params->rescan(instr->clap_host,
                                            CLAP_PARAM_RESCAN_ALL);
    }
}

bool instr_activate(instrument_s *instr, bbxs_context_s *ctx,
                    double sample_rate, uint32_t max_frames_count) {
    assert(instr->clap_host);
    assert(instr->clap_host_params);

    // load new instrument type when requested
    if (instr->new_type_index != instr->type_index && instr->synth) {
        bbxs_synth_type_e new_type = instr_synth_type_values[instr->new_type_index];
        assert(new_type != -1);
        if (new_type == -1) {
            instr->type = instr->new_type_index;
            return false;
        }

        bbxs_synth_s *new_synth = bbxs_synth_new(ctx, new_type);
        if (!new_synth) {
            instr->type = instr->new_type_index;
            return false;    
        }

        if (!copy_synth_config(instr->synth, new_synth))
            return false;

        instr->type_index = instr->new_type_index;
        instr->type = new_type;
        
        bbxs_synth_destroy(instr->synth);
        instr->synth = new_synth;

        atomic_store(&instr->need_param_rescan, true);
        instr->clap_host->request_callback(instr->clap_host);
        
        instr->frames_until_next_tick = 0;
    }

    instr->sample_rate = sample_rate;

    bbxs_synth_set_sample_rate(instr->synth, sample_rate);

    for (int i = 0; i < INSTR_EFFECT_MODULE_COUNT; ++i) {
        if (instr->effect_modules[i])
            bbxs_effect_set_sample_rate(instr->effect_modules[i], sample_rate);
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
    bbxs_synth_set_userdata(instr->synth, &inst_proc);

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
            bbxs_tick_ctx_s tick_ctx = (bbxs_tick_ctx_s) {
                .bpm = active_bpm,
                .beat = instr->cur_beat,
            };

            bbxs_synth_tick(instr->synth, &tick_ctx);

            // tick panning, eq, and fader
            bbxs_effect_tick(instr->fx.panning, &tick_ctx);
            bbxs_effect_tick(instr->fx.eq, &tick_ctx);
            bbxs_effect_tick(instr->fx.fader, &tick_ctx);

            instr->run_distortion = instr->use_distortion;
            if (instr->use_distortion)
                bbxs_effect_tick(instr->fx.distortion, &tick_ctx);

            instr->run_bitcrusher = instr->use_bitcrusher;
            if (instr->use_bitcrusher)
                bbxs_effect_tick(instr->fx.bitcrusher, &tick_ctx);

            instr->run_chorus = instr->use_chorus;
            if (instr->use_chorus)
                bbxs_effect_tick(instr->fx.chorus, &tick_ctx);

            instr->run_echo = instr->use_echo;
            if (instr->use_echo)
                bbxs_effect_tick(instr->fx.echo, &tick_ctx);
            
            instr->run_reverb = instr->use_reverb;
            if (instr->use_reverb)
                bbxs_effect_tick(instr->fx.reverb, &tick_ctx);

            instr->frames_until_next_tick =
                (uint32_t)ceil(bbxs_calc_samples_per_tick(active_bpm, instr->sample_rate));

            instr->cur_beat += beats_per_sec * sample_len * instr->frames_until_next_tick;
        }

        // render the mono audio into process_block[1]
        uint32_t frames_to_process = frame_count - i;
        if (instr->frames_until_next_tick < frames_to_process)
            frames_to_process = instr->frames_until_next_tick;

        bbxs_synth_run(instr->synth, instr->synth_mono_buffer + i, frames_to_process);

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
        if (instr->run_distortion)
            bbxs_effect_run(instr->fx.distortion, process_block, frames_to_process);
        if (instr->run_bitcrusher)
            bbxs_effect_run(instr->fx.bitcrusher, process_block, frames_to_process);

        // eq
        bbxs_effect_run(instr->fx.eq, process_block, frames_to_process);

        // then, run panning, which converts to stereo
        bbxs_effect_run(instr->fx.panning, process_block, frames_to_process);

        // then, stereo effects
        // chorus, echo and reverb
        if (instr->run_chorus)
            bbxs_effect_run(instr->fx.chorus, process_block, frames_to_process);

        if (instr->run_echo)
            bbxs_effect_run(instr->fx.echo, process_block, frames_to_process);
        
        if (instr->run_reverb)
            bbxs_effect_run(instr->fx.reverb, process_block, frames_to_process);

        // and last, volume control
        bbxs_effect_run(instr->fx.fader, process_block, frames_to_process);

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

    bbxs_synth_set_userdata(instr->synth, NULL);
}

// bool instr_is_module_active(const instrument_s *instr, instr_module_e module) {
//     // these modules cannot be deactivated
//     if (module == INSTR_MODULE_SYNTH || module == INSTR_MODULE_CONTROL)
//         return true;

//     return instr->active_modules[module];
// }

void instr_set_effect_active(instrument_s *instr, bbxs_effect_type_e effect,
                             bool value)
{
    #define HANDLE_EFFECT(name)                                                \
        if (instr->use_##name != value) {                                      \
            if (!value) {                                                      \
                bbxs_effect_stop(instr->fx.name);                           \
                instr->run_##name = false;                                     \
            }                                                                  \
            instr->use_##name = value;                                         \
        }

    switch (effect) {
        case BBXS_EFFECT_DISTORTION:
            HANDLE_EFFECT(distortion);
            break;

        case BBXS_EFFECT_BITCRUSHER:
            HANDLE_EFFECT(bitcrusher);
            break;

        case BBXS_EFFECT_CHORUS:
            HANDLE_EFFECT(chorus);
            break;

        case BBXS_EFFECT_ECHO:
            HANDLE_EFFECT(echo);
            break;

        case BBXS_EFFECT_REVERB:
            HANDLE_EFFECT(reverb);
            break;
        
        default: break;
    }

    #undef HANDLE_EFFECT
}

void instr_begin_note(instrument_s *instr, int16_t key, double velocity,
                      int32_t note_id, int16_t port_index, int16_t channel)
{
    bbxs_voice_id bbxs_id = bbxs_synth_begin_note(
        instr->synth, key, velocity,
        BBXS_NOTE_LENGTH_UNKNOWN);
   
    instr->voices[bbxs_id] = (voice_s) {
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
    for (int i = 0; i < BBXS_SYNTH_MAX_VOICES; i++) {
        voice_s *voice = &instr->voices[i];
        if (voice->active && voice_match(voice, note_id, channel, port_index, key)) {
            bbxs_synth_end_note(instr->synth, i);
        }
   }
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// use the param count of the generator that has the most number of
// parameters
#define MAX_SYNTH_PARAM_COUNT ((                                               \
        MAX(BBXS_CHIP_PARAM_COUNT,                                          \
        MAX(BBXS_FM_PARAM_COUNT,                                            \
        MAX(BBXS_NOISE_PARAM_COUNT,                                         \
        MAX(BBXS_PULSE_WIDTH_PARAM_COUNT,                                   \
        MAX(BBXS_CUSTOM_CHIP_PARAM_COUNT,                                   \
        MAX(BBXS_HARMONICS_PARAM_COUNT,                                     \
        MAX(BBXS_SPECTRUM_PARAM_COUNT,                                      \
        MAX(BBXS_PICKED_STRING_PARAM_COUNT,                                 \
            BBXS_SUPERSAW_PARAM_COUNT)                                      \
        )))))))                                                                \
    ))

uint32_t instr_params_count(void) {
    return
        BBXS_BASE_PARAM_COUNT
        + MAX_SYNTH_PARAM_COUNT
        + INSTR_CPARAM_COUNT
        + BBXS_VOLUME_PARAM_COUNT
        + BBXS_PANNING_PARAM_COUNT
        + BBXS_EQ_PARAM_COUNT
        + BBXS_DISTORTION_PARAM_COUNT
        + BBXS_BITCRUSHER_PARAM_COUNT
        + BBXS_CHORUS_PARAM_COUNT
        + BBXS_ECHO_PARAM_COUNT
        + BBXS_REVERB_PARAM_COUNT;
}

instr_param_id instr_get_param_id_with_type(bbxs_synth_type_e synth_type,
                                            uint32_t index,
                                            bool *is_inactive) {
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

    if (is_inactive)
        *is_inactive = false;

    // control parameters
    check_within(INSTR_MODULE_CONTROL, 0, INSTR_CPARAM_ENABLE_DISTORTION);
    
    // volume and panning
    check(INSTR_MODULE_VOLUME, BBXS_VOLUME_PARAM_COUNT);
    check(INSTR_MODULE_PANNING, BBXS_PANNING_PARAM_COUNT);

    // synth general parameters
    const int note_effect_start = BBXS_PARAM_ENABLE_TRANSITION_TYPE;
    check_within(INSTR_MODULE_SYNTH, 0, note_effect_start);

    // synth params
    if (index < MAX_SYNTH_PARAM_COUNT && 
        index >= bbxs_synth_param_count(synth_type) - BBXS_BASE_PARAM_COUNT
    ) {
        if (is_inactive) *is_inactive = true;
    }
    check_within(INSTR_MODULE_SYNTH, BBXS_BASE_PARAM_COUNT, MAX_SYNTH_PARAM_COUNT);

    // synth note effect parameters
    check_within(INSTR_MODULE_SYNTH, note_effect_start, BBXS_BASE_PARAM_COUNT - note_effect_start);

    // eq
    check(INSTR_MODULE_EQ, BBXS_EQ_PARAM_COUNT);

    // distortion
    check1(INSTR_MODULE_CONTROL, INSTR_CPARAM_ENABLE_DISTORTION);
    check(INSTR_MODULE_DISTORTION, BBXS_DISTORTION_PARAM_COUNT);

    // bitcrusher
    check1(INSTR_MODULE_CONTROL, INSTR_CPARAM_ENABLE_BITCRUSHER);
    check(INSTR_MODULE_BITCRUSHER, BBXS_BITCRUSHER_PARAM_COUNT);

    // chorus
    check1(INSTR_MODULE_CONTROL, INSTR_CPARAM_ENABLE_CHORUS);
    check(INSTR_MODULE_CHORUS, BBXS_CHORUS_PARAM_COUNT);

    // echo
    check1(INSTR_MODULE_CONTROL, INSTR_CPARAM_ENABLE_ECHO);
    check(INSTR_MODULE_ECHO, BBXS_ECHO_PARAM_COUNT);

    // reverb
    check1(INSTR_MODULE_CONTROL, INSTR_CPARAM_ENABLE_REVERB);
    check(INSTR_MODULE_REVERB, BBXS_REVERB_PARAM_COUNT);

    return INSTR_INVALID_ID;

    #undef check
}

instr_param_id instr_get_param_id(const instrument_s *instr, uint32_t index,
                                  bool *is_inactive) {
    return instr_get_param_id_with_type(instr->type, index, is_inactive);
}

bool instr_set_param(instrument_s *instr, instr_param_id id, double *value) {
    #define HANDLE_EFFECT(e) \
        case INSTR_CPARAM_ENABLE_##e: \
            instr_set_effect_active(instr, BBXS_EFFECT_##e, *value != 0.0); \
            *value = *value != 0.0 ? 1.0 : 0.0; \
            break;

    assert(id != INSTR_INVALID_ID);
    if (id == INSTR_INVALID_ID) return false;

    instr_module_e module;
    instr_param_id idx;
    instr_local_id(id, &module, &idx);

    switch (module) {
        case INSTR_MODULE_SYNTH: {
            assert(idx < BBXS_BASE_PARAM_COUNT + MAX_SYNTH_PARAM_COUNT);
            if (idx >= BBXS_BASE_PARAM_COUNT + MAX_SYNTH_PARAM_COUNT)
                return false;
            
            // unused param, ignore the write
            if (idx >= bbxs_synth_param_count(instr->type))
                return true;

            const bbxs_param_info_s *info =
                bbxs_synth_param_info(instr->type, idx);
            assert(info);
            if (!info) return false;

            switch (info->type) {
                case BBXS_PARAM_DOUBLE:
                    return !bbxs_synth_set_param_double(instr->synth, idx, *value);
                
                case BBXS_PARAM_INT:
                case BBXS_PARAM_UINT8:
                    *value = round(*value);
                    return !bbxs_synth_set_param_int(instr->synth, idx, (int)*value);
            }
        }
        
        case INSTR_MODULE_CONTROL:
            switch (idx) {
                case INSTR_CPARAM_SYNTH_TYPE:
                    instr->new_type_index = (int)(*value);

                    if (instr->new_type_index != instr->type_index) {
                        // -1 means that instrument type is not implemented yet
                        if (instr_synth_type_values[instr->new_type_index] == -1) {
                            instr->new_type_index = instr->type_index;
                        } else if (instr->clap_host->request_restart) {
                            instr->clap_host->request_restart(instr->clap_host);
                        }
                    }

                    break;
                
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
                bbxs_effect_s *effect =
                    instr->effect_modules[module - INSTR_FIRST_EFFECT_MODULE];
                
                const bbxs_param_info_s *info =
                    bbxs_effect_param_info(module - INSTR_FIRST_EFFECT_MODULE, idx);
                assert(info);
                if (!info) return false;

                switch (info->type) {
                    case BBXS_PARAM_DOUBLE:
                        return !bbxs_effect_set_param_double(effect, idx, *value);
                    
                    case BBXS_PARAM_INT:
                    case BBXS_PARAM_UINT8:
                        *value = round(*value);
                        return !bbxs_effect_set_param_int(effect, idx, (int)*value);
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
            assert(idx < BBXS_BASE_PARAM_COUNT + MAX_SYNTH_PARAM_COUNT);
            if (idx >= BBXS_BASE_PARAM_COUNT + MAX_SYNTH_PARAM_COUNT)
                return false;

            // unused param
            if (idx >= bbxs_synth_param_count(instr->type)) {
                *value = 0.0;
                return true;
            }

            return !bbxs_synth_get_param_double(instr->synth, idx, value);
        
        case INSTR_MODULE_CONTROL:
            switch (idx) {
                case INSTR_CPARAM_SYNTH_TYPE:
                    *value = (double)instr->type_index;
                    break;
                
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
                return !bbxs_effect_get_param_double(
                    instr->effect_modules[module - INSTR_FIRST_EFFECT_MODULE], idx, value);
            }

            return false;
    }
}

const bbxs_param_info_s* instr_get_param_info_with_type(
    bbxs_synth_type_e synth_type, instr_param_id id)
{
    assert(id != INSTR_INVALID_ID);
    if (id == INSTR_INVALID_ID) return NULL;

    instr_module_e module;
    instr_param_id idx;
    instr_local_id(id, &module, &idx);

    switch (module) {
        case INSTR_MODULE_SYNTH:
            assert(idx < BBXS_BASE_PARAM_COUNT + MAX_SYNTH_PARAM_COUNT);
            if (idx >= bbxs_synth_param_count(synth_type))
                return &unused_param_info;

            return bbxs_synth_param_info(synth_type, idx);
        
        case INSTR_MODULE_CONTROL:
            return &control_param_info[idx];
        
        default:
            if (is_effect(module))
                return bbxs_effect_param_info(
                    module - INSTR_FIRST_EFFECT_MODULE, idx);
            
            return false;
    }
}

const bbxs_param_info_s* instr_get_param_info(const instrument_s *instr,
                                                 instr_param_id id) {
    return instr_get_param_info_with_type(instr->type, id);
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

static const bbxs_param_info_s unused_param_info = {
    .type = BBXS_PARAM_UINT8,
    .id = "unused\0\0",
    .name = "Unused Parameter",

    .min_value = 0.0,
    .max_value = 1.0,
    .default_value = 0.0
};

static const char *bool_enum_values[] = { "off", "on" };
const bbxs_synth_type_e instr_synth_type_values[BBXS_SYNTH_COUNT] = {
    BBXS_SYNTH_CHIP, BBXS_SYNTH_PULSE_WIDTH, /*BBXS_SYNTH_SUPERSAW*/-1,
    BBXS_SYNTH_HARMONICS, BBXS_SYNTH_PICKED_STRING,
    BBXS_SYNTH_SPECTRUM, BBXS_SYNTH_FM, BBXS_SYNTH_CUSTOM_CHIP,
    BBXS_SYNTH_NOISE
};

static const char *synth_type_enum_values[BBXS_SYNTH_COUNT] = {
    "chip wave", "pulse width", "supersaw", "harmonics", "picked string",
    "spectrum", "FM", "custom chip", "noise"
};

static const bbxs_param_info_s control_param_info[INSTR_CPARAM_COUNT] = {
    {
        .group = "Control",
        .name = "Synth Type",
        .id = "ctType\0\0",
        .type = BBXS_PARAM_UINT8,

        .min_value = 0,
        .max_value = BBXS_SYNTH_COUNT - 1,
        .default_value = 0,

        .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,
        .enum_values = synth_type_enum_values
    },
    {
        .group = "Control",
        .name = "Gain",
        .id = "ctGain\0\0",
        .type = BBXS_PARAM_DOUBLE,

        .min_value = -10.0,
        .max_value = 10.0,
        .default_value = 0.0,

        .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,
    },
    {
        .group = "Control",
        .name = "Force Tempo",
        .id = "ctTmpMod",
        .type = BBXS_PARAM_UINT8,

        .min_value = 0,
        .max_value = 1,
        .default_value = 0,

        .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,
        .enum_values = bool_enum_values,
    },
    {
        .group = "Control",
        .name = "Tempo Multiplier",
        .id = "ctTmpMul",
        .type = BBXS_PARAM_DOUBLE,

        .min_value = 0.0,
        .max_value = 10.0,
        .default_value = 1.0,

        .flags = BBXS_PARAM_FLAG_NO_AUTOMATION
    },
    {
        .group = "Control",
        .name = "Tempo Force Value",
        .id = "ctTmpOvr",
        .type = BBXS_PARAM_DOUBLE,

        .min_value = 1.0,
        .max_value = 500.0,
        .default_value = 150.0,

        .flags = BBXS_PARAM_FLAG_NO_AUTOMATION
    },
    
    {
        .group = "Effects/Distortion",
        .name = "Distortion Toggle",
        .id = "ctDistor",
        .type = BBXS_PARAM_UINT8,
        .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,

        .min_value = 0,
        .max_value = 1,
        .default_value = 0,

        .enum_values = bool_enum_values
    },
    {
        .group = "Effects/Bitcrusher",
        .name = "Bitcrusher Toggle",
        .id = "ctBitcru",
        .type = BBXS_PARAM_UINT8,
        .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,

        .min_value = 0,
        .max_value = 1,
        .default_value = 0,

        .enum_values = bool_enum_values
    },
    {
        .group = "Effects/Chorus",
        .name = "Chorus Toggle",
        .id = "ctChorus",
        .type = BBXS_PARAM_UINT8,
        .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,

        .min_value = 0,
        .max_value = 1,
        .default_value = 0,

        .enum_values = bool_enum_values
    },
    {
        .group = "Effects/Echo",
        .name = "Echo Toggle",
        .id = "ctEcho\0\0",
        .type = BBXS_PARAM_UINT8,
        .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,

        .min_value = 0,
        .max_value = 1,
        .default_value = 0,

        .enum_values = bool_enum_values
    },
    {
        .group = "Effects/Reverb",
        .name = "Reverb Toggle",
        .id = "ctReverb",
        .type = BBXS_PARAM_UINT8,
        .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,

        .min_value = 0,
        .max_value = 1,
        .default_value = 0,

        .enum_values = bool_enum_values
    },
};