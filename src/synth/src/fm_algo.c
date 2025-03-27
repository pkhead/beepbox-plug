#include "fm_algo.h"
double fm_algo0000(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0001(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0002(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0003(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0004(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0005(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0006(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0007(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0008(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0009(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0010(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0011(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0012(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0013(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0014(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0015(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0016(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0017(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + op3_scaled
    ));
    return op0_scaled;
}
double fm_algo0100(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0101(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0102(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0103(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0104(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0105(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0106(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0107(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0108(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0109(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0110(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0111(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0112(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0113(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0114(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0115(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0116(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0117(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0200(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0201(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0202(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0203(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0204(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0205(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0206(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0207(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0208(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0209(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0210(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0211(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0212(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0213(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0214(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0215(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0216(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0217(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0300(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0301(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0302(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0303(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0304(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0305(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0306(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0307(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0308(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0309(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0310(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0311(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0312(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0313(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0314(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0315(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0316(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0317(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + op2_scaled
    ));
    return op0_scaled;
}
double fm_algo0400(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0401(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0402(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0403(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0404(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0405(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0406(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0407(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0408(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled;
}
double fm_algo0409(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0410(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0411(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0412(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0413(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0414(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0415(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0416(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0417(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op1_scaled
    ));
    return op0_scaled;
}
double fm_algo0500(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0501(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0502(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0503(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0504(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0505(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0506(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0507(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0508(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0509(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0510(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0511(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0512(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0513(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0514(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0515(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0516(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0517(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0600(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0601(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0602(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0603(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0604(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0605(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0606(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0607(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0608(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0609(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0610(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0611(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0612(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0613(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0614(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0615(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0616(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0617(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0700(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0701(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0702(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0703(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0704(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0705(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0706(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0707(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0708(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0709(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0710(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0711(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0712(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0713(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0714(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0715(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0716(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0717(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0800(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0801(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0802(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0803(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0804(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0805(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0806(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0807(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0808(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0809(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0810(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0811(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0812(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0813(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0814(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0815(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0816(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0817(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0900(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0901(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0902(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0903(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0904(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0905(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0906(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0907(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0908(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0909(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0910(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0911(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0912(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0913(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0914(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0915(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0916(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo0917(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op2_scaled + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op2_scaled + op3_scaled
    ));
    return op0_scaled + op1_scaled;
}
double fm_algo1000(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1001(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1002(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1003(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1004(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1005(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1006(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1007(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1008(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1009(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1010(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1011(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1012(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1013(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1014(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1015(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1016(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1017(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1100(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1101(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1102(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1103(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1104(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1105(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1106(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1107(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1108(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1109(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1110(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1111(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1112(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1113(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1114(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1115(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1116(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1117(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + op3_scaled + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + op3_scaled + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + op3_scaled
    ));
    return op0_scaled + op1_scaled + op2_scaled;
}
double fm_algo1200(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1201(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1202(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1203(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1204(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1205(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1206(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1207(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1208(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[3].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[2].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[1].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase + feedback_amp * voice->op_states[0].output
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1209(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1210(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1211(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1212(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1213(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1214(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1215(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[1].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[0].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1216(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[0].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
double fm_algo1217(fm_voice_s *voice, const double feedback_amp) {
    double op3_scaled = voice->op_states[3].expression * (voice->op_states[3].output = fm_calc_op(
        voice->op_states[3].phase + feedback_amp * voice->op_states[2].output
    ));
    double op2_scaled = voice->op_states[2].expression * (voice->op_states[2].output = fm_calc_op(
        voice->op_states[2].phase + feedback_amp * voice->op_states[1].output
    ));
    double op1_scaled = voice->op_states[1].expression * (voice->op_states[1].output = fm_calc_op(
        voice->op_states[1].phase + feedback_amp * voice->op_states[0].output
    ));
    double op0_scaled = voice->op_states[0].expression * (voice->op_states[0].output = fm_calc_op(
        voice->op_states[0].phase
    ));
    return op0_scaled + op1_scaled + op2_scaled + op3_scaled;
}
fm_algo_f fm_algorithm_table[234] = {
    fm_algo0000,
    fm_algo0001,
    fm_algo0002,
    fm_algo0003,
    fm_algo0004,
    fm_algo0005,
    fm_algo0006,
    fm_algo0007,
    fm_algo0008,
    fm_algo0009,
    fm_algo0010,
    fm_algo0011,
    fm_algo0012,
    fm_algo0013,
    fm_algo0014,
    fm_algo0015,
    fm_algo0016,
    fm_algo0017,
    fm_algo0100,
    fm_algo0101,
    fm_algo0102,
    fm_algo0103,
    fm_algo0104,
    fm_algo0105,
    fm_algo0106,
    fm_algo0107,
    fm_algo0108,
    fm_algo0109,
    fm_algo0110,
    fm_algo0111,
    fm_algo0112,
    fm_algo0113,
    fm_algo0114,
    fm_algo0115,
    fm_algo0116,
    fm_algo0117,
    fm_algo0200,
    fm_algo0201,
    fm_algo0202,
    fm_algo0203,
    fm_algo0204,
    fm_algo0205,
    fm_algo0206,
    fm_algo0207,
    fm_algo0208,
    fm_algo0209,
    fm_algo0210,
    fm_algo0211,
    fm_algo0212,
    fm_algo0213,
    fm_algo0214,
    fm_algo0215,
    fm_algo0216,
    fm_algo0217,
    fm_algo0300,
    fm_algo0301,
    fm_algo0302,
    fm_algo0303,
    fm_algo0304,
    fm_algo0305,
    fm_algo0306,
    fm_algo0307,
    fm_algo0308,
    fm_algo0309,
    fm_algo0310,
    fm_algo0311,
    fm_algo0312,
    fm_algo0313,
    fm_algo0314,
    fm_algo0315,
    fm_algo0316,
    fm_algo0317,
    fm_algo0400,
    fm_algo0401,
    fm_algo0402,
    fm_algo0403,
    fm_algo0404,
    fm_algo0405,
    fm_algo0406,
    fm_algo0407,
    fm_algo0408,
    fm_algo0409,
    fm_algo0410,
    fm_algo0411,
    fm_algo0412,
    fm_algo0413,
    fm_algo0414,
    fm_algo0415,
    fm_algo0416,
    fm_algo0417,
    fm_algo0500,
    fm_algo0501,
    fm_algo0502,
    fm_algo0503,
    fm_algo0504,
    fm_algo0505,
    fm_algo0506,
    fm_algo0507,
    fm_algo0508,
    fm_algo0509,
    fm_algo0510,
    fm_algo0511,
    fm_algo0512,
    fm_algo0513,
    fm_algo0514,
    fm_algo0515,
    fm_algo0516,
    fm_algo0517,
    fm_algo0600,
    fm_algo0601,
    fm_algo0602,
    fm_algo0603,
    fm_algo0604,
    fm_algo0605,
    fm_algo0606,
    fm_algo0607,
    fm_algo0608,
    fm_algo0609,
    fm_algo0610,
    fm_algo0611,
    fm_algo0612,
    fm_algo0613,
    fm_algo0614,
    fm_algo0615,
    fm_algo0616,
    fm_algo0617,
    fm_algo0700,
    fm_algo0701,
    fm_algo0702,
    fm_algo0703,
    fm_algo0704,
    fm_algo0705,
    fm_algo0706,
    fm_algo0707,
    fm_algo0708,
    fm_algo0709,
    fm_algo0710,
    fm_algo0711,
    fm_algo0712,
    fm_algo0713,
    fm_algo0714,
    fm_algo0715,
    fm_algo0716,
    fm_algo0717,
    fm_algo0800,
    fm_algo0801,
    fm_algo0802,
    fm_algo0803,
    fm_algo0804,
    fm_algo0805,
    fm_algo0806,
    fm_algo0807,
    fm_algo0808,
    fm_algo0809,
    fm_algo0810,
    fm_algo0811,
    fm_algo0812,
    fm_algo0813,
    fm_algo0814,
    fm_algo0815,
    fm_algo0816,
    fm_algo0817,
    fm_algo0900,
    fm_algo0901,
    fm_algo0902,
    fm_algo0903,
    fm_algo0904,
    fm_algo0905,
    fm_algo0906,
    fm_algo0907,
    fm_algo0908,
    fm_algo0909,
    fm_algo0910,
    fm_algo0911,
    fm_algo0912,
    fm_algo0913,
    fm_algo0914,
    fm_algo0915,
    fm_algo0916,
    fm_algo0917,
    fm_algo1000,
    fm_algo1001,
    fm_algo1002,
    fm_algo1003,
    fm_algo1004,
    fm_algo1005,
    fm_algo1006,
    fm_algo1007,
    fm_algo1008,
    fm_algo1009,
    fm_algo1010,
    fm_algo1011,
    fm_algo1012,
    fm_algo1013,
    fm_algo1014,
    fm_algo1015,
    fm_algo1016,
    fm_algo1017,
    fm_algo1100,
    fm_algo1101,
    fm_algo1102,
    fm_algo1103,
    fm_algo1104,
    fm_algo1105,
    fm_algo1106,
    fm_algo1107,
    fm_algo1108,
    fm_algo1109,
    fm_algo1110,
    fm_algo1111,
    fm_algo1112,
    fm_algo1113,
    fm_algo1114,
    fm_algo1115,
    fm_algo1116,
    fm_algo1117,
    fm_algo1200,
    fm_algo1201,
    fm_algo1202,
    fm_algo1203,
    fm_algo1204,
    fm_algo1205,
    fm_algo1206,
    fm_algo1207,
    fm_algo1208,
    fm_algo1209,
    fm_algo1210,
    fm_algo1211,
    fm_algo1212,
    fm_algo1213,
    fm_algo1214,
    fm_algo1215,
    fm_algo1216,
    fm_algo1217,
};
