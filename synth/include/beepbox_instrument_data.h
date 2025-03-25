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

#define FM_PARAM_COUNT 11
#define FM_FREQ_COUNT 35
#define FM_ALGORITHM_COUNT 13
#define FM_FEEDBACK_TYPE_COUNT 18

typedef enum {
    PARAM_FADE_IN,
    PARAM_FADE_OUT
} inst_param_e;

typedef enum {
    FM_PARAM_ALGORITHM,          // 0
    FM_PARAM_FREQ1,              // 1
    FM_PARAM_VOLUME1,            // 2
    FM_PARAM_FREQ2,              // 3
    FM_PARAM_VOLUME2,            // 4
    FM_PARAM_FREQ3,              // 5
    FM_PARAM_VOLUME3,            // 6
    FM_PARAM_FREQ4,              // 7
    FM_PARAM_VOLUME4,            // 8
    FM_PARAM_FEEDBACK_TYPE,      // 9
    FM_PARAM_FEEDBACK_VOLUME,    // 10
} fm_param_e;

#endif