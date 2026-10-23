#include "PluginProcessor.h"
#include "PluginEditor.h"

NoizzImagerAudioProcessor::NoizzImagerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
#endif
       apvts (*this, &undoManager, "Parameters", createParameterLayout())
{
    initFactoryPresets();
    stateA = apvts.copyState();
    stateB = apvts.copyState();
}

NoizzImagerAudioProcessor::~NoizzImagerAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout NoizzImagerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "INPUT_GAIN", "Input Trim",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f), 0.0f, "dB"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "SHUFFLE", "Shuffle",
        juce::NormalisableRange<float> (100.0f, 400.0f, 1.0f), 220.0f, "Hz"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "SPACE", "Space",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f), 0.0f, "dB"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "WIDTH", "Width",
        juce::NormalisableRange<float> (-6.0f, 6.0f, 0.1f), 0.0f, "dB"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "OUTPUT_TRIM", "Output Trim",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f), 0.0f, "dB"));

    params.push_back (std::make_unique<juce::AudioParameterBool> ("BYPASS", "In / Bypass", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> ("SOLO_SIDE", "Solo Side", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> ("IN_MS_MODE", "Input M/S Meter", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> ("OUT_MS_MODE", "Output M/S Meter", false));

    return { params.begin(), params.end() };
}

void NoizzImagerAudioProcessor::initFactoryPresets()
{
    factoryPresets = {
        { "Init - Flat Matrix",            "Default",      0.0f, 220.0f,  0.0f,  0.0f,  0.0f },
        { "Drums - Massive Room Shuffler", "Drums",        0.0f, 280.0f,  4.5f,  2.5f, -1.0f },
        { "Drums - Tight Overheads",       "Drums",        0.0f, 130.0f, -4.0f,  3.0f,  0.0f },
        { "Drums - Wide Industrial Kit",   "Drums",        1.0f, 340.0f,  5.0f,  4.0f, -1.5f },
        { "Vocals - Lead Air & Halo",      "Vocals",       0.0f, 360.0f, -2.5f,  3.5f, -0.5f },
        { "Vocals - Wide Backing Choir",   "Vocals",       0.0f, 260.0f,  3.5f,  5.0f, -1.5f },
        { "Guitars - Thrash Quad Wall",    "Guitars",      0.0f, 180.0f,  3.5f,  3.2f, -1.0f },
        { "Guitars - Focused Rhythm",      "Guitars",      0.0f, 120.0f, -5.0f,  2.2f,  0.5f },
        { "Guitars - Clean Shimmer",       "Guitars",      0.0f, 300.0f,  4.0f,  4.0f, -0.5f },
        { "Bass - Sub Mono Anchor",        "Bass",         0.0f, 160.0f, -12.0f,-4.0f,  1.0f },
        { "Bass - Wide Distortion Clank",  "Bass",         0.5f, 380.0f, -8.0f,  4.5f,  0.0f },
        { "Synth - Cyberpunk Lead",        "Synth",        0.0f, 240.0f,  3.0f,  3.8f, -0.8f },
        { "Synth - Wide Reese Sub Control","Synth",        0.0f, 140.0f, -9.0f,  5.0f,  0.0f },
        { "Pads - Event Horizon Nebula",   "Pads",         0.0f, 320.0f,  6.5f,  5.5f, -2.0f },
        { "Pads - Dark Ambient Wrap",      "Pads",         0.0f, 220.0f,  5.0f,  4.5f, -1.2f },
        { "Strings - Cinematic Hall Depth","Strings",      0.0f, 200.0f,  4.2f,  3.0f, -1.0f },
        { "Strings - Intimate Chamber",    "Strings",      0.0f, 150.0f,  2.0f,  1.5f,  0.0f },
        { "Horns - Wide Brass Section",    "Horns",        0.0f, 280.0f,  3.0f,  3.5f, -0.5f },
        { "Bus - All-Round Glue Widener",  "Bus Channels", 0.0f, 190.0f,  2.2f,  2.5f, -0.5f },
        { "Bus - Drum & Perc Bus Expand",  "Bus Channels", 0.0f, 160.0f,  1.8f,  2.0f, -0.3f },
        { "Mix - Master Sweet Spot (+3dB)","Mix",          0.0f, 220.0f,  2.5f,  3.0f, -0.7f },
        { "Mix - Deep Blumlein Stage",     "Mix",          0.0f, 175.0f,  3.8f,  1.8f, -0.6f }
    };
}

int NoizzImagerAudioProcessor::getNumPrograms() { return (int) factoryPresets.size(); }

const juce::String NoizzImagerAudioProcessor::getProgramName (int index)
{
    if (juce::isPositiveAndBelow (index, (int) factoryPresets.size()))
        return factoryPresets[index].name;
    return {};
}

void NoizzImagerAudioProcessor::setCurrentProgram (int index) { loadFactoryPreset (index); }

void NoizzImagerAudioProcessor::loadFactoryPreset (int index)
{
    if (! juce::isPositiveAndBelow (index, (int) factoryPresets.size()))
        return;

    currentPresetIndex = index;
    const auto& p = factoryPresets[index];
    currentPresetName = p.name;

    auto setParam = [this] (const juce::String& id, float val)
    {
        if (auto* param = apvts.getParameter (id))
            param->setValueNotifyingHost (param->convertTo0to1 (val));
    };

    setParam ("INPUT_GAIN",  p.inputGain);
    setParam ("SHUFFLE",     p.shuffleFreq);
    setParam ("SPACE",       p.spaceDb);
    setParam ("WIDTH",       p.widthDb);
    setParam ("OUTPUT_TRIM", p.outputTrim);
}

void NoizzImagerAudioProcessor::saveUserPreset (const juce::File& file)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    if (xml != nullptr)
    {
        xml->setAttribute ("presetName", file.getFileNameWithoutExtension());
        xml->writeTo (file);
        currentPresetName = file.getFileNameWithoutExtension();
    }
}

void NoizzImagerAudioProcessor::loadUserPreset (const juce::File& file)
{
    std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse (file);
    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
    {
        currentPresetName = xml->getStringAttribute ("presetName", file.getFileNameWithoutExtension());
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
    }
}

void NoizzImagerAudioProcessor::switchAB (bool selectB)
{
    if (selectB == usingStateB) return;
    if (selectB)
    {
        stateA = apvts.copyState();
        apvts.replaceState (stateB.createCopy());
    }
    else
    {
        stateB = apvts.copyState();
        apvts.replaceState (stateA.createCopy());
    }
    usingStateB = selectB;
}

void NoizzImagerAudioProcessor::copyAtoB()
{
    if (usingStateB) stateA = apvts.copyState();
    else             stateB = apvts.copyState();
}

void NoizzImagerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels = 1;

    sideSpaceFilter.prepare (spec);
    sideSpaceFilter.reset();

    updateSpaceFilter (apvts.getRawParameterValue ("SHUFFLE")->load(),
                       apvts.getRawParameterValue ("SPACE")->load());
}

void NoizzImagerAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NoizzImagerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

void NoizzImagerAudioProcessor::updateSpaceFilter (float shuffleFreq, float spaceDb)
{
    float gainFactor = juce::Decibels::decibelsToGain (spaceDb);
    *sideSpaceFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowShelf (
        currentSampleRate, juce::jlimit (100.0f, 400.0f, shuffleFreq), 0.65f, gainFactor);
}

void NoizzImagerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    if (buffer.getNumChannels() < 2) return;

    const int numSamples = buffer.getNumSamples();
    float* leftChannel  = buffer.getWritePointer (0);
    float* rightChannel = buffer.getWritePointer (1);

    const float inGainLinear  = juce::Decibels::decibelsToGain (apvts.getRawParameterValue ("INPUT_GAIN")->load());
    const float shuffleFreq   = apvts.getRawParameterValue ("SHUFFLE")->load();
    const float spaceDb       = apvts.getRawParameterValue ("SPACE")->load();
    const float widthLinear   = juce::Decibels::decibelsToGain (apvts.getRawParameterValue ("WIDTH")->load());
    const float outGainLinear = juce::Decibels::decibelsToGain (apvts.getRawParameterValue ("OUTPUT_TRIM")->load());

    const bool bypassed  = apvts.getRawParameterValue ("BYPASS")->load() > 0.5f;
    const bool soloSide  = apvts.getRawParameterValue ("SOLO_SIDE")->load() > 0.5f;
    const bool inMSMode  = apvts.getRawParameterValue ("IN_MS_MODE")->load() > 0.5f;
    const bool outMSMode = apvts.getRawParameterValue ("OUT_MS_MODE")->load() > 0.5f;

    updateSpaceFilter (shuffleFreq, spaceDb);

    float inPeakBlockL = 0.0f, inPeakBlockR = 0.0f;
    float outPeakBlockL = 0.0f, outPeakBlockR = 0.0f;
    float sumLR = 0.0f, sumL2 = 0.0f, sumR2 = 0.0f;
    int writeIdx = scopeWriteIndex.load();

    for (int i = 0; i < numSamples; ++i)
    {
        float L = leftChannel[i]  * inGainLinear;
        float R = rightChannel[i] * inGainLinear;

        float M = (L + R) * 0.5f;
        float S = (L - R) * 0.5f;

        float inMeter1 = inMSMode ? std::abs (M) : std::abs (L);
        float inMeter2 = inMSMode ? std::abs (S) : std::abs (R);
        inPeakBlockL = std::max (inPeakBlockL, inMeter1);
        inPeakBlockR = std::max (inPeakBlockR, inMeter2);

        if (! bypassed)
        {
            S = sideSpaceFilter.processSample (S);
            S *= widthLinear;

            if (soloSide)
            {
                L = S;
                R = -S;
            }
            else
            {
                L = M + S;
                R = M - S;
            }

            L *= outGainLinear;
            R *= outGainLinear;
        }
        else
        {
            L = leftChannel[i];
            R = rightChannel[i];
        }

        leftChannel[i]  = L;
        rightChannel[i] = R;

        float outM = (L + R) * 0.5f;
        float outS = (L - R) * 0.5f;
        float outMeter1 = outMSMode ? std::abs (outM) : std::abs (L);
        float outMeter2 = outMSMode ? std::abs (outS) : std::abs (R);
        outPeakBlockL = std::max (outPeakBlockL, outMeter1);
        outPeakBlockR = std::max (outPeakBlockR, outMeter2);

        sumLR += L * R;
        sumL2 += L * L;
        sumR2 += R * R;

        if ((i & 3) == 0)
        {
            scopePoints[writeIdx] = { L, R };
            writeIdx = (writeIdx + 1) % scopeBufferSize;
        }
    }

    scopeWriteIndex.store (writeIdx);

    float denom = std::sqrt (sumL2 * sumR2);
    if (denom > 1e-6f)
        phaseCorrelation.store (juce::jlimit (-1.0f, 1.0f, sumLR / denom));

    const int holdSamples = (int) (currentSampleRate * 3.0);
    auto updateMeterAndHold = [&] (float blockLinear, std::atomic<float>& levelDb,
                                   std::atomic<float>& peakDb, int& holdCounter)
    {
        float dB = juce::Decibels::gainToDecibels (blockLinear, -100.0f);
        float currentLvl = levelDb.load();
        levelDb.store ((dB > currentLvl) ? dB : (currentLvl * 0.85f + dB * 0.15f));

        float curPeak = peakDb.load();
        if (dB >= curPeak)
        {
            peakDb.store (dB);
            holdCounter = holdSamples;
        }
        else
        {
            holdCounter -= numSamples;
            if (holdCounter <= 0) peakDb.store (curPeak - 0.8f);
        }
    };

    updateMeterAndHold (inPeakBlockL,  inLevelLeft,  inPeakLeft,  inHoldCounterL);
    updateMeterAndHold (inPeakBlockR,  inLevelRight, inPeakRight, inHoldCounterR);
    updateMeterAndHold (outPeakBlockL, outLevelLeft, outPeakLeft, outHoldCounterL);
    updateMeterAndHold (outPeakBlockR, outLevelRight,outPeakRight,outHoldCounterR);
}

juce::AudioProcessorEditor* NoizzImagerAudioProcessor::createEditor()
{
    return new NoizzImagerAudioProcessorEditor (*this);
}

void NoizzImagerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void NoizzImagerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NoizzImagerAudioProcessor();
}
