#pragma once

#include <beepbox_synth.h>
#include <clap/clap.h>
#include <cstdint>

#include "params.hpp"

namespace bbxplug {

// A synthesizer or effect device, complete with a parameter list.
class device {
  public:
    enum type { TYPE_SYNTH, TYPE_EFFECT };

    ~device(){};

    virtual type get_type() const = 0;
    // return the primary bbxs object. if get_type() returns SYNTH, it should be
    // a bbxs_synth_s. if get_type() returns EFFECT, it should be a
    // bbxs_effect_s.
    virtual void *get_bbxs_object() const = 0;

    virtual void process(float **output, uint32_t frame_count,
                         uint32_t start_frame,
                         const clap_output_events_t *out_events) = 0;
    virtual void on_main_thread() {};

    virtual bool activate(bbxs_context_s *ctx, double sample_rate,
                          uint32_t max_frames_count) = 0;
    virtual void deactivate() = 0;

    virtual void begin_note(int16_t key, double velocity, int32_t note_id,
                            int16_t port_index, int16_t channel) {};
    virtual void end_notes(int16_t key, int32_t note_id, int16_t port_index,
                           int16_t channel) {};

    virtual uint32_t param_count() = 0;
    virtual param_id get_param_id(uint32_t index) const = 0;
    virtual param_info get_param_info(param_id id) const = 0;
    virtual bool set_param(param_id id, double value, void *cookie) = 0;
    virtual bool get_param(param_id id, double *value,
                           const void *cookie) const = 0;
};

} // namespace bbxplug