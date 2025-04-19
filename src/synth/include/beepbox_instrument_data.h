// Copyright 2025 pkhead
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
// is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
// IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef _beepbox_instrument_data_h_
#define _beepbox_instrument_data_h_

#define BPBX_INST_MAX_VOICES 16
#define BPBX_BASE_PARAM_COUNT 3

typedef enum {
    BPBX_PARAM_VOLUME,
    BPBX_PARAM_FADE_IN,
    BPBX_PARAM_FADE_OUT
} bpbx_inst_param_e;

#define BPBX_FM_PARAM_COUNT 11
#define BPBX_FM_FREQ_COUNT 35
#define BPBX_FM_ALGORITHM_COUNT 13
#define BPBX_FM_FEEDBACK_TYPE_COUNT 18

typedef enum {
    BPBX_FM_PARAM_ALGORITHM = BPBX_BASE_PARAM_COUNT,
    BPBX_FM_PARAM_FREQ1,
    BPBX_FM_PARAM_VOLUME1,
    BPBX_FM_PARAM_FREQ2,
    BPBX_FM_PARAM_VOLUME2,
    BPBX_FM_PARAM_FREQ3,
    BPBX_FM_PARAM_VOLUME3,
    BPBX_FM_PARAM_FREQ4,
    BPBX_FM_PARAM_VOLUME4,
    BPBX_FM_PARAM_FEEDBACK_TYPE,
    BPBX_FM_PARAM_FEEDBACK_VOLUME,
} bpbx_fm_param_e;

#endif