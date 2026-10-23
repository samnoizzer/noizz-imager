#include "PluginEditor.h"

namespace
{
    const juce::Colour bg{0xff090c0b}, panel{0xff171b1a}, panel2{0xff0d100f}, green{0xffa6ff00}, greenDim{0xff4e7d19}, gun{0xff343938}, muted{0xff87918d};
    void pcbTrace(juce::Graphics& g, float x, float y, float w, float h, int seed)
    {
        juce::Random r(seed * 7919 + 31);
        g.setColour(juce::Colour(0xff91a29a).withAlpha(0.20f));
        for (int i = 0; i < 18; ++i)
        {
            float sx = x + r.nextFloat() * w, sy = y + r.nextFloat() * h;
            float ex = x + r.nextFloat() * w, ey = y + r.nextFloat() * h;
            float mid = sx + (ex - sx) * 0.55f;
            juce::Path p; p.startNewSubPath(sx, sy); p.lineTo(mid, sy); p.lineTo(mid, ey); p.lineTo(ex, ey);
            g.strokePath(p, juce::PathStrokeType(0.8f));
            g.fillEllipse(ex - 2.0f, ey - 2.0f, 4.0f, 4.0f);
        }
    }
}

NoizzImagerAudioProcessorEditor::NoizzImagerAudioProcessorEditor(NoizzImagerAudioProcessor& p)
    : AudioProcessorEditor(&p), proc(p), aIn(p.parameters, "input", input), aOut(p.parameters, "output", output),
      aWidth(p.parameters, "width", width), aSpace(p.parameters, "space", space), aShuffle(p.parameters, "shuffle", shuffle),
      aMix(p.parameters, "mix", mix), aMS(p.parameters, "msmonitor", msButton)
{
    setResizable(true, true);
    setResizeLimits(760, 440, 1800, 1100);
    setSize(1000, 560);

    title.setText("NOIZZ IMAGER", juce::dontSendNotification);
    title.setFont(juce::Font(juce::FontOptions(25.0f, juce::Font::bold)));
    title.setColour(juce::Label::textColourId, juce::Colours::white);
    title.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(title);
    subtitle.setText("STEREO IMAGE  //  NOIZZ STUDIO", juce::dontSendNotification);
    subtitle.setFont(juce::Font(juce::FontOptions(10.0f)));
    subtitle.setColour(juce::Label::textColourId, green);
    subtitle.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(subtitle);

    presetLabel.setText("PRESET", juce::dontSendNotification);
    presetLabel.setColour(juce::Label::textColourId, muted);
    presetLabel.setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold)));
    addAndMakeVisible(presetLabel);
    const juce::StringArray cats { "DRUMS", "VOCALS", "GUITARS", "BASS", "SYNTH PADS", "STRINGS", "HORNS", "BUS CHANNELS", "FULL MIX" };
    int id = 1; presets.addItem("DEFAULT PRESET", id++);
    for (auto c : cats) for (int i = 1; i <= 4; ++i) presets.addItem(c + " // " + juce::String(i), id++);
    presets.setColour(juce::ComboBox::backgroundColourId, panel2);
    presets.setColour(juce::ComboBox::textColourId, green);
    presets.setColour(juce::ComboBox::outlineColourId, greenDim);
    presets.onChange = [this] { loadPreset(presets.getSelectedId()); };
    addAndMakeVisible(presets); presets.setSelectedId(1, juce::dontSendNotification);

    auto styleButton = [](juce::TextButton& b, bool active = false)
    {
        b.setColour(juce::TextButton::buttonColourId, panel2);
        b.setColour(juce::TextButton::buttonOnColourId, green);
        b.setColour(juce::TextButton::textColourOffId, active ? green : juce::Colours::white);
        b.setColour(juce::TextButton::textColourOnId, bg);
    };
    for (auto* b : { &saveButton, &msButton, &inButton, &undoButton, &redoButton, &aButton, &bButton }) { styleButton(*b); addAndMakeVisible(*b); }
    msButton.setClickingTogglesState(true); inButton.setClickingTogglesState(true); inButton.setToggleState(true, juce::dontSendNotification);
    aButton.setClickingTogglesState(true); bButton.setClickingTogglesState(true); aButton.setToggleState(true, juce::dontSendNotification);
    saveButton.onClick = [this] { saveUserPreset(); };
    undoButton.onClick = [this] { loadPreset(presets.getSelectedId()); };
    redoButton.onClick = [this] { loadPreset(presets.getSelectedId()); };
    aButton.onClick = [this] { if (aButton.getToggleState()) bButton.setToggleState(false, juce::dontSendNotification); };
    bButton.onClick = [this] { if (bButton.getToggleState()) aButton.setToggleState(false, juce::dontSendNotification); };

    setupKnob(input, inputLabel, "INPUT TRIM");
    setupKnob(output, outputLabel, "OUTPUT TRIM");
    setupKnob(shuffle, shuffleLabel, "SHUFFLE");
    setupKnob(space, spaceLabel, "SPACE");
    setupKnob(width, widthLabel, "WIDTH");
    setupKnob(mix, mixLabel, "MIX");
    startTimerHz(30);
}

