#include "instr_device.hpp"

#include <cassert>
#include <cstring>

using namespace bbxplug;

#define EFFECT_MODULE(NAME) (MODULE_##NAME - MODULE_FIRST_EFFECT)

static bbxs_synth_type_e synth_type_values[BBXS_SYNTH_COUNT] = {
    BBXS_SYNTH_CHIP,      BBXS_SYNTH_PULSE_WIDTH,   BBXS_SYNTH_SUPERSAW,
    BBXS_SYNTH_HARMONICS, BBXS_SYNTH_PICKED_STRING, BBXS_SYNTH_SPECTRUM,
    BBXS_SYNTH_FM,        BBXS_SYNTH_CUSTOM_CHIP,   BBXS_SYNTH_NOISE};

static constexpr bbxs_effect_type_e EFFECTS[] = {
    BBXS_EFFECT_PANNING, BBXS_EFFECT_DISTORTION, BBXS_EFFECT_BITCRUSHER,
    BBXS_EFFECT_CHORUS,  BBXS_EFFECT_ECHO,       BBXS_EFFECT_REVERB,
    BBXS_EFFECT_EQ,      BBXS_EFFECT_VOLUME,
};

static int get_synth_type_index(bbxs_synth_type_e type);

instr_device::instr_device(bbxs_context_s *ctx) : bbxs_ctx(ctx) {}

bool instr_device::init(bbxs_synth_type_e p_type, bool enable_switching) {
    type = p_type;

    // calculate type index. type index uses different values than the values
    // for the type enums.
    int type_idx = get_synth_type_index(p_type);

    // could not calculate type index for whatever reason, so signal error.
    // (probably that the type is unimplemented, but this case should be
    // impossible in v1.0.0 and later)
    assert(type_idx != -1);
    if (type_idx == -1) {
        return false;
    }

    assert(type_idx >= 0 && type_idx <= UINT8_MAX);

    this->type_index = (uint8_t)type_idx;
    this->new_type_index = (uint8_t)type_idx;

    synth = bbxs_synth_new(bbxs_ctx, type);
    if (!synth) return false;

    // must be in same order as the MODULE_* enums for effects
    static constexpr bbxs_effect_type_e EFFECTS[] = {
        BBXS_EFFECT_PANNING, BBXS_EFFECT_DISTORTION, BBXS_EFFECT_BITCRUSHER,
        BBXS_EFFECT_CHORUS,  BBXS_EFFECT_ECHO,       BBXS_EFFECT_REVERB,
        BBXS_EFFECT_EQ,      BBXS_EFFECT_VOLUME,
    };

    for (int i = 0; i < sizeof(EFFECTS) / sizeof(*EFFECTS); ++i) {
        enabled_effects[i] = false;
        running_effects[i] = false;

        effect_modules[i] = bbxs_effect_new(bbxs_ctx, EFFECTS[i]);
        if (!effect_modules[i]) return false;
    }

    init_params();

    return true;
}

instr_device::~instr_device() {
    // caller should have called this before, but do it again just in case.
    deactivate();

    bbxs_synth_destroy(synth);
    for (int i = 0; i < MODULE_EFFECT_COUNT; i++) {
        bbxs_effect_destroy(effect_modules[i]);
    }
}

device::type instr_device::get_type() const { return device::TYPE_SYNTH; }

void *instr_device::get_bbxs_object() const { return synth; }

bool instr_device::activate(double sample_rate, uint32_t max_frames_count) {
    assert(clap_host);
    assert(clap_host_params);

    // load new instrument type when requested
    if (new_type_index != type_index && synth) {
        bbxs_synth_type_e new_type = synth_type_values[new_type_index];
        assert(new_type != -1);
        if (new_type == -1) return false;

        type_index = new_type_index;
        type = new_type;

        bbxs_synth_s *new_synth = bbxs_synth_new(bbxs_ctx, new_type);
        if (!new_synth) return false;

        // if (!copy_synth_config(instr->synth, new_synth))
        //     return false;

        bbxs_synth_destroy(synth);
        synth = new_synth;

        need_param_rescan.store(true);

        if (clap_host) clap_host->request_callback(clap_host);

        frames_until_next_tick = 0;
    }

    this->sample_rate = sample_rate;
    bbxs_synth_set_sample_rate(synth, sample_rate);

    for (int i = 0; i < MODULE_EFFECT_COUNT; ++i) {
        if (effect_modules[i])
            bbxs_effect_set_sample_rate(effect_modules[i], sample_rate);
    }

    // allocate process blocks
    delete synth_mono_buffer;
    synth_mono_buffer = new (std::nothrow) float[max_frames_count];
    if (!synth_mono_buffer) return false;

    delete process_block_alloc;
    process_block_alloc = new (std::nothrow) float[max_frames_count * 2];
    if (!process_block_alloc) return false;

    for (int i = 0; i < 2; ++i) {
        process_block[i] = process_block_alloc + max_frames_count * i;
    }

    return true;
}

