#ifndef PLUGIN_CONFIG_H
#define PLUGIN_CONFIG_H

#define CPLUG_COMPANY_NAME "pkhead"
#define CPLUG_COMPANY_EMAIL ""
#define CPLUG_PLUGIN_NAME "BeepBox"
#define CPLUG_PLUGIN_URI ""
#define CPLUG_PLUGIN_VERSION "0.0.1"

#define CPLUG_IS_INSTRUMENT 1

#define CPLUG_NUM_INPUT_BUSSES 0
#define CPLUG_NUM_OUTPUT_BUSSES 1
#define CPLUG_WANT_MIDI_INPUT 1
#define CPLUG_WANT_MIDI_OUTPUT 0

#define CPLUG_WANT_GUI 1
#define CPLUG_GUI_RESIZABLE 0

// See list of categories here: https://steinbergmedia.github.io/vst3_doc/vstinterfaces/namespaceSteinberg_1_1Vst_1_1PlugType.html
#define CPLUG_VST3_CATEGORIES "Instrument|Stereo"

#define CPLUG_VST3_TUID_COMPONENT 'cplg', 'comp', 'xmpl', 0
#define CPLUG_VST3_TUID_CONTROLLER 'cplg', 'edit', 'xmpl', 0

#define CPLUG_AUV2_VIEW_CLASS BeepboxView
#define CPLUG_AUV2_VIEW_CLASS_STR "BeepboxView"
static const unsigned kAudioUnitProperty_UserPlugin = 'plug';

#define CPLUG_CLAP_ID "us.pkhead.beepbox"
#define CPLUG_CLAP_DESCRIPTION "Emulation of BeepBox's synth"
#define CPLUG_CLAP_FEATURES CLAP_PLUGIN_FEATURE_INSTRUMENT, CLAP_PLUGIN_FEATURE_STEREO

#define CPLUG_NUM_PARAMS 11

#endif // PLUGIN_CONFIG_H
