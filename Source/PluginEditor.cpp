#include "PluginEditor.h"

namespace { const juce::Colour bg{0xff080b0a}, panel{0xff111715}, green{0xffa6ff00}, muted{0xff64736a}; }
NoizzImagerAudioProcessorEditor::NoizzImagerAudioProcessorEditor(NoizzImagerAudioProcessor& p)
: AudioProcessorEditor(&p), proc(p), aIn(p.parameters,"input",input), aOut(p.parameters,"output",output), aWidth(p.parameters,"width",width), aSpace(p.parameters,"space",space), aShuffle(p.parameters,"shuffle",shuffle), aMix(p.parameters,"mix",mix), aMS(p.parameters,"msmonitor",msButton)
{
    setResizable(true,true); setResizeLimits(820,520,1600,1000); setSize(1100,680);
    auto asset = juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory().getChildFile("Assets/noizz-studio-logo.jpg");
    if (asset.existsAsFile()) logo=juce::ImageFileFormat::loadFrom(asset);
    title.setText("NOIZZ IMAGER", juce::dontSendNotification); title.setFont(juce::Font(30.0f,juce::Font::bold)); title.setColour(juce::Label::textColourId,green); addAndMakeVisible(title);
    subtitle.setText("STEREO FIELD PROCESSOR  //  SPATIAL ENGINE v1.0",juce::dontSendNotification); subtitle.setFont(juce::Font(11.0f)); subtitle.setColour(juce::Label::textColourId,muted); addAndMakeVisible(subtitle);
    presetLabel.setText("PRESET",juce::dontSendNotification); presetLabel.setColour(juce::Label::textColourId,green); addAndMakeVisible(presetLabel);
    const juce::StringArray cats{"DRUMS","VOCALS","GUITARS","BASS","SYNTH PADS","STRINGS","HORNS","BUS CHANNELS","FULL MIX"};
    int id=1; presets.addItem("INIT // NEUTRAL",id++);
    for(auto c:cats) for(int i=1;i<=4;++i) presets.addItem(c+" // "+juce::String(i),id++);
    presets.setColour(juce::ComboBox::backgroundColourId,panel); presets.setColour(juce::ComboBox::textColourId,juce::Colours::white); presets.setColour(juce::ComboBox::outlineColourId,green); presets.onChange=[this]{loadPreset(presets.getSelectedId());}; addAndMakeVisible(presets); presets.setSelectedId(1,juce::dontSendNotification);
    saveButton.setColour(juce::TextButton::buttonColourId,panel); saveButton.setColour(juce::TextButton::textColourOffId,green); saveButton.onClick=[this]{saveUserPreset();}; addAndMakeVisible(saveButton);
    msButton.setClickingTogglesState(true); msButton.setColour(juce::TextButton::buttonColourId,panel); msButton.setColour(juce::TextButton::buttonOnColourId,green); msButton.setColour(juce::TextButton::textColourOffId,green); addAndMakeVisible(msButton);
    auto setup=[this](juce::Slider& s,juce::Label& l,const juce::String& name){ s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag); s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,86,20); s.setColour(juce::Slider::rotarySliderFillColourId,green); s.setColour(juce::Slider::rotarySliderOutlineColourId,muted); s.setColour(juce::Slider::thumbColourId,juce::Colours::white); s.setColour(juce::Slider::textBoxTextColourId,juce::Colours::white); s.setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack); addAndMakeVisible(s); l.setText(name,juce::dontSendNotification); l.setJustificationType(juce::Justification::centred); l.setColour(juce::Label::textColourId,juce::Colours::white); l.setFont(juce::Font(12.0f,juce::Font::bold)); addAndMakeVisible(l); };
    setup(input,inputLabel,"INPUT (dB)"); setup(output,outputLabel,"OUTPUT (dB)"); setup(width,widthLabel,"WIDTH"); setup(space,spaceLabel,"SPACE"); setup(shuffle,shuffleLabel,"SHUFFLE (Hz)"); setup(mix,mixLabel,"MIX"); startTimerHz(30);
}
void NoizzImagerAudioProcessorEditor::styleKnob(juce::Slider&,const juce::String&){}
void NoizzImagerAudioProcessorEditor::loadPreset(int id)
{
    // Broad category presets with conservative, mix-safe stereo-width starting points.
    float w=100,s=0,h=700,m=100;
    if(id>=2){ int k=(id-2)/4; int variant=(id-2)%4;
        const float widths[9]={125,112,120,105,145,130,118,110,108}; const float spaces[9]={12,8,10,0,22,16,12,6,4};
        w=widths[k]+(variant-1)*8; s=spaces[k]+(variant-1)*4; h=500+variant*250; m=100; }
    proc.parameters.getParameter("width")->setValueNotifyingHost(proc.parameters.getParameterRange("width").convertTo0to1(w));
    proc.parameters.getParameter("space")->setValueNotifyingHost(proc.parameters.getParameterRange("space").convertTo0to1(s));
    proc.parameters.getParameter("shuffle")->setValueNotifyingHost(proc.parameters.getParameterRange("shuffle").convertTo0to1(h));
    proc.parameters.getParameter("mix")->setValueNotifyingHost(proc.parameters.getParameterRange("mix").convertTo0to1(m));
}
void NoizzImagerAudioProcessorEditor::saveUserPreset()
{
    auto dir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                   .getChildFile("NOIZZ STUDIO/NOIZZ IMAGER/Presets");
    dir.createDirectory();

    // JUCE 8 uses launchAsync; keep the chooser alive until its callback runs.
    static std::unique_ptr<juce::FileChooser> chooser;
    chooser = std::make_unique<juce::FileChooser>(
        "Save NOIZZ IMAGER preset", dir.getChildFile("My Preset.noizzpreset"), "*.noizzpreset");

    chooser->launchAsync(juce::FileBrowserComponent::saveMode |
                             juce::FileBrowserComponent::canSelectFiles |
                             juce::FileBrowserComponent::warnAboutOverwriting,
                         [this](const juce::FileChooser& fc)
                         {
                             const auto file = fc.getResult();
                             if (file == juce::File{})
                                 return;

                             auto state = proc.parameters.copyState();
                             std::unique_ptr<juce::XmlElement> xml(state.createXml());
                             if (xml != nullptr)
                                 xml->writeTo(file);
                         });
}
void NoizzImagerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(bg); g.setColour(green.withAlpha(.13f)); for(int x=0;x<getWidth();x+=32) g.drawVerticalLine(x,0.0f,(float)getHeight()); for(int y=0;y<getHeight();y+=32) g.drawHorizontalLine(y,0.0f,(float)getWidth());
    g.setColour(green); g.drawRect(getLocalBounds().reduced(8),1); g.setColour(panel); g.fillRoundedRectangle(18,112,(float)getWidth()-36, getHeight()-136,10); g.setColour(green.withAlpha(.5f)); g.drawRoundedRectangle(18,112,(float)getWidth()-36,getHeight()-136,10,1);
    auto meter=[&](int x,int y,const juce::String& name,std::atomic<float>& l,std::atomic<float>& r){ g.setColour(muted); g.drawText(name,x,y,130,18,juce::Justification::centredLeft); for(int c=0;c<2;++c){float v=juce::jlimit(0.0f,1.0f,(c==0?l.load():r.load())); g.setColour(juce::Colour(0xff253129)); g.fillRoundedRectangle((float)x+c*58,(float)y+24,48,9,3); g.setColour(green); g.fillRoundedRectangle((float)x+c*58,(float)y+24,48*v,9,3); }};
    meter(32,124,"INPUT PEAK",proc.inL,proc.inR); meter(178,124,"OUTPUT PEAK",proc.outL,proc.outR); meter(324,124,"MID / SIDE",proc.midMeter,proc.sideMeter);
    // Polar stereo field display: L/R samples are accumulated by the audio thread and shown as a live vector.
    int cx=getWidth()/2, cy=getHeight()/2+36; float rad=juce::jmin(95.0f,(float)getWidth()*.12f);
    g.setColour(muted.withAlpha(.7f)); for(int i=1;i<=3;++i) g.drawEllipse(cx-rad*i/3,cy-rad*i/3,rad*2*i/3,rad*2*i/3,1); g.drawLine((float)cx-rad,cy,(float)cx+rad,cy,1); g.drawLine(cx,cy-rad,cx,cy+rad,1);
    float sx=proc.scopeL.load(), sy=proc.scopeR.load(); g.setColour(green); g.drawLine((float)cx,(float)cy,cx+juce::jlimit(-rad,rad,(sx-sy)*rad*2),cy-juce::jlimit(-rad,rad,(sx+sy)*rad),2.5f); g.setFont(11); g.drawText("STEREO FIELD // POLAR VECTOR",cx-(int)rad-40,cy+(int)rad+8,(int)rad*2+80,18,juce::Justification::centred);
    if(!logo.isNull()){ g.setOpacity(.92f); g.drawImageWithin(logo,24,14,112,72,juce::RectanglePlacement::centred); g.setOpacity(1); }
    g.setColour(muted); g.setFont(10); g.drawText("NOIZZ STUDIO  •  MID/SIDE MATRIX  •  LOW-LATENCY DSP",24,getHeight()-20,getWidth()-48,14,juce::Justification::centred);
}
void NoizzImagerAudioProcessorEditor::resized()
{
    auto w=getWidth(); title.setBounds(150,18,w/2,40); subtitle.setBounds(152,58,w/2,20); presetLabel.setBounds(w-460,22,60,22); presets.setBounds(w-400,20,220,28); saveButton.setBounds(w-172,20,140,28); msButton.setBounds(w-172,56,140,26);
    int y=275, knobW=120, gap=(w-6*knobW-60)/5; int x=30; juce::Slider* sliders[]={&input,&output,&width,&space,&shuffle,&mix}; juce::Label* labels[]={&inputLabel,&outputLabel,&widthLabel,&spaceLabel,&shuffleLabel,&mixLabel};
    for(int i=0;i<6;++i){ labels[i]->setBounds(x,y-24,knobW,22); sliders[i]->setBounds(x,y,knobW,150); x+=knobW+gap; }
}
void NoizzImagerAudioProcessorEditor::timerCallback(){ repaint(); }
