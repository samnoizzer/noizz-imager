#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"

struct FactoryPreset
{
    juce::String name;
    juce::String category;
    float inputGain;   // dB (-12 to +12)
    float shuffleFreq; // Hz (40 to 400)
    float spaceDb;     // dB (-12 to +12)
    float widthDb;     // dB (-12 to +12)
    float outputTrim;  // dB (-12 to +12)
};

class NoizzImagerAudioProcessor : public juce::AudioProcessor
{
public:
    NoizzImagerAudioProcessor();
    ~NoizzImagerAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "NOIZZ IMAGER"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override { return currentPresetIndex; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Parameter Tree & Undo Manager
    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Preset & A/B Comparison System
    const std::vector<FactoryPreset>& getFactoryPresets() const { return factoryPresets; }
    void loadFactoryPreset (int index);
    void saveUserPreset (const juce::File& file);
    void loadUserPreset (const juce::File& file);
    void switchAB (bool selectB);
    void copyAtoB();
    bool isStateBSelected() const { return usingStateB; }
    juce::String getCurrentPresetDisplayName() const { return currentPresetName; }

    // Lock-free Metering Atomics (Input/Output L/R or M/S + 3s Peak Hold)
    std::atomic<float> inLevelLeft  { -100.0f }, inLevelRight  { -100.0f };
    std::atomic<float> outLevelLeft { -100.0f }, outLevelRight { -100.0f };
    std::atomic<float> inPeakLeft   { -100.0f }, inPeakRight   { -100.0f };
    std::atomic<float> outPeakLeft  { -100.0f }, outPeakRight  { -100.0f };
    std::atomic<float> phaseCorrelation { 1.0f };

    // Lock-free Ring Buffer for Polar Sample Vectorscope
    static constexpr int scopeBufferSize = 2048;
    std::array<juce::Point<float>, scopeBufferSize> scopePoints;
    std::atomic<int> scopeWriteIndex { 0 };

private:
    void initFactoryPresets();
    void updateSpaceFilter (float shuffleFreq, float spaceDb);

    double currentSampleRate = 44100.0;
    juce::dsp::IIR::Filter<float> sideSpaceFilter;

    // Peak hold timers (in samples, 3 seconds hold)
    int inHoldCounterL = 0, inHoldCounterR = 0;
    int outHoldCounterL = 0, outHoldCounterR = 0;

    // A/B States & Presets
    bool usingStateB = false;
    juce::ValueTree stateA, stateB;
    int currentPresetIndex = 0;
    juce::String currentPresetName = "Init - Default Stereo";
    std::vector<FactoryPreset> factoryPresets;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoizzImagerAudioProcessor)
};