void instr_device::deactivate() {
    delete process_block_alloc;
    process_block_alloc = nullptr;
    process_block[0] = nullptr;
    process_block[1] = nullptr;

    delete synth_mono_buffer;
    synth_mono_buffer = nullptr;
}

uint32_t instr_device::param_count() { return (uint32_t)param_id_list.size(); }

param_id instr_device::get_param_id(uint32_t index) const {
    if (index >= param_id_list.size()) return INVALID_PARAMETER_ID;
    return param_id_list[index];
}

param_info instr_device::get_param_info(param_id id) const {
    const param_data *data;
    {
        const auto &it = param_database.find(id);
        assert(it != param_database.end());
        if (it == param_database.end()) return {};
        data = &it->second;
    }

    param_info ret;
    ret.active = !data->inactive;
    ret.cookie = (void *)&data->cookie;
    ret.info = data->info;
    return ret;
}

bool instr_device::set_param(param_id id, double value, void *p_cookie) {
    const param_cookie *cookie = (const param_cookie *)p_cookie;
    if (!cookie) {
        const auto &it = param_database.find(id);
        if (it == param_database.end()) return false;
        if (it->second.inactive) return false;
        cookie = &it->second.cookie;
    }

    return param_cookie_set(cookie, id, value);
}

bool instr_device::get_param(param_id id, double *value,
                             const void *p_cookie) const {
    const param_cookie *cookie = (const param_cookie *)p_cookie;
    if (!cookie) {
        const auto &it = param_database.find(id);
        if (it == param_database.end()) return false;
        if (it->second.inactive) return false;
        cookie = &it->second.cookie;
    }

    return param_cookie_get(cookie, id, value);
}

void instr_device::on_main_thread() {
    assert(clap_host);
    assert(clap_host_params);

    if (need_param_rescan.exchange(false) && clap_host_params) {
        clap_host_params->rescan(clap_host, CLAP_PARAM_RESCAN_ALL);
    }
}

void instr_device::set_effect_active(bbxs_effect_type_e effect, bool value) {}

//------------------------------------------------------------------------------
// parameter setup
//------------------------------------------------------------------------------

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#ifndef BBXS_FM6_PARAM_COUNT
#    define BBXS_FM6_PARAM_COUNT 0 // in case I forget to add this type later on
#endif

static constexpr int MAX_SYNTH_PARAM_COUNT =
    BBXS_BASE_PARAM_COUNT +
    MAX(BBXS_CHIP_PARAM_COUNT,
        MAX(BBXS_FM_PARAM_COUNT,
            MAX(BBXS_NOISE_PARAM_COUNT,
                MAX(BBXS_PULSE_WIDTH_PARAM_COUNT,
                    MAX(BBXS_CUSTOM_CHIP_PARAM_COUNT,
                        MAX(BBXS_HARMONICS_PARAM_COUNT,
                            MAX(BBXS_SPECTRUM_PARAM_COUNT,
                                MAX(BBXS_PICKED_STRING_PARAM_COUNT,
                                    MAX(BBXS_FM6_PARAM_COUNT,
                                        BBXS_SUPERSAW_PARAM_COUNT)))))))));

