#pragma once
#include <JuceHeader.h>

class NoizzImagerAudioProcessor : public juce::AudioProcessor
{
public:
    NoizzImagerAudioProcessor();
    ~NoizzImagerAudioProcessor() override = default;
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    std::atomic<float> inL{0}, inR{0}, outL{0}, outR{0}, midMeter{0}, sideMeter{0};
    std::atomic<float> scopeL{0}, scopeR{0};
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoizzImagerAudioProcessor)
};
