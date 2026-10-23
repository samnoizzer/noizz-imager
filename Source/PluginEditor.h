#pragma once
#include "PluginProcessor.h"

//==============================================================================
// Custom LookAndFeel for Brushed Metal Neon Knobs & Bottom Bar Buttons
//==============================================================================
class NoizzExactLookAndFeel : public juce::LookAndFeel_V4
{
public:
    NoizzExactLookAndFeel();

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, const float rotaryStartAngle,
                           const float rotaryEndAngle, juce::Slider& slider) override;
};

//==============================================================================
// Polar Dome Vectorscope (Center Top Display)
//==============================================================================
class PolarVectorscope : public juce::Component
{
public:
    explicit PolarVectorscope (NoizzImagerAudioProcessor& p) : processor (p) {}
    void paint (juce::Graphics& g) override;

private:
    NoizzImagerAudioProcessor& processor;
};

//==============================================================================
// Dual Segmented LED Meter with Center dB Scale (0 to -60)
//==============================================================================
class SegmentedSciFiMeter : public juce::Component
{
public:
    SegmentedSciFiMeter() = default;
    void setLevels (float leftDb, float rightDb, float peakLDb, float peakRDb, bool isMS);
    void paint (juce::Graphics& g) override;

private:
    float lDb = -100.0f, rDb = -100.0f;
    float pLDb = -100.0f, pRDb = -100.0f;
    bool msMode = false;
};

//==============================================================================
// Main Plugin Editor (1000 x 560 Reference Aspect Ratio, Fully Resizable)
//==============================================================================
class NoizzImagerAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    explicit NoizzImagerAudioProcessorEditor (NoizzImagerAudioProcessor&);
    ~NoizzImagerAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void paintOverChildren (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    void drawCircuitTraces (juce::Graphics& g, float scale);
    void drawKnobScaleAndLabels (juce::Graphics& g, juce::Rectangle<float> knobBounds,
                                 const juce::String& title, const juce::String& unit,
                                 const std::vector<juce::String>& tickLabels, float scale);
    void openPresetMenu();
    void saveUserPresetDialog();

    NoizzImagerAudioProcessor& audioProcessor;
    NoizzExactLookAndFeel exactLnF;

    // 5 Rotary Knobs
    juce::Slider inTrimSlider, outTrimSlider, shuffleSlider, spaceSlider, widthSlider;

    // Hardware Square Buttons ("IN" and "SOLO SIDE") & M/S Meter Toggles
    juce::ToggleButton inActiveBtn { "IN" };
    juce::ToggleButton soloSideBtn { "SOLO\nSIDE" };
    juce::ToggleButton inMSBtn     { "M / S" };
    juce::ToggleButton outMSBtn    { "M / S" };

    // Bottom Bar Controls
    juce::TextButton undoBtn          { "UNDO" };
    juce::TextButton redoBtn          { "REDO" };
    juce::TextButton prevPresetBtn    { "<" };
    juce::TextButton presetDisplayBtn { "Default Preset" };
    juce::TextButton nextPresetBtn    { ">" };
    juce::TextButton savePresetBtn    { "SAVE" };
    juce::TextButton btnA             { "A" };
    juce::TextButton btnB             { "B" };

    // Custom Displays
    PolarVectorscope vectorscope;
    SegmentedSciFiMeter inputMeter, outputMeter;

    // APVTS Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<SliderAttachment> inTrimAtt, outTrimAtt, shuffleAtt, spaceAtt, widthAtt;
    std::unique_ptr<ButtonAttachment> soloSideAtt, inMSAtt, outMSAtt;
    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoizzImagerAudioProcessorEditor)
};