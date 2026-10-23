#include "PluginProcessor.h"
#include "PluginEditor.h"

NoizzImagerAudioProcessor::NoizzImagerAudioProcessor()
: AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)), parameters(*this, nullptr, "NOIZZ_STATE", createParameters()) {}

juce::AudioProcessorValueTreeState::ParameterLayout NoizzImagerAudioProcessor::createParameters()
{
    using P = juce::AudioProcessorValueTreeState;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    auto add = [&p](const juce::String& id, const juce::String& name, float lo, float hi, float def, float step = 0.0f) {
        p.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id, 1}, name, juce::NormalisableRange<float>(lo, hi, step), def)); };
    add("input", "Input", -24, 12, 0); add("output", "Output", -24, 12, 0);
    add("width", "Width", 0, 200, 100); add("space", "Space", -100, 100, 0);
    add("shuffle", "Shuffle", 200, 2000, 700); add("mix", "Mix", 0, 100, 100);
    p.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"msmonitor",1}, "M/S Monitor", false));
    return {p.begin(), p.end()};
}
void NoizzImagerAudioProcessor::prepareToPlay(double, int) {}
bool NoizzImagerAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const { return l.getMainInputChannelSet() == juce::AudioChannelSet::stereo() && l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo(); }
void NoizzImagerAudioProcessor::processBlock(juce::AudioBuffer<float>& b, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto n = b.getNumSamples(); if (b.getNumChannels() < 2) return;
    auto* l = b.getWritePointer(0); auto* r = b.getWritePointer(1);
    const float ig = juce::Decibels::decibelsToGain(parameters.getRawParameterValue("input")->load());
    const float og = juce::Decibels::decibelsToGain(parameters.getRawParameterValue("output")->load());
    const float width = parameters.getRawParameterValue("width")->load() * .01f;
    const float space = parameters.getRawParameterValue("space")->load() * .01f;
    const float shuffle = parameters.getRawParameterValue("shuffle")->load();
    const float mix = parameters.getRawParameterValue("mix")->load() * .01f;
    const bool ms = parameters.getRawParameterValue("msmonitor")->load() > .5f;
    float il=0, ir=0, ol=0, orr=0, mm=0, ss=0, sl=0, sr=0;
    // One-pole frequency split gives Space/Shuffle a smooth frequency-dependent side adjustment.
    const float coeff = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * juce::jlimit(20.0f, 18000.0f, shuffle) / (float)getSampleRate());
    static thread_local float lowL=0, lowR=0;
    for (int i=0;i<n;++i) {
        float a=l[i], c=r[i]; il=juce::jmax(il,std::abs(a)); ir=juce::jmax(ir,std::abs(c));
        float mid=(a+c)*0.5f*ig, side=(a-c)*0.5f*ig;
        lowL += coeff*(side-lowL); lowR += coeff*(side-lowR);
        float shapedSide = side * width + space * (lowL - (side-lowL));
        float wetL=(mid+shapedSide)*og, wetR=(mid-shapedSide)*og;
        float dryL=a*ig*og, dryR=c*ig*og;
        float x=juce::jmap(mix,dryL,wetL), y=juce::jmap(mix,dryR,wetR);
        if(ms){ x=mid+side; y=mid-side; }
        l[i]=x; r[i]=y; ol=juce::jmax(ol,std::abs(x)); orr=juce::jmax(orr,std::abs(y)); mm=juce::jmax(mm,std::abs(mid)); ss=juce::jmax(ss,std::abs(side)); sl=x; sr=y;
    }
    inL.store(il); inR.store(ir); outL.store(ol); outR.store(orr); midMeter.store(mm); sideMeter.store(ss); scopeL.store(sl); scopeR.store(sr);
}
juce::AudioProcessorEditor* NoizzImagerAudioProcessor::createEditor() { return new NoizzImagerAudioProcessorEditor(*this); }
void NoizzImagerAudioProcessor::getStateInformation(juce::MemoryBlock& d) { auto s=parameters.copyState(); std::unique_ptr<juce::XmlElement> x(s.createXml()); copyXmlToBinary(*x,d); }
void NoizzImagerAudioProcessor::setStateInformation(const void* d,int n) { std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d,n)); if(x && x->hasTagName(parameters.state.getType())) parameters.replaceState(juce::ValueTree::fromXml(*x)); }
