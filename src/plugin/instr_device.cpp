#include "instr_device.hpp"

#include <cassert>

using namespace bbxplug;

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

    bpm = 150.0;
    tempo_multiplier = 1.0;
    tempo_override = 150.0;
    tempo_use_override = false;

    synth = bbxs_synth_new(bbxs_ctx, type);
    if (!synth) return false;

    // must be in same order as the MODULE_* enums for effects
    static constexpr bbxs_effect_type_e EFFECTS[] = {
        BBXS_EFFECT_PANNING, BBXS_EFFECT_DISTORTION, BBXS_EFFECT_BITCRUSHER,
        BBXS_EFFECT_CHORUS,  BBXS_EFFECT_ECHO,       BBXS_EFFECT_REVERB,
        BBXS_EFFECT_EQ,      BBXS_EFFECT_VOLUME,
    };

    for (int i = 0; i < sizeof(EFFECTS) / sizeof(*EFFECTS); ++i) {
        effect_modules[i] = bbxs_effect_new(bbxs_ctx, EFFECTS[i]);
        if (!effect_modules[i]) return false;
    }

    init_params();

    return true;
}

instr_device::~instr_device() {
    deactivate();

    bbxs_synth_destroy(synth);
    for (int i = 0; i < MODULE_EFFECT_COUNT; i++) {
        bbxs_effect_destroy(effect_modules[i]);
    }

    bbxs_context_destroy(bbxs_ctx);
}

device::type instr_device::get_type() const { return device::TYPE_SYNTH; }

void instr_device::process(float **output, uint32_t frame_count,
                           uint32_t start_frame,
                           const clap_output_events_t *out_events) {}

void instr_device::on_main_thread() {}

void *instr_device::get_bbxs_object() const { return synth; }

bool instr_device::activate(double sample_rate, uint32_t max_frames_count) {
    return false;
}

void instr_device::deactivate() {
    delete process_block_alloc;
    process_block_alloc = nullptr;
    process_block[0] = nullptr;
    process_block[1] = nullptr;

    delete synth_mono_buffer;
    synth_mono_buffer = nullptr;
}

void instr_device::set_effect_active(bbxs_effect_type_e effect, bool value) {}

void instr_device::begin_note(int16_t key, double velocity, int32_t note_id,
                              int16_t port_index, int16_t channel) {}
void instr_device::end_notes(int16_t key, int32_t note_id, int16_t port_index,
                             int16_t channel) {}

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

////////////////////////////////////////////////////////////////////////////////

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#ifndef BBXS_FM6_PARAM_COUNT
#    define BBXS_FM6_PARAM_COUNT 0 // in case I forget to add this type later on
#endif

#define MAX_SYNTH_PARAM_COUNT                                                  \
    BBXS_BASE_PARAM_COUNT +                                                    \
        MAX(BBXS_CHIP_PARAM_COUNT,                                             \
            MAX(BBXS_FM_PARAM_COUNT,                                           \
                MAX(BBXS_NOISE_PARAM_COUNT,                                    \
                    MAX(BBXS_PULSE_WIDTH_PARAM_COUNT,                          \
                        MAX(BBXS_CUSTOM_CHIP_PARAM_COUNT,                      \
                            MAX(BBXS_HARMONICS_PARAM_COUNT,                    \
                                MAX(BBXS_SPECTRUM_PARAM_COUNT,                 \
                                    MAX(BBXS_PICKED_STRING_PARAM_COUNT,        \
                                        MAX(BBXS_FM6_PARAM_COUNT,              \
                                            BBXS_SUPERSAW_PARAM_COUNT)))))))))

