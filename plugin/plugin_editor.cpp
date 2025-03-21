#include "plugin_processor.hpp"
#include "plugin_editor.hpp"
#include <backends/imgui_impl_opengl3.h>
#include <imgui_impl_juce/imgui_impl_juce.h>

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor(p)
{
    setOpaque(true);
    setWantsKeyboardFocus(true);
    setSize (400, 400);

    gl_context.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);
    gl_context.setRenderer(this);
    gl_context.attachTo(*this);
    gl_context.setContinuousRepainting(true);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

void AudioPluginAudioProcessorEditor::newOpenGLContextCreated()
{
  IMGUI_CHECKVERSION();
  imgui_ctx = ImGui::CreateContext();
  ImGui::SetCurrentContext(imgui_ctx);

  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = nullptr; // we will store gui layout manually
  //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  //loadLayout();

  ImGui_ImplJuce_Init(*this, gl_context);
  ImGui_ImplOpenGL3_Init();
}

void AudioPluginAudioProcessorEditor::renderOpenGL()
{
    using namespace juce::gl;

    ImGui::SetCurrentContext(imgui_ctx);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplJuce_NewFrame();
    ImGui::NewFrame();
    drawImGui();
    ImGui::Render();

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO &io = ImGui::GetIO();
    if (io.WantSaveIniSettings) {
        //saveLayout();
        io.WantSaveIniSettings = false;
    }
}

void AudioPluginAudioProcessorEditor::openGLContextClosing()
{
    ImGui::SetCurrentContext(imgui_ctx);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplJuce_Shutdown();
    //saveLayout();
    ImGui::DestroyContext();
}

template <typename T>
void imguiParam(T *param);

template <>
void imguiParam(juce::AudioParameterFloat *param) {
    float v = *param;
    float min = param->convertFrom0to1(0);
    float max = param->convertFrom0to1(1);

    if (ImGui::SliderFloat(param->name.toRawUTF8(), &v, min, max))
        *param = v;
}

template <>
void imguiParam(juce::AudioParameterInt *param) {
    int v = *param;
    int min = (int) param->convertFrom0to1(0);
    int max = (int) param->convertFrom0to1(1);

    if (ImGui::SliderInt(param->name.toRawUTF8(), &v, min, max))
        *param = v;
}

void AudioPluginAudioProcessorEditor::drawImGui()
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2());
    ImGui::SetNextWindowSize(viewport->Size);

    if (ImGui::Begin("window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings))
    {
        imguiParam(processor.param.algorithm);

        imguiParam(processor.param.op1_freq);
        imguiParam(processor.param.op1_vol);

        imguiParam(processor.param.op2_freq);
        imguiParam(processor.param.op2_vol);

        imguiParam(processor.param.op3_freq);
        imguiParam(processor.param.op3_vol);

        imguiParam(processor.param.op4_freq);
        imguiParam(processor.param.op4_vol);
    }
    ImGui::End();

    //ImGui::ShowDemoWindow();
}