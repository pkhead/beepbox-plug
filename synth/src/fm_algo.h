#ifndef _fm_algo_h_
#define _fm_algo_h_
#include "fm.h"

typedef double (*fm_algo_f)(fm_voice_s *voice, const double feedback_amp);
extern fm_algo_f fm_algorithm_table[234];
#endif
