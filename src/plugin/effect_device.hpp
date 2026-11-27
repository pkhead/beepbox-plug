#pragma once

#include "device.hpp"

namespace bbxplug {

class effect_device : public device {
  public:
    effect_device(bbxs_context_s *ctx, bbxs_effect_type_e type);
    ~effect_device();

    void process(float **output, uint32_t frame_count, uint32_t start_frame,
                 const clap_output_events_t *out_events) override;
    void *get_bbxs_object() const override;

    bool activate(bbxs_context_s *ctx, double sample_rate,
                  uint32_t max_frames_count) override;
    bool deactivate() override;

    uint32_t param_count() override;
    param_id get_param_id(uint32_t index) const override;
    param_info get_param_info(bbxs_param_info_s id) const;
    bool set_param(param_id id, double value, void *cookie) override;
    bool get_param(param_id id, double *value,
                   const void *cookie) const override;
};
} // namespace bbxplug