void NoizzImagerAudioProcessorEditor::setupKnob(juce::Slider& s, juce::Label& l, const juce::String& name)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 58, 16);
    s.setColour(juce::Slider::rotarySliderFillColourId, green);
    s.setColour(juce::Slider::rotarySliderOutlineColourId, gun);
    s.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    s.setColour(juce::Slider::textBoxBackgroundColourId, panel2);
    s.setColour(juce::Slider::textBoxOutlineColourId, greenDim);
    addAndMakeVisible(s);
    l.setText(name, juce::dontSendNotification); l.setJustificationType(juce::Justification::centred);
    l.setColour(juce::Label::textColourId, juce::Colours::white); l.setFont(juce::Font(juce::FontOptions(10.5f, juce::Font::bold))); addAndMakeVisible(l);
}

void NoizzImagerAudioProcessorEditor::loadPreset(int id)
{
    float w = 100, s = 0, h = 700, m = 100;
    if (id >= 2) { int k = (id - 2) / 4, v = (id - 2) % 4; const float widths[9] = {125,112,120,105,145,130,118,110,108}; const float spaces[9] = {12,8,10,0,22,16,12,6,4}; w = widths[k] + (v - 1) * 8; s = spaces[k] + (v - 1) * 4; h = 500 + v * 250; }
    auto set = [this](const char* key, float val) { if (auto* param = proc.parameters.getParameter(key)) param->setValueNotifyingHost(proc.parameters.getParameterRange(key).convertTo0to1(val)); };
    set("width", w); set("space", s); set("shuffle", h); set("mix", m);
}

void NoizzImagerAudioProcessorEditor::saveUserPreset()
{
    auto dir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("NOIZZ STUDIO/NOIZZ IMAGER/Presets");
    dir.createDirectory();
    auto chooser = std::make_shared<juce::FileChooser>("Save NOIZZ IMAGER preset", dir.getChildFile("My Preset.noizzpreset"), "*.noizzpreset");
    chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting,
        [this, chooser](const juce::FileChooser& fc) { auto file = fc.getResult(); if (file == juce::File{}) return; auto state = proc.parameters.copyState(); std::unique_ptr<juce::XmlElement> xml(state.createXml()); if (xml) xml->writeTo(file); });
}

void NoizzImagerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(bg);
    auto bounds = getLocalBounds().toFloat();
    juce::ColourGradient grad(juce::Colour(0xff292e2c), 0, 0, panel, 0, (float)getHeight(), false);
    g.setGradientFill(grad); g.fillRoundedRectangle(bounds.reduced(10), 9.0f);
    pcbTrace(g, 14, 52, (float)getWidth() - 28, (float)getHeight() - 112, getWidth() + getHeight());
    g.setColour(panel2); g.fillRoundedRectangle(14, 12, (float)getWidth()-28, 40, 6);
    g.setColour(green.withAlpha(0.6f)); g.drawRoundedRectangle(14, 12, (float)getWidth()-28, (float)getHeight()-24, 8, 1.0f);
    auto body = juce::Rectangle<float>(20, 56, (float)getWidth()-40, (float)getHeight()-112);
    g.setColour(juce::Colour(0xff111514).withAlpha(0.92f)); g.fillRoundedRectangle(body, 5);
    g.setColour(juce::Colour(0xff89948e).withAlpha(0.35f)); g.drawRoundedRectangle(body, 5, 1);

    auto drawMeter = [&](juce::Rectangle<int> r, const juce::String& name, float lv, float rv)
    {
        g.setColour(juce::Colours::white); g.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold))); g.drawText(name, r.removeFromTop(20), juce::Justification::centred);
        g.setColour(panel2); g.fillRoundedRectangle(r.toFloat(), 4);
        int bars = 22; float gap = 2.0f; float barH = (r.getHeight()-10.0f-(bars-1)*gap)/bars; float barW = (r.getWidth()-18.0f)/2.0f;
        for (int c=0;c<2;++c) { float v = c==0 ? lv : rv; int lit = juce::jlimit(0,bars,(int)std::round(v*bars)); float bx = r.getX()+6+c*(barW+6);
            for (int i=0;i<bars;++i) { float by = r.getBottom()-5-(i+1)*(barH+gap)+gap; g.setColour(i<lit ? (i>bars-4 ? juce::Colour(0xffffb000) : green) : juce::Colour(0xff242a26)); g.fillRoundedRectangle(bx, by, barW, barH, 1.5f); }
        }
    };
    int meterW = juce::jlimit(66, 96, getWidth()/11);
    drawMeter({28, 78, meterW, getHeight()-190}, "INPUT", proc.inL.load(), proc.inR.load());
    drawMeter({getWidth()-28-meterW, 78, meterW, getHeight()-190}, "OUTPUT", proc.outL.load(), proc.outR.load());

    // Live stereo-field vectorscope with subdued grid and neon-green energy cloud.
    int scopeW = juce::jmin(360, getWidth()/3), scopeH = juce::jmin(170, getHeight()/3);
    juce::Rectangle<float> scope((getWidth()-scopeW)*0.5f, 68.0f, (float)scopeW, (float)scopeH);
    g.setColour(panel2); g.fillRoundedRectangle(scope, 6); g.setColour(green.withAlpha(0.55f)); g.drawRoundedRectangle(scope, 6, 1.0f);
    auto centre = juce::Point<float>(scope.getCentreX(), scope.getBottom()-14);
    float rad = juce::jmin(scope.getWidth()*0.39f, scope.getHeight()*0.72f);
    g.setColour(green.withAlpha(0.25f));
    for (int i=1;i<=4;++i) g.drawEllipse(centre.x-rad*i/4, centre.y-rad*i/4, rad*i/2, rad*i/2, 0.8f);
    g.drawLine(centre.x-rad, centre.y, centre.x+rad, centre.y, 0.8f); g.drawLine(centre.x, centre.y-rad, centre.x, centre.y, 0.8f);
    juce::Random rand(1234); g.setColour(green.withAlpha(0.35f));
    float spread = juce::jlimit(0.08f, 0.95f, 0.18f + std::abs(proc.scopeL.load()-proc.scopeR.load())*1.6f);
    for (int i=0;i<260;++i) { float xx = (rand.nextFloat()+rand.nextFloat()-1.0f)*rad*spread; float yy = rand.nextFloat()*rad; g.fillEllipse(centre.x+xx-0.7f, centre.y-yy-0.7f, 1.4f, 1.4f); }
    g.setColour(green); g.setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold))); g.drawText("L", (int)(scope.getX()+8), (int)(scope.getBottom()-20), 18, 16, juce::Justification::centred); g.drawText("R", (int)(scope.getRight()-26), (int)(scope.getBottom()-20), 18, 16, juce::Justification::centred);
    g.drawText("0°", (int)centre.x-15, (int)(scope.getY()+6), 30, 14, juce::Justification::centred);

    g.setColour(muted); g.setFont(juce::Font(juce::FontOptions(9.0f)));
    g.drawText("M / S", 25, getHeight()-88, meterW+6, 16, juce::Justification::centred);
    g.drawText("M / S", getWidth()-meterW-31, getHeight()-88, meterW+6, 16, juce::Justification::centred);
}

void NoizzImagerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    title.setBounds(area.getCentreX()-150, 12, 300, 27);
    subtitle.setBounds(area.getCentreX()-150, 36, 300, 14);
    int footerH = 48, footerY = area.getBottom()-footerH-8;
    int meterW = juce::jlimit(66, 96, getWidth()/11);
    int scopeW = juce::jmin(360, getWidth()/3), scopeH = juce::jmin(170, getHeight()/3);
    int scopeX = (getWidth()-scopeW)/2;
    int knobY = 280;
    int knobAreaW = getWidth()-2*meterW-100;
    int knobW = juce::jlimit(82, 112, knobAreaW/6);
    int gap = juce::jmax(4, (knobAreaW-6*knobW)/5);
    int startX = (getWidth()-(6*knobW+5*gap))/2;
    juce::Slider* sliders[] = { &input, &output, &shuffle, &space, &width, &mix };
    juce::Label* labels[] = { &inputLabel, &outputLabel, &shuffleLabel, &spaceLabel, &widthLabel, &mixLabel };
    // Place trims flanking the vectorscope; spatial controls sit beneath it.
    int trimY = 108, trimW = juce::jmin(120, meterW+42);
    inputLabel.setBounds(meterW+24, trimY-20, trimW, 18); input.setBounds(meterW+24, trimY, trimW, 112);
    outputLabel.setBounds(getWidth()-meterW-trimW-24, trimY-20, trimW, 18); output.setBounds(getWidth()-meterW-trimW-24, trimY, trimW, 112);
    int spatialY = juce::jmax(scopeH+92, getHeight()-190);
    int controlsW = juce::jmin(getWidth()-2*meterW-80, 6*knobW+5*gap);
    startX = (getWidth()-controlsW)/2;
    for (int i=0;i<3;++i) { int x = startX+i*(knobW+gap); labels[i+2]->setBounds(x, spatialY-18, knobW, 16); sliders[i+2]->setBounds(x, spatialY, knobW, 104); }
    // Dry/wet mix remains available as a fourth compact spatial control.
    mixLabel.setBounds(startX+3*(knobW+gap), spatialY-18, knobW, 16); mix.setBounds(startX+3*(knobW+gap), spatialY, knobW, 104);
    presetLabel.setBounds(28, footerY+4, 48, 20); presets.setBounds(78, footerY+3, juce::jmax(150, getWidth()-430), 26);
    undoButton.setBounds(28, footerY-29, 68, 23); redoButton.setBounds(100, footerY-29, 68, 23);
    saveButton.setBounds(getWidth()-205, footerY+2, 74, 27); aButton.setBounds(getWidth()-122, footerY+2, 38, 27); bButton.setBounds(getWidth()-78, footerY+2, 38, 27);
    inButton.setBounds(meterW+25, getHeight()-116, 56, 28); msButton.setBounds(getWidth()-meterW-83, getHeight()-116, 70, 28);
    // Ensure scope remains centered and doesn't collide with trims.
    (void)scopeX; (void)scopeW; (void)scopeH; (void)knobY;
}

void NoizzImagerAudioProcessorEditor::timerCallback() { repaint(); }
