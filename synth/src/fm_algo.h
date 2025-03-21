#ifndef _fm_algo_h_
#define _fm_algo_h_
#include "fm.h"

typedef double (*fm_algo_func_t)(fm_voice_t *voice, const float feedback_amp);
extern fm_algo_func_t fm_algorithm_table[13];
#endif
