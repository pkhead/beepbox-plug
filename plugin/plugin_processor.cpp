#include "plugin_processor.hpp"
#include "plugin_editor.hpp"
#include <beepbox_synth.h>

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
    addParameter(param.algorithm = new juce::AudioParameterInt("algorithm",
        "Algorithm",
        0, 12, 0));

    addParameter(param.op1_freq = new juce::AudioParameterFloat("freq1",
        "Operator 1 Frequency",
        1.f, 20.f, 1.f));

    addParameter(param.op1_vol = new juce::AudioParameterFloat("vol1",
        "Operator 1 Volume",
        0.f, 1.f, 1.f));

    addParameter(param.op2_freq = new juce::AudioParameterFloat("freq2",
        "Operator 2 Frequency",
        1.f, 20.f, 1.f));

    addParameter(param.op2_vol = new juce::AudioParameterFloat("vol2",
        "Operator 2 Volume",
        0.f, 1.f, 0.f));

    addParameter(param.op3_freq = new juce::AudioParameterFloat("freq3",
        "Operator 3 Frequency",
        1.f, 20.f, 1.f));

    addParameter(param.op3_vol = new juce::AudioParameterFloat("vol3",
        "Operator 3 Volume",
        0.f, 1.f, 0.f));

    addParameter(param.op4_freq = new juce::AudioParameterFloat("freq4",
        "Operator 4 Frequency",
        1.f, 20.f, 1.f));

    addParameter(param.op4_vol = new juce::AudioParameterFloat("vol4",
        "Operator 4 Volume",
        0.f, 1.f, 0.f));
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    constexpr int channel_count = 2;

    synth = beepbox::inst_new(beepbox::INSTRUMENT_FM, (int)sampleRate, channel_count);
    interleaved_block = new float[samplesPerBlock * channel_count];
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    beepbox::inst_destroy(synth);
    synth = nullptr;

    delete[] interleaved_block;
    interleaved_block = nullptr;
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    assert(totalNumInputChannels == 0);
    assert(totalNumOutputChannels == 2);
    assert(synth);
    assert(interleaved_block);

    float *left_channel = buffer.getWritePointer(0);
    float *right_channel = buffer.getWritePointer(1);

    for (const auto metadata : midiMessages) {
        auto message = metadata.getMessage();
        const auto time = metadata.samplePosition;
        
        if (message.isNoteOn()) {
            beepbox::inst_midi_on(synth, message.getNoteNumber(), message.getVelocity());
        } else if (message.isNoteOff()) {
            beepbox::inst_midi_off(synth, message.getNoteNumber(), message.getVelocity());
        }
    }
    
    beepbox::inst_set_param_int(synth, 0, *param.algorithm);
    beepbox::inst_set_param_float(synth, 1, *param.op1_freq);
    beepbox::inst_set_param_float(synth, 2, *param.op1_vol);
    beepbox::inst_set_param_float(synth, 3, *param.op2_freq);
    beepbox::inst_set_param_float(synth, 4, *param.op2_vol);
    beepbox::inst_set_param_float(synth, 5, *param.op3_freq);
    beepbox::inst_set_param_float(synth, 6, *param.op3_vol);
    beepbox::inst_set_param_float(synth, 7, *param.op4_freq);
    beepbox::inst_set_param_float(synth, 8, *param.op4_vol);

    beepbox::inst_run(synth, interleaved_block, buffer.getNumSamples());

    int j = 0;
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        *left_channel++ = interleaved_block[j++];
        *right_channel++ = interleaved_block[j++];
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}