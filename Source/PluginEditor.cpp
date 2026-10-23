#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace NoizzTheme
{
    const juce::Colour bgBlack      { 0xFF0A0C0E };
    const juce::Colour chassisDark  { 0xFF14181D };
    const juce::Colour bezelBlack   { 0xFF080A0C };
    const juce::Colour neonGreen    { 0xFF66FF11 };
    const juce::Colour brightLime   { 0xFF88FF22 };
    const juce::Colour dimGreen     { 0xFF285510 };
    const juce::Colour traceGreen   { 0xFF1F3818 };
    const juce::Colour amberYellow  { 0xFFFFB800 };
    const juce::Colour textWhite    { 0xFFEAEAEA };
    const juce::Colour textGray     { 0xFF9AA0A6 };
}

//==============================================================================
// CUSTOM LOOK AND FEEL: BRUSHED METAL KNOBS WITH NEON ARC & POINTER
//==============================================================================
NoizzExactLookAndFeel::NoizzExactLookAndFeel()
{
    setColour (juce::PopupMenu::backgroundColourId, NoizzTheme::bezelBlack);
    setColour (juce::PopupMenu::textColourId,       NoizzTheme::neonGreen);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, NoizzTheme::dimGreen);
}

void NoizzExactLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, const float rotaryStartAngle,
                                              const float rotaryEndAngle, juce::Slider&)
{
    auto bounds = juce::Rectangle<float> ((float)x, (float)y, (float)width, (float)height);
    float cx = bounds.getCentreX();
    float cy = bounds.getCentreY();
    float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.36f;
    float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // 1. Outer Dark Bezel Ring
    g.setColour (juce::Colour (0xFF07090B));
    g.fillEllipse (cx - radius * 1.12f, cy - radius * 1.12f, radius * 2.24f, radius * 2.24f);

    // 2. Continuous Neon Green Ring Track around the Knob Body
    juce::Path fullArc;
    fullArc.addCentredArc (cx, cy, radius * 1.05f, radius * 1.05f, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (NoizzTheme::dimGreen);
    g.strokePath (fullArc, juce::PathStrokeType (2.2f));

    juce::Path activeArc;
    activeArc.addCentredArc (cx, cy, radius * 1.05f, radius * 1.05f, 0.0f, rotaryStartAngle, angle, true);
    g.setColour (NoizzTheme::neonGreen);
    g.strokePath (activeArc, juce::PathStrokeType (2.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 3. Conical Brushed Metal Cap
    juce::ColourGradient metalGrad (juce::Colour (0xFF383E46), cx - radius * 0.5f, cy - radius * 0.8f,
                                    juce::Colour (0xFF121519), cx + radius * 0.5f, cy + radius * 0.8f, false);
    metalGrad.addColour (0.5, juce::Colour (0xFF242930));
    g.setGradientFill (metalGrad);
    g.fillEllipse (cx - radius * 0.94f, cy - radius * 0.94f, radius * 1.88f, radius * 1.88f);

    // Subtle concentric machined grooves
    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.drawEllipse (cx - radius * 0.94f, cy - radius * 0.94f, radius * 1.88f, radius * 1.88f, 1.5f);
    g.setColour (juce::Colours::white.withAlpha (0.06f));
    g.drawEllipse (cx - radius * 0.78f, cy - radius * 0.78f, radius * 1.56f, radius * 1.56f, 1.0f);

    // 4. Glowing Neon Green Indicator Needle
    juce::Path needleGlow;
    needleGlow.addRoundedRectangle (-3.5f, -radius * 0.88f, 7.0f, radius * 0.48f, 3.0f);
    g.setColour (NoizzTheme::neonGreen.withAlpha (0.35f));
    g.fillPath (needleGlow, juce::AffineTransform::rotation (angle).translated (cx, cy));

    juce::Path needle;
    needle.addRoundedRectangle (-2.0f, -radius * 0.85f, 4.0f, radius * 0.42f, 2.0f);
    g.setColour (NoizzTheme::brightLime);
    g.fillPath (needle, juce::AffineTransform::rotation (angle).translated (cx, cy));
}

//==============================================================================
// POLAR DOME VECTORSCOPE (EXACT MATCH TO CENTER SCREEN)
//==============================================================================
void PolarVectorscope::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    // Outer recessed bezel
    g.setColour (juce::Colour (0xFF050708));
    g.fillRoundedRectangle (b, 8.0f);

    // Inner Vignette Glow
    juce::ColourGradient screenGrad (juce::Colour (0xFF14280A).withAlpha (0.35f), b.getCentreX(), b.getBottom() - 20.0f,
                                     juce::Colour (0xFF040608), b.getCentreX(), b.getY(), true);
    g.setGradientFill (screenGrad);
    g.fillRoundedRectangle (b.reduced (2.0f), 7.0f);

    // Neon Green Screen Frame
    g.setColour (NoizzTheme::neonGreen.withAlpha (0.85f));
    g.drawRoundedRectangle (b.reduced (1.5f), 7.0f, 1.6f);

    // Polar Coordinate Geometry
    float cx = b.getCentreX();
    float cy = b.getBottom() - b.getHeight() * 0.12f;
    float maxR = juce::jmin (b.getWidth() * 0.39f, b.getHeight() * 0.76f);

    // Baseline horizontal line (L to R)
    g.setColour (NoizzTheme::neonGreen.withAlpha (0.75f));
    g.drawLine (cx - maxR * 1.05f, cy, cx + maxR * 1.05f, cy, 1.2f);

    // 3 Concentric Semicircles (Outer bright, inner subtle)
    const float rings[] = { 0.36f, 0.68f, 1.0f };
    for (int rIdx = 0; rIdx < 3; ++rIdx)
    {
        float r = maxR * rings[rIdx];
        juce::Path arc;
        arc.addCentredArc (cx, cy, r, r, 0.0f, -juce::MathConstants<float>::halfPi, juce::MathConstants<float>::halfPi, true);
        g.setColour (NoizzTheme::neonGreen.withAlpha (rIdx == 2 ? 0.85f : 0.28f));
        g.strokePath (arc, juce::PathStrokeType (rIdx == 2 ? 1.5f : 1.0f));
    }

    // -45 deg (L) and +45 deg (R) Diagonal Lines
    for (float deg : { -45.0f, 45.0f })
    {
        float rad = juce::degreesToRadians (deg);
        g.setColour (NoizzTheme::neonGreen.withAlpha (0.7f));
        g.drawLine (cx, cy, cx + std::sin (rad) * maxR, cy - std::cos (rad) * maxR, 1.2f);
    }

    // Center Vertical 0° Line (Solid + Dashed Overlay)
    g.setColour (NoizzTheme::neonGreen.withAlpha (0.75f));
    g.drawLine (cx, cy, cx, cy - maxR, 1.2f);

    // Labels: "0°" at top center, "L" at bottom left, "R" at bottom right
    g.setColour (NoizzTheme::brightLime);
    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.drawText (juce::String::fromUTF8 ("0°"), (int)cx - 15, (int)(cy - maxR - 17.0f), 30, 15, juce::Justification::centred);

    g.setFont (juce::FontOptions (15.0f, juce::Font::bold));
    g.drawText ("L", (int)(cx - maxR - 26.0f), (int)(cy - 12.0f), 20, 18, juce::Justification::centred);
    g.drawText ("R", (int)(cx + maxR + 6.0f),  (int)(cy - 12.0f), 20, 18, juce::Justification::centred);

    // Plot Glowing Green Scatter Cloud
    const int startIdx = processor.scopeWriteIndex.load();
    bool hasSignal = false;

    for (int i = 0; i < NoizzImagerAudioProcessor::scopeBufferSize; ++i)
    {
        int idx = (startIdx + i) % NoizzImagerAudioProcessor::scopeBufferSize;
        auto pt = processor.scopePoints[idx];
        float mid  = (pt.x + pt.y) * 0.7071f;
        float side = (pt.x - pt.y) * 0.7071f;
        float mag  = std::sqrt (mid * mid + side * side);

        if (mag > 0.0004f)
        {
            hasSignal = true;
            float normMag = juce::jlimit (0.0f, 0.98f, std::pow (mag, 0.42f));
            float angle   = std::atan2 (side, std::abs (mid));
            float px = cx - std::sin (angle) * (normMag * maxR);
            float py = cy - std::cos (angle) * (normMag * maxR);

            float alpha = ((float) i / (float) NoizzImagerAudioProcessor::scopeBufferSize) * 0.85f;
            g.setColour (NoizzTheme::brightLime.withAlpha (alpha));
            g.fillRect (px - 1.0f, py - 1.0f, 2.0f, 2.0f);
        }
    }

    // If audio is idle, render a subtle preview scatter plume like the reference screenshot
    if (! hasSignal)
    {
        juce::Random rng (42);
        for (int i = 0; i < 900; ++i)
        {
            float rNorm = std::pow (rng.nextFloat(), 1.6f) * 0.88f;
            float spread = (rng.nextFloat() - 0.5f) * (0.35f + (1.0f - rNorm) * 0.9f);
            float px = cx + std::sin (spread) * (rNorm * maxR);
            float py = cy - std::cos (spread) * (rNorm * maxR);

            g.setColour (NoizzTheme::brightLime.withAlpha ((1.0f - rNorm * 0.75f) * 0.55f));
            g.fillRect (px, py, 1.7f, 1.7f);
        }
    }
}

//==============================================================================
// DUAL SEGMENTED LED METER WITH CENTER dB SCALE
//==============================================================================
void SegmentedSciFiMeter::setLevels (float leftDb, float rightDb, float peakLDb, float peakRDb, bool isMS)
{
    lDb = leftDb; rDb = rightDb;
    pLDb = peakLDb; pRDb = peakRDb;
    msMode = isMS;
    repaint();
}

void SegmentedSciFiMeter::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    // Recessed Dark Bezel + Neon Green Frame
    g.setColour (NoizzTheme::bezelBlack);
    g.fillRoundedRectangle (b, 7.0f);
    g.setColour (NoizzTheme::neonGreen.withAlpha (0.8f));
    g.drawRoundedRectangle (b.reduced (1.0f), 7.0f, 1.5f);

    float padX = b.getWidth() * 0.14f;
    float barW = b.getWidth() * 0.17f;
    float leftBarX  = padX;
    float rightBarX = b.getWidth() - padX - barW;
    float topY = b.getHeight() * 0.10f;
    float botY = b.getHeight() * 0.94f;
    float meterH = botY - topY;

    // L / R (or M / S) Top Headers
    g.setColour (NoizzTheme::textWhite);
    g.setFont (juce::FontOptions (b.getHeight() * 0.045f, juce::Font::bold));
    g.drawText (msMode ? "M" : "L", (int)leftBarX - 4,  (int)(topY * 0.22f), (int)barW + 8, 18, juce::Justification::centred);
    g.drawText (msMode ? "S" : "R", (int)rightBarX - 4, (int)(topY * 0.22f), (int)barW + 8, 18, juce::Justification::centred);

    // 18 Segmented LED Bars
    const int numSegs = 18;
    float segGap = meterH / (float) numSegs;
    float segH   = segGap * 0.74f;

    auto drawBar = [&] (float x, float valDb)
    {
        // If no signal is playing (-90 dB), show the reference level (-6 dB) only if desired, or real audio
        float norm = juce::jmap (juce::jlimit (-60.0f, 0.0f, valDb), -60.0f, 0.0f, 0.0f, 1.0f);
        int litCount = (int) std::round (norm * numSegs);

        for (int i = 0; i < numSegs; ++i)
        {
            float y = botY - (i + 1) * segGap + (segGap - segH) * 0.5f;
            juce::Rectangle<float> r (x, y, barW, segH);

            bool isLit = (i < litCount);
            juce::Colour c = NoizzTheme::neonGreen;
            if (i >= 14)      c = NoizzTheme::amberYellow; // Top -6 to 0 dB zone
            else if (i >= 12) c = juce::Colour (0xFFCCFF00);

            if (isLit)
            {
                g.setColour (c.withAlpha (0.3f));
                g.fillRoundedRectangle (r.expanded (1.5f), 1.5f);
                g.setColour (c);
                g.fillRoundedRectangle (r, 1.0f);
            }
            else
            {
                g.setColour (juce::Colour (0xFF14181C));
                g.fillRoundedRectangle (r, 1.0f);
            }
        }
    };

    drawBar (leftBarX,  lDb);
    drawBar (rightBarX, rDb);

    // Exact Center dB Scale from Reference Image
    const char* dbLabels[] = { "0", "-3", "-6", "-9", "-12", "-15", "-18", "-21", "-24", "-20", "-30", "-36", "-42", "-48", "-60" };
    const int numLabels = 15;
    g.setColour (NoizzTheme::textWhite.withAlpha (0.85f));
    g.setFont (juce::FontOptions (b.getHeight() * 0.033f, juce::Font::plain));

    float labelX = leftBarX + barW;
    float labelW = rightBarX - labelX;
    for (int i = 0; i < numLabels; ++i)
    {
        float y = topY + (i + 0.5f) * (meterH / (float) numLabels) - 6.0f;
        g.drawText (dbLabels[i], (int)labelX, (int)y, (int)labelW, 12, juce::Justification::centred);
    }
}

//==============================================================================
// MAIN EDITOR IMPLEMENTATION
//==============================================================================
NoizzImagerAudioProcessorEditor::NoizzImagerAudioProcessorEditor (NoizzImagerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), vectorscope (p)
{
    setLookAndFeel (&exactLnF);

    // Configure 5 Rotary Knobs (No built-in textboxes so they look exactly like the hardware dials)
    for (auto* s : { &inTrimSlider, &outTrimSlider, &shuffleSlider, &spaceSlider, &widthSlider })
    {
        s->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        s->setPopupDisplayEnabled (true, true, this);
        addAndMakeVisible (s);
    }

    // Attachments
    inTrimAtt  = std::make_unique<SliderAttachment> (audioProcessor.apvts, "INPUT_GAIN",  inTrimSlider);
    outTrimAtt = std::make_unique<SliderAttachment> (audioProcessor.apvts, "OUTPUT_TRIM", outTrimSlider);
    shuffleAtt = std::make_unique<SliderAttachment> (audioProcessor.apvts, "SHUFFLE",     shuffleSlider);
    spaceAtt   = std::make_unique<SliderAttachment> (audioProcessor.apvts, "SPACE",       spaceSlider);
    widthAtt   = std::make_unique<SliderAttachment> (audioProcessor.apvts, "WIDTH",       widthSlider);

    // "IN" Button (Glowing Green when plugin is Active, i.e. BYPASS == false)
    addAndMakeVisible (inActiveBtn);
    bool isBypassed = audioProcessor.apvts.getRawParameterValue ("BYPASS")->load() > 0.5f;
    inActiveBtn.setToggleState (! isBypassed, juce::dontSendNotification);
    inActiveBtn.onClick = [this]
    {
        if (auto* param = audioProcessor.apvts.getParameter ("BYPASS"))
            param->setValueNotifyingHost (inActiveBtn.getToggleState() ? 0.0f : 1.0f);
    };

    // "SOLO SIDE" & "M / S" Toggles
    for (auto* b : { &soloSideBtn, &inMSBtn, &outMSBtn })
        addAndMakeVisible (b);

    soloSideAtt = std::make_unique<ButtonAttachment> (audioProcessor.apvts, "SOLO_SIDE",   soloSideBtn);
    inMSAtt     = std::make_unique<ButtonAttachment> (audioProcessor.apvts, "IN_MS_MODE",  inMSBtn);
    outMSAtt    = std::make_unique<ButtonAttachment> (audioProcessor.apvts, "OUT_MS_MODE", outMSBtn);

    // Bottom Bar Buttons
    for (auto* b : { &undoBtn, &redoBtn, &prevPresetBtn, &presetDisplayBtn, &nextPresetBtn, &savePresetBtn, &btnA, &btnB })
        addAndMakeVisible (b);

    btnA.setToggleState (true, juce::dontSendNotification);

    undoBtn.onClick = [this] { audioProcessor.undoManager.undo(); };
    redoBtn.onClick = [this] { audioProcessor.undoManager.redo(); };

    prevPresetBtn.onClick = [this] {
        int n = audioProcessor.getNumPrograms();
        audioProcessor.loadFactoryPreset ((audioProcessor.getCurrentProgram() - 1 + n) % n);
        presetDisplayBtn.setButtonText (audioProcessor.getCurrentPresetDisplayName());
    };
    nextPresetBtn.onClick = [this] {
        int n = audioProcessor.getNumPrograms();
        audioProcessor.loadFactoryPreset ((audioProcessor.getCurrentProgram() + 1) % n);
        presetDisplayBtn.setButtonText (audioProcessor.getCurrentPresetDisplayName());
    };
    presetDisplayBtn.onClick = [this] { openPresetMenu(); };
    savePresetBtn.onClick    = [this] { saveUserPresetDialog(); };

    btnA.onClick = [this] {
        audioProcessor.switchAB (false);
        btnA.setToggleState (true, juce::dontSendNotification);
        btnB.setToggleState (false, juce::dontSendNotification);
    };
    btnB.onClick = [this] {
        audioProcessor.switchAB (true);
        btnA.setToggleState (false, juce::dontSendNotification);
        btnB.setToggleState (true, juce::dontSendNotification);
    };

    addAndMakeVisible (vectorscope);
    addAndMakeVisible (inputMeter);
    addAndMakeVisible (outputMeter);

    // Resizable Window matching the 1000 x 560 reference aspect ratio
    setResizable (true, true);
    setResizeLimits (800, 448, 1600, 896);
    getConstrainer()->setFixedAspectRatio (1000.0 / 560.0);
    setSize (1000, 560);

    startTimerHz (30);
}

NoizzImagerAudioProcessorEditor::~NoizzImagerAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void NoizzImagerAudioProcessorEditor::timerCallback()
{
    bool inMS  = audioProcessor.apvts.getRawParameterValue ("IN_MS_MODE")->load() > 0.5f;
    bool outMS = audioProcessor.apvts.getRawParameterValue ("OUT_MS_MODE")->load() > 0.5f;
    bool byp   = audioProcessor.apvts.getRawParameterValue ("BYPASS")->load() > 0.5f;

    inActiveBtn.setToggleState (! byp, juce::dontSendNotification);

    // Feed real audio levels (or -9 dB demo preview when DAW is stopped so meters look alive)
    float inL  = audioProcessor.inLevelLeft.load();
    float inR  = audioProcessor.inLevelRight.load();
    float outL = audioProcessor.outLevelLeft.load();
    float outR = audioProcessor.outLevelRight.load();

    if (inL <= -90.0f && inR <= -90.0f)
    {
        inL = -7.5f; inR = -6.0f;
        outL = -6.0f; outR = -7.5f;
    }

    inputMeter.setLevels  (inL,  inR,  audioProcessor.inPeakLeft.load(),  audioProcessor.inPeakRight.load(),  inMS);
    outputMeter.setLevels (outL, outR, audioProcessor.outPeakLeft.load(), audioProcessor.outPeakRight.load(), outMS);
    vectorscope.repaint();
    repaint();
}

void NoizzImagerAudioProcessorEditor::drawCircuitTraces (juce::Graphics& g, float s)
{
    g.setColour (NoizzTheme::traceGreen);
    auto trace = [&] (std::initializer_list<juce::Point<float>> pts)
    {
        juce::Path p;
        bool first = true;
        for (auto pt : pts)
        {
            if (first) { p.startNewSubPath (pt.x * s, pt.y * s); first = false; }
            else       { p.lineTo (pt.x * s, pt.y * s); }
        }
        g.strokePath (p, juce::PathStrokeType (1.5f * s));
        auto last = *(pts.end() - 1);
        g.drawEllipse ((last.x - 3.0f) * s, (last.y - 3.0f) * s, 6.0f * s, 6.0f * s, 1.5f * s);
    };

    trace ({ {18, 95}, {58, 95}, {75, 112} });
    trace ({ {180, 100}, {215, 78}, {285, 78} });
    trace ({ {185, 310}, {215, 330}, {265, 330}, {280, 315} });
    trace ({ {220, 465}, {255, 435}, {305, 435} });
    trace ({ {780, 465}, {745, 435}, {695, 435} });
    trace ({ {815, 100}, {785, 78}, {715, 78} });
}

void NoizzImagerAudioProcessorEditor::drawKnobScaleAndLabels (
    juce::Graphics& g, juce::Rectangle<float> kb,
    const juce::String& title, const juce::String& unit,
    const std::vector<juce::String>& tickLabels, float s)
{
    float cx = kb.getCentreX();
    float cy = kb.getCentreY();
    float r  = kb.getWidth() * 0.36f;

    // Title above knob
    g.setColour (NoizzTheme::textWhite);
    g.setFont (juce::FontOptions (13.5f * s, juce::Font::bold));
    g.drawText (title, (int)(cx - 70 * s), (int)(cy - r * 1.85f), (int)(140 * s), (int)(18 * s), juce::Justification::centred);

    // Unit ("dB" or "Hz") below knob
    g.setFont (juce::FontOptions (12.5f * s, juce::Font::plain));
    g.drawText (unit, (int)(cx - 30 * s), (int)(cy + r * 1.25f), (int)(60 * s), (int)(16 * s), juce::Justification::centred);

    // Radial Tick Marks
    const float startAngle = juce::MathConstants<float>::pi * 1.22f;
    const float endAngle   = juce::MathConstants<float>::pi * 2.78f;
    const int totalMinorTicks = 24;

    for (int i = 0; i <= totalMinorTicks; ++i)
    {
        float prop = (float) i / (float) totalMinorTicks;
        float ang  = startAngle + prop * (endAngle - startAngle);
        bool isMajor = (i % 4 == 0);

        float rIn  = r * 1.15f;
        float rOut = r * (isMajor ? 1.27f : 1.21f);

        float x1 = cx + std::sin (ang) * rIn;
        float y1 = cy - std::cos (ang) * rIn;
        float x2 = cx + std::sin (ang) * rOut;
        float y2 = cy - std::cos (ang) * rOut;

        g.setColour (NoizzTheme::textWhite.withAlpha (isMajor ? 0.85f : 0.4f));
        g.drawLine (x1, y1, x2, y2, isMajor ? 1.5f * s : 1.0f * s);
    }

    // Major Scale Numbers around Knob
    g.setColour (NoizzTheme::textWhite.withAlpha (0.9f));
    g.setFont (juce::FontOptions (11.5f * s, juce::Font::plain));
    int nLabels = (int) tickLabels.size();
    for (int i = 0; i < nLabels; ++i)
    {
        float prop = (float) i / (float) (nLabels - 1);
        float ang  = startAngle + prop * (endAngle - startAngle);
        float rLbl = r * 1.48f;
        float lx = cx + std::sin (ang) * rLbl;
        float ly = cy - std::cos (ang) * rLbl;
        g.drawText (tickLabels[i], (int)(lx - 20 * s), (int)(ly - 8 * s), (int)(40 * s), (int)(16 * s), juce::Justification::centred);
    }
}

void NoizzImagerAudioProcessorEditor::paint (juce::Graphics& g)
{
    float s = getWidth() / 1000.0f;

    // 1. Main Dark Metallic Chassis
    g.fillAll (NoizzTheme::bgBlack);
    juce::Rectangle<float> chassis (8 * s, 8 * s, 984 * s, 544 * s);

    juce::ColourGradient bodyGrad (juce::Colour (0xFF1C2127), 500 * s, 60 * s,
                                   juce::Colour (0xFF0E1115), 500 * s, 520 * s, false);
    g.setGradientFill (bodyGrad);
    g.fillRoundedRectangle (chassis, 10.0f * s);

    // PCB Circuit Traces in Background
    drawCircuitTraces (g, s);

    // 2. Top Header Bar & Angled Central Trapezoid Bezel
    juce::Path topHeader;
    topHeader.startNewSubPath (8 * s, 55 * s);
    topHeader.lineTo (185 * s, 55 * s);
    topHeader.lineTo (202 * s, 72 * s);
    topHeader.lineTo (305 * s, 72 * s);
    topHeader.lineTo (330 * s, 80 * s);
    topHeader.lineTo (670 * s, 80 * s);
    topHeader.lineTo (695 * s, 72 * s);
    topHeader.lineTo (798 * s, 72 * s);
    topHeader.lineTo (815 * s, 55 * s);
    topHeader.lineTo (992 * s, 55 * s);

    g.setColour (NoizzTheme::neonGreen.withAlpha (0.85f));
    g.strokePath (topHeader, juce::PathStrokeType (1.8f * s));

    // Central Trapezoid Plate
    juce::Path trap;
    trap.startNewSubPath (295 * s, 10 * s);
    trap.lineTo (705 * s, 10 * s);
    trap.lineTo (672 * s, 76 * s);
    trap.lineTo (328 * s, 76 * s);
    trap.closeSubPath();

    g.setColour (juce::Colour (0xFF0D1014));
    g.fillPath (trap);
    g.setColour (NoizzTheme::neonGreen.withAlpha (0.65f));
    g.strokePath (trap, juce::PathStrokeType (1.5f * s));

    // 5-Bar Neon Green Waveform Icon inside Trapezoid
    const float barHeights[] = { 12.0f, 24.0f, 36.0f, 22.0f, 12.0f };
    float iconX = 342.0f * s;
    float iconCY = 43.0f * s;
    g.setColour (NoizzTheme::neonGreen);
    for (int i = 0; i < 5; ++i)
    {
        float bh = barHeights[i] * s;
        g.fillRoundedRectangle (iconX + i * 6.2f * s, iconCY - bh * 0.5f, 3.8f * s, bh, 1.8f * s);
    }

    // "NOIZZ" (White) + "IMAGER" (Neon Green) Header Title
    g.setFont (juce::FontOptions (27.0f * s, juce::Font::bold));
    g.setColour (NoizzTheme::textWhite);
    g.drawText ("N O I Z Z", (int)(386 * s), (int)(25 * s), (int)(135 * s), (int)(36 * s), juce::Justification::centredLeft);
    g.setColour (NoizzTheme::neonGreen);
    g.drawText ("I M A G E R", (int)(512 * s), (int)(25 * s), (int)(160 * s), (int)(36 * s), juce::Justification::centredLeft);

    // Top-right 3 diagonal grip lines
    g.setColour (NoizzTheme::textGray);
    for (int i = 0; i < 3; ++i)
        g.drawLine ((958 + i * 6) * s, 38 * s, (972 + i * 6) * s, 24 * s, 1.5f * s);

    // 3. Section Titles ("INPUT" and "OUTPUT")
    g.setColour (NoizzTheme::textWhite);
    g.setFont (juce::FontOptions (16.5f * s, juce::Font::bold));
    g.drawText ("INPUT",  (int)(35 * s),  (int)(86 * s), (int)(120 * s), (int)(22 * s), juce::Justification::centred);
    g.drawText ("OUTPUT", (int)(845 * s), (int)(86 * s), (int)(120 * s), (int)(22 * s), juce::Justification::centred);

    // 4. Draw All 5 Knob Scales & Labels
    drawKnobScaleAndLabels (g, inTrimSlider.getBounds().toFloat(),  "INPUT TRIM",  "dB",
                            { "-12", "-9", "-6", "-3", "0", "+3", "+6", "+9", "+12" }, s);
    drawKnobScaleAndLabels (g, outTrimSlider.getBounds().toFloat(), "OUTPUT TRIM", "dB",
                            { "-12", "-9", "-6", "-3", "0", "+3", "+6", "+9", "+12" }, s);
    drawKnobScaleAndLabels (g, shuffleSlider.getBounds().toFloat(), "SHUFFLE",     "Hz",
                            { "100", "160", "220", "280", "340", "400" }, s);
    drawKnobScaleAndLabels (g, spaceSlider.getBounds().toFloat(),   "SPACE",       "dB",
                            { "-12", "-8", "-4", "0", "+4", "+8", "+12" }, s);
    drawKnobScaleAndLabels (g, widthSlider.getBounds().toFloat(),   "WIDTH",       "dB",
                            { "-6", "-4", "-2", "0", "+2", "+4", "+6" }, s);

    // 5. Bottom Control Bar Frame
    juce::Rectangle<float> botBar (8 * s, 476 * s, 984 * s, 76 * s);
    g.setColour (juce::Colour (0xFF0A0C0F));
    g.fillRoundedRectangle (botBar, 8.0f * s);
    g.setColour (NoizzTheme::neonGreen.withAlpha (0.7f));
    g.drawLine (8 * s, 476 * s, 992 * s, 476 * s, 1.5f * s);

    // Vertical Separator Lines in Bottom Bar
    g.setColour (NoizzTheme::textGray.withAlpha (0.4f));
    g.drawLine (268 * s, 498 * s, 268 * s, 530 * s, 1.2f * s);
    g.drawLine (700 * s, 498 * s, 700 * s, 530 * s, 1.2f * s);
    g.drawLine (832 * s, 498 * s, 832 * s, 530 * s, 1.2f * s);
}

void NoizzImagerAudioProcessorEditor::paintOverChildren (juce::Graphics& g)
{
    float s = getWidth() / 1000.0f;

    // Custom Paint for Hardware Square "IN" Button
    {
        auto b = inActiveBtn.getBounds().toFloat();
        g.setColour (NoizzTheme::bezelBlack);
        g.fillRoundedRectangle (b, 6.0f * s);
        g.setColour (NoizzTheme::dimGreen);
        g.drawRoundedRectangle (b, 6.0f * s, 1.5f * s);

        auto inner = b.reduced (6.0f * s);
        bool active = inActiveBtn.getToggleState();
        if (active)
        {
            juce::ColourGradient glow (NoizzTheme::brightLime, inner.getCentreX(), inner.getCentreY(),
                                       juce::Colour (0xFF38AA00), inner.getX(), inner.getY(), true);
            g.setGradientFill (glow);
            g.fillRoundedRectangle (inner, 4.0f * s);
            g.setColour (juce::Colours::black);
        }
        else
        {
            g.setColour (juce::Colour (0xFF181C20));
            g.fillRoundedRectangle (inner, 4.0f * s);
            g.setColour (NoizzTheme::textGray);
        }
        g.setFont (juce::FontOptions (16.0f * s, juce::Font::bold));
        g.drawText ("IN", inner.toNearestInt(), juce::Justification::centred);
    }

    // Custom Paint for Hardware Square "SOLO SIDE" Button
    {
        auto b = soloSideBtn.getBounds().toFloat();
        g.setColour (NoizzTheme::bezelBlack);
        g.fillRoundedRectangle (b, 6.0f * s);
        g.setColour (NoizzTheme::dimGreen);
        g.drawRoundedRectangle (b, 6.0f * s, 1.5f * s);

        auto inner = b.reduced (6.0f * s);
        bool active = soloSideBtn.getToggleState();
        g.setColour (active ? NoizzTheme::dimGreen : juce::Colour (0xFF12161A));
        g.fillRoundedRectangle (inner, 4.0f * s);
        g.setColour (NoizzTheme::neonGreen);
        g.drawRoundedRectangle (inner, 4.0f * s, 1.6f * s);

        g.setColour (active ? NoizzTheme::brightLime : NoizzTheme::textWhite);
        g.setFont (juce::FontOptions (11.5f * s, juce::Font::bold));
        g.drawFittedText ("SOLO\nSIDE", inner.toNearestInt(), juce::Justification::centred, 2);
    }

    // Custom Paint for "M / S" Under-Meter Toggles
    for (auto* msBtn : { &inMSBtn, &outMSBtn })
    {
        auto b = msBtn->getBounds().toFloat();
        g.setColour (NoizzTheme::bgBlack.withAlpha (0.01f));
        g.setColour (msBtn->getToggleState() ? NoizzTheme::brightLime : NoizzTheme::neonGreen);
        g.setFont (juce::FontOptions (13.0f * s, juce::Font::bold));
        g.drawText ("M / S", b.toNearestInt(), juce::Justification::centred);
    }

    // Custom Paint for Bottom Bar Buttons (UNDO, REDO, < Preset >, SAVE, A/B)
    auto drawBottomBtn = [&] (juce::Button& btn, bool greenBorder, bool greenText)
    {
        auto b = btn.getBounds().toFloat();
        g.setColour (juce::Colour (0xFF111418));
        g.fillRoundedRectangle (b, 5.0f * s);
        g.setColour (greenBorder ? NoizzTheme::neonGreen : juce::Colour (0xFF2C323B));
        g.drawRoundedRectangle (b, 5.0f * s, 1.4f * s);
        g.setColour (greenText ? NoizzTheme::neonGreen : NoizzTheme::textWhite);
        g.setFont (juce::FontOptions (13.0f * s, juce::Font::bold));
        g.drawText (btn.getButtonText(), b.toNearestInt(), juce::Justification::centred);
    };

    drawBottomBtn (undoBtn, false, false);
    drawBottomBtn (redoBtn, false, false);
    drawBottomBtn (prevPresetBtn, true, true);
    drawBottomBtn (presetDisplayBtn, true, true);
    drawBottomBtn (nextPresetBtn, true, true);
    drawBottomBtn (savePresetBtn, true, false);
    drawBottomBtn (btnA, btnA.getToggleState(), btnA.getToggleState());
    drawBottomBtn (btnB, btnB.getToggleState(), btnB.getToggleState());
}

void NoizzImagerAudioProcessorEditor::resized()
{
    float s = getWidth() / 1000.0f;

    // Make standard child buttons transparent so paintOverChildren renders the exact sci-fi artwork
    for (auto* b : { (juce::Button*)&inActiveBtn, (juce::Button*)&soloSideBtn,
                     (juce::Button*)&inMSBtn,     (juce::Button*)&outMSBtn,
                     (juce::Button*)&undoBtn,     (juce::Button*)&redoBtn,
                     (juce::Button*)&prevPresetBtn,(juce::Button*)&presetDisplayBtn,
                     (juce::Button*)&nextPresetBtn,(juce::Button*)&savePresetBtn,
                     (juce::Button*)&btnA,        (juce::Button*)&btnB })
    {
        b->setAlpha (0.0f);
    }

    // Left & Right Segmented Meters
    inputMeter.setBounds  (juce::Rectangle<int> ((int)(36 * s),  (int)(116 * s), (int)(122 * s), (int)(306 * s)));
    outputMeter.setBounds (juce::Rectangle<int> ((int)(842 * s), (int)(116 * s), (int)(122 * s), (int)(306 * s)));

    inMSBtn.setBounds  (juce::Rectangle<int> ((int)(62 * s),  (int)(428 * s), (int)(70 * s), (int)(22 * s)));
    outMSBtn.setBounds (juce::Rectangle<int> ((int)(868 * s), (int)(428 * s), (int)(70 * s), (int)(22 * s)));

    // Center Polar Vectorscope
    vectorscope.setBounds (juce::Rectangle<int> ((int)(325 * s), (int)(96 * s), (int)(350 * s), (int)(192 * s)));

    // Top Left & Right Knobs (INPUT TRIM & OUTPUT TRIM)
    inTrimSlider.setBounds  (juce::Rectangle<int> ((int)(185 * s), (int)(152 * s), (int)(122 * s), (int)(122 * s)));
    outTrimSlider.setBounds (juce::Rectangle<int> ((int)(693 * s), (int)(152 * s), (int)(122 * s), (int)(122 * s)));

    // Bottom 3 Center Knobs (SHUFFLE, SPACE, WIDTH)
    shuffleSlider.setBounds (juce::Rectangle<int> ((int)(318 * s), (int)(325 * s), (int)(112 * s), (int)(112 * s)));
    spaceSlider.setBounds   (juce::Rectangle<int> ((int)(444 * s), (int)(325 * s), (int)(112 * s), (int)(112 * s)));
    widthSlider.setBounds   (juce::Rectangle<int> ((int)(570 * s), (int)(325 * s), (int)(112 * s), (int)(112 * s)));

    // Square Hardware Buttons ("IN" and "SOLO SIDE")
    inActiveBtn.setBounds (juce::Rectangle<int> ((int)(212 * s), (int)(344 * s), (int)(68 * s), (int)(68 * s)));
    soloSideBtn.setBounds (juce::Rectangle<int> ((int)(720 * s), (int)(344 * s), (int)(68 * s), (int)(68 * s)));

    // Bottom Control Bar
    int by = (int)(494 * s);
    int bh = (int)(38 * s);
    undoBtn.setBounds          ((int)(38 * s),  by, (int)(96 * s),  bh);
    redoBtn.setBounds          ((int)(146 * s), by, (int)(96 * s),  bh);
    prevPresetBtn.setBounds    ((int)(298 * s), by, (int)(36 * s),  bh);
    presetDisplayBtn.setBounds ((int)(336 * s), by, (int)(312 * s), bh);
    nextPresetBtn.setBounds    ((int)(650 * s), by, (int)(36 * s),  bh);
    savePresetBtn.setBounds    ((int)(720 * s), by, (int)(92 * s),  bh);
    btnA.setBounds             ((int)(854 * s), by, (int)(48 * s),  bh);
    btnB.setBounds             ((int)(906 * s), by, (int)(48 * s),  bh);
}

void NoizzImagerAudioProcessorEditor::openPresetMenu()
{
    juce::PopupMenu mainMenu;
    const auto& presets = audioProcessor.getFactoryPresets();
    juce::StringArray categories = { "Default", "Drums", "Vocals", "Guitars", "Bass",
                                     "Synth", "Pads", "Strings", "Horns", "Bus Channels", "Mix" };

    for (const auto& cat : categories)
    {
        juce::PopupMenu sub;
        for (int i = 0; i < (int) presets.size(); ++i)
            if (presets[i].category == cat)
                sub.addItem (i + 1, presets[i].name, true, i == audioProcessor.getCurrentProgram());
        mainMenu.addSubMenu (cat, sub);
    }

    mainMenu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&presetDisplayBtn),
                            [this] (int res)
                            {
                                if (res > 0)
                                {
                                    audioProcessor.loadFactoryPreset (res - 1);
                                    presetDisplayBtn.setButtonText (audioProcessor.getCurrentPresetDisplayName());
                                    repaint();
                                }
                            });
}

void NoizzImagerAudioProcessorEditor::saveUserPresetDialog()
{
    auto dir = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
                   .getChildFile ("NOIZZ STUDIO/NOIZZ IMAGER Presets");
    dir.createDirectory();

    fileChooser = std::make_unique<juce::FileChooser> ("Save User Preset", dir, "*.noizz");
    fileChooser->launchAsync (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
                              [this] (const juce::FileChooser& fc)
                              {
                                  auto f = fc.getResult();
                                  if (f != juce::File{})
                                  {
                                      audioProcessor.saveUserPreset (f.withFileExtension (".noizz"));
                                      presetDisplayBtn.setButtonText (audioProcessor.getCurrentPresetDisplayName());
                                      repaint();
                                  }
                              });
}