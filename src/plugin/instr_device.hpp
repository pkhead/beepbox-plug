#pragma once

#include <atomic>
#include <beepbox_synth.h>
#include <clap/clap.h>
#include <cstdint>
#include <unordered_map>

#include "device.hpp"

namespace bbxplug {

// synth + effect chain
class instr_device : public device {
  public:
    const clap_host_t *clap_host;
    const clap_host_params_t *clap_host_params;

    instr_device(bbxs_context_s *ctx);
    ~instr_device();

    device::type get_type() const override;

    bool init(bbxs_synth_type_e type, bool enable_switching);

    void process(float **output, uint32_t frame_count, uint32_t start_frame,
                 const clap_output_events_t *out_events) override;
    void on_main_thread() override;
    void *get_bbxs_object() const override;

    bool activate(double sample_rate, uint32_t max_frames_count) override;
    void deactivate() override;

    void set_effect_active(bbxs_effect_type_e effect, bool value);
    // bool instr_is_module_active(const instrument_s *instr, instr_module_e
    // module);

    void begin_note(int16_t key, double velocity, int32_t note_id,
                    int16_t port_index, int16_t channel) override;
    void end_notes(int16_t key, int32_t note_id, int16_t port_index,
                   int16_t channel) override;

    uint32_t param_count() override;
    param_id get_param_id(uint32_t index) const override;
    param_info get_param_info(param_id id) const override;
    bool set_param(param_id id, double value, void *cookie) override;
    bool get_param(param_id id, double *value,
                   const void *cookie) const override;

  protected:
    struct voice {
        bool active;

        int32_t note_id;
        int16_t port_index;
        int16_t channel;
        int16_t key;
    };

    bbxs_synth_type_e type;
    uint8_t type_index;
    uint8_t new_type_index;

    bbxs_context_s *bbxs_ctx;
    bbxs_synth_s *synth;

    bbxs_effect_s *effect_modules[MODULE_EFFECT_COUNT];
    bool enabled_effects[MODULE_EFFECT_COUNT];
    // when effects are added, they must wait until the next tick to begin
    // processing. these bools determine whether or not said effect is fully
    // active or not.
    bool running_effects[MODULE_EFFECT_COUNT];

    uint32_t frames_until_next_tick;

    // mono output buffer for the synth (is nullptr if the synth module is not
    // present)
    float *synth_mono_buffer;

    // stereo processing block (effect processing is done in-place)
    // process_block[0] is the left channel, and process_block[1] is the right.
    float *process_block[2];
    float *process_block_alloc;

    double sample_rate;
    double bpm;
    double cur_beat;
    bool is_playing;
    std::atomic_bool need_param_rescan;

    double gain;
    bool tempo_use_override;
    double tempo_multiplier;
    double tempo_override;

    // derived from gain property
    double linear_gain;

    // tracked voices
    voice voices[BBXS_SYNTH_MAX_VOICES];
    int8_t active_voice_count;

    struct param_cookie {
        enum param_type {
            TYPE_FLOAT64,
            TYPE_BOOL,
            TYPE_FUNC,
            TYPE_BBXS_SYNTH_PARAM_DOUBLE,
            TYPE_BBXS_SYNTH_PARAM_INT,
            TYPE_BBXS_EFFECT_PARAM_DOUBLE,
            TYPE_BBXS_EFFECT_PARAM_INT,
        } type;
        void *userdata;

        bool (*set)(param_id id, double value, void *ud);
        bool (*get)(param_id id, double *value, void *ud);
    };

    struct param_data {
        bool inactive;
        const bbxs_param_info_s *info;
        param_cookie cookie;
    };

    std::vector<param_id> param_id_list;
    // important: modifying this list or calling init_params will invalidate all
    // cookies, leaving them as dangling pointers.
    std::unordered_map<param_id, param_data> param_database;

    static bool param_cookie_set(const param_cookie *cookie, param_id id,
                                 double value);
    static bool param_cookie_get(const param_cookie *cookie, param_id id,
                                 double *value);
    void init_params();
};
} // namespace bbxplug