void instr_device::init_params() {
    param_id_list.clear();
    param_database.clear();

#define PARAM(ID, INFO, ...)                                                   \
    param_database[ID] = {false, INFO, __VA_ARGS__};                           \
    param_id_list.push_back(ID);
#define CTL(...) BBXPLUG_CTLPARAM(__VA_ARGS__)
#define SYN(...) BBXPLUG_SYNPARAM(__VA_ARGS__)
#define CTL_INFO(MODULE, MODULE_LOWER, NAME)                                   \
    &MODULE_LOWER##_control_param_info[MODULE##_CONTROL_PARAM_##NAME]
#define CTL_PARAM(MODULE, MODULE_LOWER, NAME, ...)                             \
    PARAM(CTL(MODULE, NAME), CTL_INFO(MODULE, MODULE_LOWER, NAME),             \
          ##__VA_ARGS__)

    (void)"SwatTheNotabug";

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
    CTL_PARAM(TEMPO, tempo, USE_OVERRIDE, {
        param_cookie::TYPE_BOOL,
        &tempo_use_override,
    });
    CTL_PARAM(TEMPO, tempo, MULTIPLIER, {
        param_cookie::TYPE_FLOAT64,
        &tempo_multiplier,
    });
    CTL_PARAM(TEMPO, tempo, OVERRIDE, {
        param_cookie::TYPE_FLOAT64,
        &tempo_override,
    });
    // clang-format on

    // synth parameters
    int synth_param_count = (int)bbxs_synth_param_count(type);
    for (int i = 0; i < synth_param_count; i++) {
        const bbxs_param_info_s *info = bbxs_synth_param_info(type, i);
        assert(info);
        if (!info) continue;

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
    }
    for (int i = synth_param_count; i < MAX_SYNTH_PARAM_COUNT; i++) {
        param_id id = param_global_id(MODULE_SYNTH, i);

        param_database[id] = {true, &unused_param_info};
        param_id_list.push_back(id);
    }

    // effect parameters
    for (int efi = 0; efi < MODULE_EFFECT_COUNT; efi++) {
        bbxs_effect_s *effect = effect_modules[efi];
        assert(effect);
        if (!effect) continue;

        param_module module_index = (param_module)(MODULE_FIRST_EFFECT + efi);

        // effect controls
        param_id control_idx;

#define CHECK_MODULE_IDX(NAME)                                                 \
    case MODULE_##NAME:                                                        \
        control_idx = CTL(EFFECT, ENABLE_##NAME);                              \
        break

        switch (module_index) {
            CHECK_MODULE_IDX(DISTORTION);
            CHECK_MODULE_IDX(BITCRUSHER);
            CHECK_MODULE_IDX(CHORUS);
            CHECK_MODULE_IDX(ECHO);
            CHECK_MODULE_IDX(REVERB);
        default:
            assert(false);
        }

#undef CHECK_MODULE_IDX

        PARAM(control_idx, &effect_control_param_info[efi],
              {param_cookie::TYPE_BOOL, &enabled_effects[module_index]});

        bbxs_effect_type_e effect_type = bbxs_effect_type(effect);
        int effect_param_count = (int)bbxs_effect_param_count(effect_type);
        for (int i = 0; i < effect_param_count; i++) {
            const bbxs_param_info_s *info =
                bbxs_effect_param_info(effect_type, i);
            assert(info);
            if (!info) continue;

            switch (info->type) {
            case BBXS_PARAM_DOUBLE:
                PARAM(param_global_id(module_index, i), info,
                      {param_cookie::TYPE_BBXS_EFFECT_PARAM_DOUBLE, effect});
                break;

            case BBXS_PARAM_INT:
            case BBXS_PARAM_UINT8:
                PARAM(param_global_id(module_index, i), info,
                      {param_cookie::TYPE_BBXS_EFFECT_PARAM_INT, effect});
                break;
            }
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

////////////////////////////////////////////////////////////////////////////////

static int get_synth_type_index(bbxs_synth_type_e type) {
    int type_idx = -1;
    for (int i = 0; i < BBXS_SYNTH_COUNT; ++i) {
        if (synth_type_values[i] == type) type_idx = i;
    }

    return type_idx;
}