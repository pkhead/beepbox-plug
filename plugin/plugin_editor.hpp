#pragma once

#include <imgui.h>
#include <juce_opengl/juce_opengl.h>
#include "plugin_processor.hpp"

//==============================================================================
class AudioPluginAudioProcessorEditor final :
    public juce::AudioProcessorEditor,
    public juce::OpenGLRenderer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;
    
    // regular ui not used
    void paint(juce::Graphics &) override {}
    void resized() override {}

    // imgui needs opengl rendering
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    void setScaleFactor(float v) override { (void)v; }

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processor;
    
    juce::OpenGLContext gl_context;
    ImGuiContext *imgui_ctx;

    void drawImGui();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};