void instr_device::init_params() {
    param_id_list.clear();
    param_database.clear();

#define PARAM(ID, INFO, ...)                                                   \
    do {                                                                       \
        param_database[ID] = {false, INFO, __VA_ARGS__};                       \
        param_id_list.push_back(ID);                                           \
    } while (false);
#define CTL(...) BBXPLUG_CTLPARAM(__VA_ARGS__)
#define SYN(...) BBXPLUG_SYNPARAM(__VA_ARGS__)
#define CTL_INFO(MODULE, MODULE_LOWER, NAME)                                   \
    &MODULE_LOWER##_control_param_info[MODULE##_CONTROL_PARAM_##NAME]
#define CTL_PARAM(MODULE, MODULE_LOWER, NAME, ...)                             \
    PARAM(CTL(MODULE, NAME), CTL_INFO(MODULE, MODULE_LOWER, NAME),             \
          ##__VA_ARGS__)

    // clang-format off
    // control params
    CTL_PARAM(SYNTH, synth, TYPE, {
        param_cookie::TYPE_FUNC,
        this,

        // set
        [](param_id id, double value, void *ud) {
            instr_device *self = (instr_device *)ud;
            self->new_type_index = (int)value;

            if (self->new_type_index != self->type_index) {
                // -1 means that instrument type is not implemented yet
                if (synth_type_values[self->new_type_index] == -1) {
                    self->new_type_index = self->type_index;
                } else if (self->clap_host->request_restart) {
                    self->clap_host->request_restart(self->clap_host);
                }
            }

            return true;
        },

        // get
        [](param_id id, double *value, void *ud) {
            instr_device *self = (instr_device *)ud;
            *value = (double)self->type_index;
            return true;
        }
    });
    CTL_PARAM(GENERAL, general, GAIN, {
        param_cookie::TYPE_FLOAT64,
        &gain,
    });
    CTL_PARAM(TEMPO, tempo, MULTIPLIER, {
        param_cookie::TYPE_FLOAT64,
        &tempo_multiplier,
    });
    CTL_PARAM(TEMPO, tempo, USE_OVERRIDE, {
        param_cookie::TYPE_BOOL,
        &tempo_use_override,
    });
    CTL_PARAM(TEMPO, tempo, OVERRIDE, {
        param_cookie::TYPE_FLOAT64,
        &tempo_override,
    });
    // clang-format on

    // synth parameters
    int synth_param_count = (int)bbxs_synth_param_count(type);
    auto register_synth_param = [&](int i) {
        const bbxs_param_info_s *info = bbxs_synth_param_info(type, i);
        assert(info);
        if (!info) return;

        switch (info->type) {
        case BBXS_PARAM_DOUBLE:
            PARAM(param_global_id(MODULE_SYNTH, i), info,
                  {param_cookie::TYPE_BBXS_SYNTH_PARAM_DOUBLE, synth});
            break;

        case BBXS_PARAM_INT:
        case BBXS_PARAM_UINT8:
            PARAM(param_global_id(MODULE_SYNTH, i), info,
                  {param_cookie::TYPE_BBXS_SYNTH_PARAM_INT, synth});
            break;
        }
    };

    auto register_effect_param = [&](param_module module, int index) -> bool {
        int efi = module - MODULE_FIRST_EFFECT;

        bbxs_effect_s *effect = effect_modules[efi];
        assert(effect);
        if (!effect) return false;

        bbxs_effect_type_e effect_type = bbxs_effect_type(effect);
        const bbxs_param_info_s *info =
            bbxs_effect_param_info(effect_type, index);
        assert(info);
        if (!info) return false;

        switch (info->type) {
        case BBXS_PARAM_DOUBLE:
            PARAM(param_global_id(module, index), info,
                  {param_cookie::TYPE_BBXS_EFFECT_PARAM_DOUBLE, effect});
            break;

        case BBXS_PARAM_INT:
        case BBXS_PARAM_UINT8:
            PARAM(param_global_id(module, index), info,
                  {param_cookie::TYPE_BBXS_EFFECT_PARAM_INT, effect});
            break;
        }

        return true;
    };

    // everything from the "General" category, neatly ordered
    register_effect_param(MODULE_VOLUME, BBXS_VOLUME_PARAM_VOLUME);
    register_synth_param(BBXS_PARAM_FADE_IN);
    register_synth_param(BBXS_PARAM_FADE_OUT);
    register_effect_param(MODULE_PANNING, BBXS_PANNING_PARAM_PAN);
    register_effect_param(MODULE_PANNING, BBXS_PANNING_PARAM_PAN_DELAY);

    // modulation targets
    register_synth_param(BBXS_PARAM_MOD_X);
    register_synth_param(BBXS_PARAM_MOD_Y);

    // synth type-specific parameters
    for (int i = BBXS_BASE_PARAM_COUNT; i < synth_param_count; i++)
        register_synth_param(i);

    // filler
    for (int i = synth_param_count; i < MAX_SYNTH_PARAM_COUNT; i++) {
        param_id id = param_global_id(MODULE_SYNTH, i);
        param_database[id] = {true, &unused_param_info};
        param_id_list.push_back(id);
    }

    // note effects
    for (int i = 0; i < synth_param_count; i++) {
        const bbxs_param_info_s *info = bbxs_synth_param_info(type, i);
        assert(info);
        if (!info || !info->group) return;

        const char *group = info->group;
        size_t slen = strlen(group);
        if (slen == 7 && !strcmp(group, "Effects") ||
            slen > 7 && !memcmp(group, "Effects/", 8)) {
            register_synth_param(i);
        }
    }

    // all the audio effects
    for (int efi = 0; efi < MODULE_EFFECT_COUNT; efi++) {
        bbxs_effect_s *effect = effect_modules[efi];
        assert(effect);
        if (!effect) continue;

        param_module module_index = (param_module)(MODULE_FIRST_EFFECT + efi);

        // these modules were already accounted for
        if (module_index == MODULE_PANNING || module_index == MODULE_VOLUME)
            continue;

            // add associated effect enable control param if it exists
#define HANDLE_CONTROL_PARAM(NAME, IDX)                                        \
    case MODULE_##NAME:                                                        \
        PARAM(CTL(EFFECT, ENABLE_##NAME), &effect_control_param_info[IDX],     \
              {param_cookie::TYPE_BOOL, &enabled_effects[module_index]});      \
        break

        switch (module_index) {
            HANDLE_CONTROL_PARAM(DISTORTION, 0);
            HANDLE_CONTROL_PARAM(BITCRUSHER, 1);
            HANDLE_CONTROL_PARAM(CHORUS, 2);
            HANDLE_CONTROL_PARAM(ECHO, 3);
            HANDLE_CONTROL_PARAM(REVERB, 4);
        default:
            break;
        }
#undef HANDLE_CONTROL_PARAM

        // add all parameters of the effect
        bbxs_effect_type_e effect_type = bbxs_effect_type(effect);
        int effect_param_count = (int)bbxs_effect_param_count(effect_type);
        for (int i = 0; i < effect_param_count; i++) {
            register_effect_param(module_index, i);
        }
    }

#undef PARAM
#undef CTL
#undef SYN
#undef CTL_PARAM
}

bool instr_device::param_cookie_set(const param_cookie *cookie, param_id id,
                                    double value) {
    param_id local_index;

    switch (cookie->type) {
    case param_cookie::TYPE_FLOAT64:
        *((double *)cookie->userdata) = value;
        return true;

    case param_cookie::TYPE_BOOL:
        *((bool *)cookie->userdata) = value < 0.5;
        return true;

    case param_cookie::TYPE_FUNC:
        return cookie->set(id, value, cookie->userdata);

    case param_cookie::TYPE_BBXS_SYNTH_PARAM_DOUBLE:
        local_index = id & PARAM_LOCAL_INDEX_MASK;
        return bbxs_synth_set_param_double((bbxs_synth_s *)cookie->userdata,
                                           (uint32_t)local_index, value);

    case param_cookie::TYPE_BBXS_SYNTH_PARAM_INT:
        local_index = id & PARAM_LOCAL_INDEX_MASK;
        return bbxs_synth_set_param_int((bbxs_synth_s *)cookie->userdata,
                                        (uint32_t)local_index, (int)value);

    case param_cookie::TYPE_BBXS_EFFECT_PARAM_DOUBLE:
        local_index = id & PARAM_LOCAL_INDEX_MASK;
        return bbxs_effect_set_param_double((bbxs_effect_s *)cookie->userdata,
                                            (uint32_t)local_index, value);

    case param_cookie::TYPE_BBXS_EFFECT_PARAM_INT:
        local_index = id & PARAM_LOCAL_INDEX_MASK;
        return bbxs_effect_set_param_int((bbxs_effect_s *)cookie->userdata,
                                         (uint32_t)local_index, (int)value);
    }

    assert(0);
    return false;
}

bool instr_device::param_cookie_get(const param_cookie *cookie, param_id id,
                                    double *value) {
    param_id local_index;
    bool s;
    int ivalue;

    switch (cookie->type) {
    case param_cookie::TYPE_FLOAT64:
        *value = *((double *)cookie->userdata);
        return true;

    case param_cookie::TYPE_BOOL:
        *value = *((bool *)cookie->userdata) ? 1.0 : 0.0;
        return true;

    case param_cookie::TYPE_FUNC:
        return cookie->get(id, value, cookie->userdata);

    case param_cookie::TYPE_BBXS_SYNTH_PARAM_DOUBLE:
        local_index = id & PARAM_LOCAL_INDEX_MASK;
        return bbxs_synth_get_param_double((bbxs_synth_s *)cookie->userdata,
                                           (uint32_t)local_index, value);

    case param_cookie::TYPE_BBXS_SYNTH_PARAM_INT:
        local_index = id & PARAM_LOCAL_INDEX_MASK;
        s = bbxs_synth_get_param_int((bbxs_synth_s *)cookie->userdata,
                                     (uint32_t)local_index, &ivalue);
        if (!s) return false;
        *value = (double)ivalue;
        return true;

    case param_cookie::TYPE_BBXS_EFFECT_PARAM_DOUBLE:
        local_index = id & PARAM_LOCAL_INDEX_MASK;
        return bbxs_effect_get_param_double((bbxs_effect_s *)cookie->userdata,
                                            (uint32_t)local_index, value);

    case param_cookie::TYPE_BBXS_EFFECT_PARAM_INT:
        local_index = id & PARAM_LOCAL_INDEX_MASK;
        s = bbxs_effect_get_param_int((bbxs_effect_s *)cookie->userdata,
                                      (uint32_t)local_index, &ivalue);
        if (!s) return false;
        *value = (double)ivalue;
        return true;
    }

    assert(0);
    return false;
}

//------------------------------------------------------------------------------
// processing
//------------------------------------------------------------------------------

clap_process_status instr_device::run(float **output, uint32_t frame_count,
                                      uint32_t start_frame,
                                      const clap_output_events_t *out_events) {
    process_userdata inst_proc = {
        .self = this,
        .cur_sample = start_frame,
        .out_events = out_events,
    };

    bbxs_synth_set_userdata(synth, &inst_proc);
    bbxs_synth_callbacks_s *bbxs_cb = bbxs_synth_get_callback_table(synth);
    bbxs_cb->voice_end = bbxs_voice_end_cb;

    linear_gain = pow(10.0, gain / 10.0);
    double active_bpm;
    if (tempo_use_override) {
        active_bpm = tempo_override;
    } else {
        active_bpm = bpm;
    }

    active_bpm *= tempo_multiplier;

    // clamp active_bpm because if it is zero then the plugin will
    // take a (theoretically) infinite amount of time to process a tick.
    // then this means any subsequent ticks will not be processed.
    // also completely freezes processing somehow
    if (active_bpm < 1.0) {
        active_bpm = 1.0;
    }

    const double beats_per_sec = active_bpm / 60.0;
    const double sample_len = 1.0 / sample_rate;

    float *out_l = output[0];
    float *out_r = output[1];
    for (uint32_t i = 0; i < frame_count;) {
        // instrument needs a tick
        if (frames_until_next_tick == 0) {
            bbxs_tick_ctx_s tick_ctx = {
                .bpm = active_bpm,
                .beat = cur_beat,
            };

            bbxs_synth_tick(synth, &tick_ctx);

            // tick panning, eq, and fader
            bbxs_effect_tick(get_effect_module(MODULE_PANNING), &tick_ctx);
            bbxs_effect_tick(get_effect_module(MODULE_EQ), &tick_ctx);
            bbxs_effect_tick(get_effect_module(MODULE_VOLUME), &tick_ctx);

#define TICK_EFFECT(NAME)                                                      \
    if ((running_effects[EFFECT_MODULE(NAME)] =                                \
             enabled_effects[EFFECT_MODULE(NAME)]))                            \
    bbxs_effect_tick(get_effect_module(MODULE_##NAME), &tick_ctx)

            TICK_EFFECT(DISTORTION);
            TICK_EFFECT(BITCRUSHER);
            TICK_EFFECT(CHORUS);
            TICK_EFFECT(ECHO);
            TICK_EFFECT(REVERB);

#undef TICK_EFFECT

            frames_until_next_tick = (uint32_t)ceil(
                bbxs_calc_samples_per_tick(active_bpm, sample_rate));

            cur_beat += beats_per_sec * sample_len * frames_until_next_tick;
        }

        // render the mono audio into process_block[1]
        uint32_t frames_to_process = frame_count - i;
        if (frames_until_next_tick < frames_to_process)
            frames_to_process = frames_until_next_tick;

        bbxs_synth_run(synth, synth_mono_buffer + i, frames_to_process);

        // convert mono audio from process_block[1] into process_block[0]
        float *process_block[2];
        process_block[0] = process_block[0] + i;
        process_block[1] = process_block[1] + i;

        for (uint32_t j = 0; j < frames_to_process; ++j) {
            const float v = synth_mono_buffer[i + j];
            process_block[0][j] = v;
            process_block[1][j] = 0.0;
        }

        // perform effect processing

#define RUN_EFFECT(NAME)                                                       \
    bbxs_effect_run(get_effect_module(MODULE_##NAME), process_block,           \
                    frames_to_process)
#define RUN_OPT_EFFECT(NAME)                                                   \
    if (running_effects[EFFECT_MODULE(NAME)]) RUN_EFFECT(NAME)

        // mono effects first
        // distortion, bitcrusher
        RUN_OPT_EFFECT(DISTORTION);
        RUN_OPT_EFFECT(BITCRUSHER);

        // eq
        RUN_EFFECT(EQ);

        // then, run panning, which converts to stereo
        RUN_EFFECT(PANNING);

        // then, stereo effects
        // chorus, echo and reverb
        RUN_OPT_EFFECT(CHORUS);
        RUN_OPT_EFFECT(ECHO);
        RUN_OPT_EFFECT(REVERB);

        // and last, volume control
        RUN_EFFECT(VOLUME);

#undef RUN_EFFECT
#undef RUN_OPT_EFFECT

        i += frames_to_process;
        inst_proc.cur_sample += frames_to_process;
        frames_until_next_tick -= frames_to_process;
    }

    // write output
    float control_gain = (float)linear_gain;
    for (uint32_t i = 0; i < frame_count; ++i) {
        output[0][i] = process_block[0][i] * control_gain;
        output[1][i] = process_block[1][i] * control_gain;
    }

    bbxs_synth_set_userdata(synth, nullptr);
    bbxs_cb->voice_end = nullptr;

    return CLAP_PROCESS_CONTINUE;
}

void instr_device::process_event(const clap_event_header_t *hdr,
                                 const clap_output_events_t *out_events) {
    if (hdr->space_id == CLAP_CORE_EVENT_SPACE_ID) {
        switch (hdr->type) {
        case CLAP_EVENT_NOTE_ON: {
            const clap_event_note_t *ev = (const clap_event_note_t *)hdr;

            begin_note(ev->key, ev->velocity, ev->note_id, ev->port_index,
                       ev->channel);
            break;
        }

        case CLAP_EVENT_NOTE_OFF: {
            const clap_event_note_t *ev = (const clap_event_note_t *)hdr;

            end_notes(ev->key, ev->note_id, ev->port_index, ev->channel);
            break;
        }

        case CLAP_EVENT_NOTE_CHOKE: {
            const clap_event_note_t *ev = (const clap_event_note_t *)hdr;
            // TODO: handle note choke
            break;
        }

        case CLAP_EVENT_NOTE_EXPRESSION: {
            const clap_event_note_expression_t *ev =
                (const clap_event_note_expression_t *)hdr;
            // TODO: handle note expression
            break;
        }

        case CLAP_EVENT_PARAM_VALUE: {
            const clap_event_param_value_t *ev =
                (const clap_event_param_value_t *)hdr;

            set_param(ev->param_id, ev->value, ev->cookie);
            // plugin_params_set_value(plug, ev->param_id, ev->value,
            //                         SEND_TO_GUI, out_events);

            break;
        }

        case CLAP_EVENT_PARAM_MOD: {
            const clap_event_param_mod_t *ev =
                (const clap_event_param_mod_t *)hdr;
            // TODO: handle parameter modulation
            break;
        }

        case CLAP_EVENT_TRANSPORT: {
            const clap_event_transport_t *ev =
                (const clap_event_transport_t *)hdr;
            process_transport(ev);
        }

        case CLAP_EVENT_MIDI: {
            const clap_event_midi_t *ev = (const clap_event_midi_t *)hdr;

            uint8_t status = ev->data[0] & 0xF0;
            uint8_t channel = ev->data[0] & 0x0F;

            // off
            if ((status == 0x80) || ((status == 0x90) && ev->data[2] == 0)) {
                end_notes(ev->data[1], -1, ev->port_index, channel);
            }

            // on
            else if (status == 0x90) {
                begin_note(ev->data[1], ev->data[2] / 127.0, -1, ev->port_index,
                           channel);
            }

            // channel mode messages
            else if (status == 0xB0) {
                // all notes off
                if (ev->data[1] == 123 && ev->data[2] == 0) {
                    end_notes(-1, -1, ev->port_index, channel);
                }
            }

            break;
        }

        case CLAP_EVENT_MIDI_SYSEX: {
            const clap_event_midi_sysex_t *ev =
                (const clap_event_midi_sysex_t *)hdr;
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

void instr_device::process_transport(const clap_event_transport_t *ev) {
    if (ev->flags & CLAP_TRANSPORT_HAS_TEMPO) {
        bpm = ev->tempo;
    } else {
        bpm = 150.0;
    }

    bool is_playing = (ev->flags & CLAP_TRANSPORT_IS_PLAYING) != 0;

    // if playing state changed, update current beat from transport event.
    // otherwise plugin should increment cur beat on its own using bpm info.
    if (is_playing != this->is_playing) {
        if (ev->flags & CLAP_TRANSPORT_HAS_BEATS_TIMELINE) {
            int64_t beats_int = ev->song_pos_beats / CLAP_BEATTIME_FACTOR;
            int64_t beats_frac = ev->song_pos_beats % CLAP_BEATTIME_FACTOR;
            cur_beat =
                (double)beats_int + (double)beats_frac / CLAP_BEATTIME_FACTOR;
        }

        if (is_playing) bbxs_synth_begin_transport(synth, cur_beat, bpm);
    }

    this->is_playing = is_playing;
}

//------------------------------------------------------------------------------
// voice handling
//------------------------------------------------------------------------------

inline bool instr_device::voice_match(const voice_data *voice, int32_t note_id,
                                      int16_t channel, int16_t port_index,
                                      int16_t key) {
    return (note_id == -1 || note_id == voice->note_id) &&
           (channel == -1 || channel == voice->channel) &&
           (port_index == -1 || port_index == voice->port_index) &&
           (key == -1 || key == voice->key);
}

void instr_device::begin_note(int16_t key, double velocity, int32_t note_id,
                              int16_t port_index, int16_t channel) {
    bbxs_voice_id bbxs_id =
        bbxs_synth_begin_note(synth, key, velocity, BBXS_NOTE_LENGTH_UNKNOWN);

    voices[bbxs_id] = {.active = true,
                       .note_id = note_id,
                       .port_index = port_index,
                       .channel = channel,
                       .key = key};

    ++active_voice_count;
}

void instr_device::end_notes(int16_t key, int32_t note_id, int16_t port_index,
                             int16_t channel) {
    for (int i = 0; i < BBXS_SYNTH_MAX_VOICES; i++) {
        voice_data *voice = &voices[i];
        if (voice->active &&
            voice_match(voice, note_id, channel, port_index, key)) {
            bbxs_synth_end_note(synth, i);
        }
    }
}

void instr_device::bbxs_voice_end_cb(bbxs_synth_s *inst, bbxs_voice_id id) {
    process_userdata *ud = (process_userdata *)bbxs_synth_get_userdata(inst);
    assert(ud);

    instr_device *self = ud->self;

    voice_data *voice = &self->voices[id];
    voice->active = false;
    self->active_voice_count--;

    clap_event_note_t ev = {
        .header =
            {
                .size = sizeof(ev),
                .time = ud->cur_sample,
                .type = CLAP_EVENT_NOTE_END,
            },
        .note_id = voice->note_id,
        .port_index = voice->port_index,
        .channel = voice->channel,
        .key = voice->key,
    };

    ud->out_events->try_push(ud->out_events, (clap_event_header_t *)&ev);

    // char str[128];
    // snprintf(str, 128, "sample: %i", ud->cur_sample);
    // ud->plugin->host_log->log(ud->plugin->host, CLAP_LOG_DEBUG, str);
}

//------------------------------------------------------------------------------

static int get_synth_type_index(bbxs_synth_type_e type) {
    int type_idx = -1;
    for (int i = 0; i < BBXS_SYNTH_COUNT; ++i) {
        if (synth_type_values[i] == type) type_idx = i;
    }

    return type_idx;
}