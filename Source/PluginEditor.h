#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class NoizzImagerAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit NoizzImagerAudioProcessorEditor(NoizzImagerAudioProcessor&);
    ~NoizzImagerAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override;
    void loadPreset(int);
    void saveUserPreset();
    void setupKnob(juce::Slider&, juce::Label&, const juce::String&);
    NoizzImagerAudioProcessor& proc;
    juce::Label title, subtitle, presetLabel;
    juce::ComboBox presets;
    juce::TextButton saveButton{"SAVE"}, msButton{"SOLO SIDE"}, inButton{"IN"}, undoButton{"UNDO"}, redoButton{"REDO"}, aButton{"A"}, bButton{"B"};
    juce::Slider input, output, width, space, shuffle, mix;
    juce::Label inputLabel, outputLabel, widthLabel, spaceLabel, shuffleLabel, mixLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment aIn, aOut, aWidth, aSpace, aShuffle, aMix;
    juce::AudioProcessorValueTreeState::ButtonAttachment aMS;
    juce::Image logo;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoizzImagerAudioProcessorEditor)
};
