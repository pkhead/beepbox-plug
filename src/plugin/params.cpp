#include "params.hpp"

using namespace bbxplug;

static const char *bool_enum_values[] = {"off", "on"};
static const char *synth_type_enum_values[BBXS_SYNTH_COUNT] = {
    "chip wave", "pulse width", "supersaw",    "harmonics", "picked string",
    "spectrum",  "FM",          "custom chip", "noise"};

const bbxs_param_info_s bbxplug::unused_param_info = {
    .type = BBXS_PARAM_UINT8,
    .flags = 0,
    .name = "Unused Parameter",
    .group = nullptr,

    .id = "unused\0",

    .min_value = 0.0,
    .max_value = 1.0,
    .default_value = 0.0,

    .enum_values = nullptr,
};

const bbxs_param_info_s
    bbxplug::synth_control_param_info[SYNTH_CONTROL_PARAM__COUNT] = {
        {
            .type = BBXS_PARAM_UINT8,
            .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,
            .name = "Synth Type",
            .group = "Control",
            .id = "ctType\0",

            .min_value = 0,
            .max_value = BBXS_SYNTH_COUNT - 1,
            .default_value = 0,

            .enum_values = synth_type_enum_values,
        },
};

const bbxs_param_info_s
    bbxplug::general_control_param_info[GENERAL_CONTROL_PARAM__COUNT] = {{
        .type = BBXS_PARAM_DOUBLE,
        .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,
        .name = "Gain",
        .group = "Control",
        .id = "ctGain\0",

        .min_value = -10.0,
        .max_value = 10.0,
        .default_value = 0.0,

    }};

const bbxs_param_info_s
    bbxplug::tempo_control_param_info[TEMPO_CONTROL_PARAM__COUNT] = {
        {
            .type = BBXS_PARAM_UINT8,
            .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,
            .name = "Force Tempo",
            .group = "Control",
            .id = {'c', 't', 'T', 'm', 'p', 'M', 'o', 'd'},
            // there. fucking c++ compiler. Happy now?

            .min_value = 0,
            .max_value = 1,
            .default_value = 0,

            .enum_values = bool_enum_values,
        },
        {
            .type = BBXS_PARAM_DOUBLE,
            .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,
            .name = "Tempo Multiplier",
            .group = "Control",
            .id = {'c', 't', 'T', 'm', 'p', 'M', 'u', 'l'},
            // i fucking hate c++

            .min_value = 0.0,
            .max_value = 10.0,
            .default_value = 1.0,

        },
        {
            .type = BBXS_PARAM_DOUBLE,
            .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,
            .name = "Tempo Force Value",
            .group = "Control",
            .id = {'c', 't', 'T', 'm', 'p', 'O', 'v', 'r'},
            // converting this to C++ was the worst idea ever

            .min_value = 1.0,
            .max_value = 500.0,
            .default_value = 150.0,

        },
};

const bbxs_param_info_s
    bbxplug::effect_control_param_info[EFFECT_CONTROL_PARAM__COUNT] = {
        {
            .type = BBXS_PARAM_UINT8,
            .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,
            .name = "Distortion Toggle",
            .group = "Effects/Distortion",
            .id = {'c', 't', 'D', 'i', 's', 't', 'o', 'r'},
            // all of this just for proper atomic bools using MSVC

            .min_value = 0,
            .max_value = 1,
            .default_value = 0,

            .enum_values = bool_enum_values,
        },
        {
            .type = BBXS_PARAM_UINT8,
            .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,
            .name = "Bitcrusher Toggle",
            .group = "Effects/Bitcrusher",
            .id = {'c', 't', 'B', 'i', 't', 'c', 'r', 'u'},
            // and for the plugin to use a single language instead of mixing
            // C/C++

            .min_value = 0,
            .max_value = 1,
            .default_value = 0,

            .enum_values = bool_enum_values,
        },
        {
            .type = BBXS_PARAM_UINT8,
            .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,
            .name = "Chorus Toggle",
            .group = "Effects/Chorus",
            .id = {'c', 't', 'C', 'h', 'o', 'r', 'u', 's'},
            // is this really worth the effort. it worked perfectly fine.

            .min_value = 0,
            .max_value = 1,
            .default_value = 0,

            .enum_values = bool_enum_values,
        },
        {
            .type = BBXS_PARAM_UINT8,
            .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,
            .name = "Echo Toggle",
            .group = "Effects/Echo",
            .id = "ctEcho\0",
            // ok this one is convenient

            .min_value = 0,
            .max_value = 1,
            .default_value = 0,

            .enum_values = bool_enum_values,
        },
        {
            .type = BBXS_PARAM_UINT8,
            .flags = BBXS_PARAM_FLAG_NO_AUTOMATION,
            .name = "Reverb Toggle",
            .group = "Effects/Reverb",
            .id = {'c', 't', 'R', 'e', 'v', 'e', 'r', 'b'},
            // fuck microsoft. fuck bjarne strousoup. whatever you spell his
            // name. horible fucking language, took them two decades to
            // implement initializer lists. language is literally the equivalent
            // of a fat old diseased rat. like. why can't i just Do This.  You
            // can do this in C.

            .min_value = 0,
            .max_value = 1,
            .default_value = 0,

            .enum_values = bool_enum_values,
        },
};