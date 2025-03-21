#include "fm_algo.h"
double fm_algo0000(fm_voice_t *voice, const float feedback_amp) {
    voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase,
        voice->op_states[3].expression
    );
    voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase,
        voice->op_states[2].expression
    );
    voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase,
        voice->op_states[1].expression
    );
    voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + voice->op_states[1].output + voice->op_states[2].output + voice->op_states[3].output,
        voice->op_states[0].expression
    );
    return voice->op_states[0].output;
}
double fm_algo0100(fm_voice_t *voice, const float feedback_amp) {
    voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase,
        voice->op_states[3].expression
    );
    voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + voice->op_states[3].output,
        voice->op_states[2].expression
    );
    voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase,
        voice->op_states[1].expression
    );
    voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + voice->op_states[1].output + voice->op_states[2].output,
        voice->op_states[0].expression
    );
    return voice->op_states[0].output;
}
double fm_algo0200(fm_voice_t *voice, const float feedback_amp) {
    voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase,
        voice->op_states[3].expression
    );
    voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase,
        voice->op_states[2].expression
    );
    voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + voice->op_states[2].output + voice->op_states[3].output,
        voice->op_states[1].expression
    );
    voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + voice->op_states[1].output,
        voice->op_states[0].expression
    );
    return voice->op_states[0].output;
}
double fm_algo0300(fm_voice_t *voice, const float feedback_amp) {
    voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase,
        voice->op_states[3].expression
    );
    voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + voice->op_states[3].output,
        voice->op_states[2].expression
    );
    voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + voice->op_states[3].output,
        voice->op_states[1].expression
    );
    voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + voice->op_states[1].output + voice->op_states[2].output,
        voice->op_states[0].expression
    );
    return voice->op_states[0].output;
}
double fm_algo0400(fm_voice_t *voice, const float feedback_amp) {
    voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase,
        voice->op_states[3].expression
    );
    voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + voice->op_states[3].output,
        voice->op_states[2].expression
    );
    voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + voice->op_states[2].output,
        voice->op_states[1].expression
    );
    voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + voice->op_states[1].output,
        voice->op_states[0].expression
    );
    return voice->op_states[0].output;
}
double fm_algo0500(fm_voice_t *voice, const float feedback_amp) {
    voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase,
        voice->op_states[3].expression
    );
    voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase,
        voice->op_states[2].expression
    );
    voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + voice->op_states[3].output,
        voice->op_states[1].expression
    );
    voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + voice->op_states[2].output,
        voice->op_states[0].expression
    );
    return voice->op_states[0].output + voice->op_states[1].output;
}
double fm_algo0600(fm_voice_t *voice, const float feedback_amp) {
    voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase,
        voice->op_states[3].expression
    );
    voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase,
        voice->op_states[2].expression
    );
    voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + voice->op_states[2].output + voice->op_states[3].output,
        voice->op_states[1].expression
    );
    voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase,
        voice->op_states[0].expression
    );
    return voice->op_states[0].output + voice->op_states[1].output;
}
double fm_algo0700(fm_voice_t *voice, const float feedback_amp) {
    voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase,
        voice->op_states[3].expression
    );
    voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + voice->op_states[3].output,
        voice->op_states[2].expression
    );
    voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + voice->op_states[2].output,
        voice->op_states[1].expression
    );
    voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase,
        voice->op_states[0].expression
    );
    return voice->op_states[0].output + voice->op_states[1].output;
}
double fm_algo0800(fm_voice_t *voice, const float feedback_amp) {
    voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase,
        voice->op_states[3].expression
    );
    voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + voice->op_states[3].output,
        voice->op_states[2].expression
    );
    voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + voice->op_states[2].output,
        voice->op_states[1].expression
    );
    voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + voice->op_states[2].output,
        voice->op_states[0].expression
    );
    return voice->op_states[0].output + voice->op_states[1].output;
}
double fm_algo0900(fm_voice_t *voice, const float feedback_amp) {
    voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase,
        voice->op_states[3].expression
    );
    voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase,
        voice->op_states[2].expression
    );
    voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + voice->op_states[2].output + voice->op_states[3].output,
        voice->op_states[1].expression
    );
    voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + voice->op_states[2].output + voice->op_states[3].output,
        voice->op_states[0].expression
    );
    return voice->op_states[0].output + voice->op_states[1].output;
}
double fm_algo1000(fm_voice_t *voice, const float feedback_amp) {
    voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase,
        voice->op_states[3].expression
    );
    voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + voice->op_states[3].output,
        voice->op_states[2].expression
    );
    voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase,
        voice->op_states[1].expression
    );
    voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase,
        voice->op_states[0].expression
    );
    return voice->op_states[0].output + voice->op_states[1].output + voice->op_states[2].output;
}
double fm_algo1100(fm_voice_t *voice, const float feedback_amp) {
    voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase,
        voice->op_states[3].expression
    );
    voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + voice->op_states[3].output,
        voice->op_states[2].expression
    );
    voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + voice->op_states[3].output,
        voice->op_states[1].expression
    );
    voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + voice->op_states[3].output,
        voice->op_states[0].expression
    );
    return voice->op_states[0].output + voice->op_states[1].output + voice->op_states[2].output;
}
double fm_algo1200(fm_voice_t *voice, const float feedback_amp) {
    voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase,
        voice->op_states[3].expression
    );
    voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase,
        voice->op_states[2].expression
    );
    voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase,
        voice->op_states[1].expression
    );
    voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase,
        voice->op_states[0].expression
    );
    return voice->op_states[0].output + voice->op_states[1].output + voice->op_states[2].output + voice->op_states[3].output;
}
fm_algo_func_t fm_algorithm_table[13] = {
    fm_algo0000,
    fm_algo0100,
    fm_algo0200,
    fm_algo0300,
    fm_algo0400,
    fm_algo0500,
    fm_algo0600,
    fm_algo0700,
    fm_algo0800,
    fm_algo0900,
    fm_algo1000,
    fm_algo1100,
    fm_algo1200,
};
