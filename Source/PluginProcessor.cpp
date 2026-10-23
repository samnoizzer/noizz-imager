#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace SciFiColors
{
    const juce::Colour pureBlack   { 0xFF07080A };
    const juce::Colour panelGray   { 0xFF12151A };
    const juce::Colour darkMetal   { 0xFF1B2028 };
    const juce::Colour neonGreen   { 0xFF00FF66 };
    const juce::Colour dimGreen    { 0xFF005522 };
    const juce::Colour cyberCyan   { 0xFF00FFAA };
    const juce::Colour alertAmber  { 0xFFFFB000 };
    const juce::Colour clipRed     { 0xFFFF2244 };
}

//==============================================================================
// LOOK AND FEEL
//==============================================================================
NoizzSciFiLookAndFeel::NoizzSciFiLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId,       SciFiColors::neonGreen);
    setColour (juce::Slider::textBoxBackgroundColourId, SciFiColors::pureBlack);
    setColour (juce::Slider::textBoxOutlineColourId,    SciFiColors::dimGreen);
    setColour (juce::Label::textColourId,               juce::Colours::white);
    setColour (juce::PopupMenu::backgroundColourId,     SciFiColors::panelGray);
    setColour (juce::PopupMenu::textColourId,           SciFiColors::neonGreen);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, SciFiColors::dimGreen);
}

void NoizzSciFiLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, const float rotaryStartAngle,
                                              const float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float> ((float)x, (float)y, (float)width, (float)height).reduced (8.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Outer Sci-Fi Segmented Tick Ring
    const int numTicks = 25;
    for (int i = 0; i < numTicks; ++i)
    {
        float tickProp = (float) i / (float) (numTicks - 1);
        float tickAngle = rotaryStartAngle + tickProp * (rotaryEndAngle - rotaryStartAngle);
        bool active = (tickProp <= sliderPos);

        g.setColour (active ? SciFiColors::neonGreen : SciFiColors::darkMetal.brighter (0.15f));
        juce::Path tick;
        tick.addRectangle (-1.2f, -radius * 1.02f, 2.4f, radius * 0.1f);
        g.fillPath (tick, juce::AffineTransform::rotation (tickAngle).translated (centreX, centreY));
    }

    // Background Arc
    juce::Path bgArc;
    bgArc.addCentredArc (centreX, centreY, radius * 0.84f, radius * 0.84f, 0.0f,
                         rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (SciFiColors::pureBlack);
    g.strokePath (bgArc, juce::PathStrokeType (4.5f));

    // Active Neon Value Arc
    juce::Path valArc;
    valArc.addCentredArc (centreX, centreY, radius * 0.84f, radius * 0.84f, 0.0f,
                          rotaryStartAngle, angle, true);
    g.setColour (SciFiColors::neonGreen);
    g.strokePath (valArc, juce::PathStrokeType (3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Dark Metallic Inner Rotor Body
    auto coreRadius = radius * 0.68f;
    juce::ColourGradient grad (SciFiColors::darkMetal.brighter (0.15f), centreX, centreY - coreRadius,
                               SciFiColors::pureBlack, centreX, centreY + coreRadius, false);
    g.setGradientFill (grad);
    g.fillEllipse (centreX - coreRadius, centreY - coreRadius, coreRadius * 2.0f, coreRadius * 2.0f);

    g.setColour (SciFiColors::dimGreen.brighter (0.3f));
    g.drawEllipse (centreX - coreRadius, centreY - coreRadius, coreRadius * 2.0f, coreRadius * 2.0f, 1.5f);

    // Neon Indicator Needle
    juce::Path pointer;
    pointer.addRoundedRectangle (-2.0f, -coreRadius * 0.9f, 4.0f, coreRadius * 0.55f, 1.5f);
    g.setColour (SciFiColors::neonGreen);
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (centreX, centreY));
}

void NoizzSciFiLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                                  const juce::Colour&, bool highlighted, bool down)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (1.5f);
    bool toggled = button.getToggleState();

    g.setColour (toggled ? SciFiColors::dimGreen : SciFiColors::pureBlack);
    g.fillRoundedRectangle (bounds, 3.0f);

    g.setColour ((toggled || highlighted || down) ? SciFiColors::neonGreen : SciFiColors::darkMetal.brighter (0.3f));
    g.drawRoundedRectangle (bounds, 3.0f, toggled ? 1.8f : 1.0f);
}

//==============================================================================
// POLAR SAMPLE PLOT VECTORSCOPE
//==============================================================================
void PolarVectorscope::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat().reduced (4.0f);

    // Sci-Fi Bezel & Background
    g.setColour (SciFiColors::pureBlack);
    g.fillRoundedRectangle (b, 6.0f);
    g.setColour (SciFiColors::dimGreen);
    g.drawRoundedRectangle (b, 6.0f, 1.5f);

    auto domeArea = b.reduced (12.0f, 22.0f);
    float cx = domeArea.getCentreX();
    float cy = domeArea.getBottom() - 18.0f;
    float radius = juce::jmin (domeArea.getWidth() * 0.48f, domeArea.getHeight() * 0.88f);

    // Polar Semi-Circle Grid (SSL Fusion Polar Sample Plot style)
    g.setColour (SciFiColors::dimGreen.withAlpha (0.5f));
    for (float rMult : { 0.33f, 0.66f, 1.0f })
    {
        float r = radius * rMult;
        juce::Path arc;
        arc.addCentredArc (cx, cy, r, r, 0.0f, -juce::MathConstants<float>::halfPi, juce::MathConstants<float>::halfPi, true);
        g.strokePath (arc, juce::PathStrokeType (1.0f));
    }

    // Center M axis (0 deg) and +45/-45 L/R Phase Boundaries
    for (float deg : { -45.0f, 0.0f, 45.0f, -90.0f, 90.0f })
    {
        float rad = juce::degreesToRadians (deg);
        float ex = cx + std::sin (rad) * radius;
        float ey = cy - std::cos (rad) * radius;
        g.drawLine (cx, cy, ex, ey, (std::abs (deg) == 45.0f) ? 1.5f : 1.0f);
    }

    // Labels
    g.setColour (SciFiColors::neonGreen);
    g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    g.drawText ("M", (int)cx - 10, (int)(cy - radius - 16), 20, 14, juce::Justification::centred);
    g.drawText ("L", (int)(cx - radius * 0.78f) - 14, (int)(cy - radius * 0.78f) - 10, 20, 14, juce::Justification::centred);
    g.drawText ("R", (int)(cx + radius * 0.78f) - 6,  (int)(cy - radius * 0.78f) - 10, 20, 14, juce::Justification::centred);
    g.drawText ("+S", (int)(cx - radius - 22), (int)cy - 10, 22, 14, juce::Justification::centred);
    g.drawText ("-S", (int)(cx + radius + 2),  (int)cy - 10, 22, 14, juce::Justification::centred);

    // Plot Real-Time Polar Samples
    const int startIdx = processor.scopeWriteIndex.load();
    for (int i = 0; i < NoizzImagerAudioProcessor::scopeBufferSize; ++i)
    {
        int idx = (startIdx + i) % NoizzImagerAudioProcessor::scopeBufferSize;
        auto pt = processor.scopePoints[idx];
        float L = pt.x;
        float R = pt.y;

        float mid  = (L + R) * 0.7071f;
        float side = (L - R) * 0.7071f;
        float mag  = std::sqrt (mid * mid + side * side);

        if (mag > 0.0005f)
        {
            // Compress amplitude logarithmically so quiet signals still show clearly
            float normMag = juce::jlimit (0.0f, 1.0f, std::pow (mag, 0.45f));
            float angle = std::atan2 (side, std::abs (mid)); // Polar upper hemisphere plot

            float px = cx - std::sin (angle) * (normMag * radius);
            float py = cy - std::cos (angle) * (normMag * radius);

            float ageAlpha = (float) i / (float) NoizzImagerAudioProcessor::scopeBufferSize;
            g.setColour (SciFiColors::neonGreen.withAlpha (ageAlpha * 0.85f));
            g.fillRect (px - 1.0f, py - 1.0f, 2.2f, 2.2f);
        }
    }

    // Bottom Phase Correlation Bar (-1 to +1)
    auto corrArea = juce::Rectangle<float> (b.getX() + 18.0f, b.getBottom() - 16.0f, b.getWidth() - 36.0f, 8.0f);
    g.setColour (SciFiColors::darkMetal);
    g.fillRoundedRectangle (corrArea, 2.0f);

    float corr = processor.phaseCorrelation.load();
    float midX = corrArea.getCentreX();
    float posX = midX + corr * (corrArea.getWidth() * 0.5f);

    g.setColour (corr >= 0.0f ? SciFiColors::neonGreen : SciFiColors::clipRed);
    g.fillRect (juce::jmin (midX, posX), corrArea.getY(), std::abs (posX - midX), corrArea.getHeight());
}

//==============================================================================
// SEGMENTED SCI-FI METER (WITH 3-SECOND PEAK HOLD)
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
    auto b = getLocalBounds().toFloat().reduced (2.0f);
    g.setColour (SciFiColors::pureBlack);
    g.fillRoundedRectangle (b, 4.0f);
    g.setColour (SciFiColors::dimGreen);
    g.drawRoundedRectangle (b, 4.0f, 1.2f);

    const int numSegments = 30;
    auto meterArea = b.reduced (6.0f, 20.0f);
    float barW = (meterArea.getWidth() - 6.0f) * 0.5f;
    float segH = meterArea.getHeight() / (float) numSegments;

    auto drawBar = [&] (float x, float currentDb, float peakHoldDb)
    {
        float normLvl  = juce::jmap (juce::jlimit (-60.0f, 6.0f, currentDb),  -60.0f, 6.0f, 0.0f, 1.0f);
        float normPeak = juce::jmap (juce::jlimit (-60.0f, 6.0f, peakHoldDb), -60.0f, 6.0f, 0.0f, 1.0f);
        int activeSegs = (int) std::round (normLvl * numSegments);
        int peakSeg    = (int) std::round (normPeak * numSegments) - 1;

        for (int s = 0; s < numSegments; ++s)
        {
            float y = meterArea.getBottom() - (s + 1) * segH;
            juce::Rectangle<float> segRect (x, y + 1.0f, barW, segH - 1.8f);

            juce::Colour segCol = SciFiColors::neonGreen;
            if (s > numSegments * 0.88f)      segCol = SciFiColors::clipRed;
            else if (s > numSegments * 0.72f) segCol = SciFiColors::alertAmber;

            if (s < activeSegs || s == peakSeg)
                g.setColour (segCol);
            else
                g.setColour (SciFiColors::darkMetal.darker (0.4f));

            g.fillRect (segRect);
        }
    };

    drawBar (meterArea.getX(), lDb, pLDb);
    drawBar (meterArea.getX() + barW + 6.0f, rDb, pRDb);

    // Channel Labels (L/R or M/S)
    g.setColour (SciFiColors::neonGreen);
    g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    g.drawText (msMode ? "M" : "L", (int)meterArea.getX(), (int)b.getBottom() - 16, (int)barW, 14, juce::Justification::centred);
    g.drawText (msMode ? "S" : "R", (int)(meterArea.getX() + barW + 6.0f), (int)b.getBottom() - 16, (int)barW, 14, juce::Justification::centred);
}

//==============================================================================
// MAIN EDITOR
//==============================================================================
NoizzImagerAudioProcessorEditor::NoizzImagerAudioProcessorEditor (NoizzImagerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), vectorscope (p)
{
    setLookAndFeel (&sciFiLnF);

    // Load BinaryData::logo_png if user added logo.png to Projucer
   #if JucePlugin_Build_VST3 || JucePlugin_Build_Standalone
    // Safe fallback check: If you add logo.png in Projucer, uncomment the line below:
    // logoImage = juce::ImageCache::getFromMemory (BinaryData::logo_png, BinaryData::logo_pngSize);
   #endif

    // Setup Top Preset & Utility Bar
    for (auto* btn : { &prevPresetBtn, &nextPresetBtn, &presetDisplayBtn, &savePresetBtn,
                       &loadPresetBtn, &undoBtn, &redoBtn, &btnA, &btnB, &copyABBtn })
    {
        addAndMakeVisible (btn);
    }

    btnA.setToggleState (true, juce::dontSendNotification);

    prevPresetBtn.onClick = [this] {
        int idx = (audioProcessor.getCurrentProgram() - 1 + audioProcessor.getNumPrograms()) % audioProcessor.getNumPrograms();
        audioProcessor.loadFactoryPreset (idx);
        presetDisplayBtn.setButtonText (audioProcessor.getCurrentPresetDisplayName());
    };
    nextPresetBtn.onClick = [this] {
        int idx = (audioProcessor.getCurrentProgram() + 1) % audioProcessor.getNumPrograms();
        audioProcessor.loadFactoryPreset (idx);
        presetDisplayBtn.setButtonText (audioProcessor.getCurrentPresetDisplayName());
    };
    presetDisplayBtn.onClick = [this] { openPresetMenu(); };
    savePresetBtn.onClick    = [this] { saveUserPresetDialog(); };
    loadPresetBtn.onClick    = [this] { loadUserPresetDialog(); };

    undoBtn.onClick = [this] { audioProcessor.undoManager.undo(); };
    redoBtn.onClick = [this] { audioProcessor.undoManager.redo(); };

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
    copyABBtn.onClick = [this] { audioProcessor.copyAtoB(); };

    // Setup Rotary Knobs
    auto setupKnob = [this] (juce::Slider& s, juce::Label& l, const juce::String& name)
    {
        s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 72, 20);
        addAndMakeVisible (s);

        l.setText (name, juce::dontSendNotification);
        l.setJustificationType (juce::Justification::centred);
        l.setFont (juce::FontOptions (13.0f, juce::Font::bold));
        l.setColour (juce::Label::textColourId, SciFiColors::neonGreen);
        addAndMakeVisible (l);
    };

    setupKnob (inTrimSlider,  inTrimLabel,  "INPUT TRIM");
    setupKnob (shuffleSlider, shuffleLabel, "SHUFFLE (Hz)");
    setupKnob (spaceSlider,   spaceLabel,   "SPACE (dB)");
    setupKnob (widthSlider,   widthLabel,   "WIDTH (dB)");
    setupKnob (outTrimSlider, outTrimLabel, "OUTPUT TRIM");

    // Setup Switches
    for (auto* tBtn : { &bypassBtn, &soloSideBtn, &inMSBtn, &outMSBtn })
    {
        tBtn->setClickingTogglesState (true);
        addAndMakeVisible (tBtn);
    }

    // Attachments
    inTrimAtt   = std::make_unique<SliderAttachment> (audioProcessor.apvts, "INPUT_GAIN",  inTrimSlider);
    shuffleAtt  = std::make_unique<SliderAttachment> (audioProcessor.apvts, "SHUFFLE",     shuffleSlider);
    spaceAtt    = std::make_unique<SliderAttachment> (audioProcessor.apvts, "SPACE",       spaceSlider);
    widthAtt    = std::make_unique<SliderAttachment> (audioProcessor.apvts, "WIDTH",       widthSlider);
    outTrimAtt  = std::make_unique<SliderAttachment> (audioProcessor.apvts, "OUTPUT_TRIM", outTrimSlider);

    bypassAtt   = std::make_unique<ButtonAttachment> (audioProcessor.apvts, "BYPASS",      bypassBtn);
    soloSideAtt = std::make_unique<ButtonAttachment> (audioProcessor.apvts, "SOLO_SIDE",   soloSideBtn);
    inMSAtt     = std::make_unique<ButtonAttachment> (audioProcessor.apvts, "IN_MS_MODE",  inMSBtn);
    outMSAtt    = std::make_unique<ButtonAttachment> (audioProcessor.apvts, "OUT_MS_MODE", outMSBtn);

    addAndMakeVisible (vectorscope);
    addAndMakeVisible (inputMeter);
    addAndMakeVisible (outputMeter);

    // Resizable GUI Configuration
    setResizable (true, true);
    setResizeLimits (780, 480, 1600, 1000);
    getConstrainer()->setFixedAspectRatio (920.0 / 560.0);
    setSize (920, 560);

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

    inputMeter.setLevels (audioProcessor.inLevelLeft.load(),  audioProcessor.inLevelRight.load(),
                          audioProcessor.inPeakLeft.load(),   audioProcessor.inPeakRight.load(), inMS);

    outputMeter.setLevels (audioProcessor.outLevelLeft.load(), audioProcessor.outLevelRight.load(),
                           audioProcessor.outPeakLeft.load(),  audioProcessor.outPeakRight.load(), outMS);

    vectorscope.repaint();
}

void NoizzImagerAudioProcessorEditor::openPresetMenu()
{
    juce::PopupMenu mainMenu;
    const auto& presets = audioProcessor.getFactoryPresets();

    juce::StringArray categories = { "Default", "Drums", "Vocals", "Guitars", "Bass",
                                     "Synth", "Pads", "Strings", "Horns", "Bus Channels", "Mix" };

    for (const auto& cat : categories)
    {
        juce::PopupMenu subMenu;
        for (int i = 0; i < (int) presets.size(); ++i)
        {
            if (presets[i].category == cat)
                subMenu.addItem (i + 1, presets[i].name, true, i == audioProcessor.getCurrentProgram());
        }
        mainMenu.addSubMenu (cat, subMenu);
    }

    mainMenu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&presetDisplayBtn),
                            [this] (int result)
                            {
                                if (result > 0)
                                {
                                    audioProcessor.loadFactoryPreset (result - 1);
                                    presetDisplayBtn.setButtonText (audioProcessor.getCurrentPresetDisplayName());
                                }
                            });
}

void NoizzImagerAudioProcessorEditor::saveUserPresetDialog()
{
    auto defaultDir = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
                          .getChildFile ("NOIZZ STUDIO/NOIZZ IMAGER Presets");
    defaultDir.createDirectory();

    fileChooser = std::make_unique<juce::FileChooser> ("Save User Preset", defaultDir, "*.noizz");
    fileChooser->launchAsync (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
                              [this] (const juce::FileChooser& fc)
                              {
                                  auto file = fc.getResult();
                                  if (file != juce::File{})
                                  {
                                      audioProcessor.saveUserPreset (file.withFileExtension (".noizz"));
                                      presetDisplayBtn.setButtonText (audioProcessor.getCurrentPresetDisplayName());
                                  }
                              });
}

void NoizzImagerAudioProcessorEditor::loadUserPresetDialog()
{
    auto defaultDir = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
                          .getChildFile ("NOIZZ STUDIO/NOIZZ IMAGER Presets");
    defaultDir.createDirectory();

    fileChooser = std::make_unique<juce::FileChooser> ("Load User Preset", defaultDir, "*.noizz");
    fileChooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [this] (const juce::FileChooser& fc)
                              {
                                  auto file = fc.getResult();
                                  if (file.existsAsFile())
                                  {
                                      audioProcessor.loadUserPreset (file);
                                      presetDisplayBtn.setButtonText (audioProcessor.getCurrentPresetDisplayName());
                                  }
                              });
}

void NoizzImagerAudioProcessorEditor::drawNoizzLogo (juce::Graphics& g, juce::Rectangle<float> area)
{
    if (logoImage.isValid())
    {
        g.drawImageWithin (logoImage, (int)area.getX(), (int)area.getY(),
                           (int)area.getWidth(), (int)area.getHeight(),
                           juce::RectanglePlacement::centred);
        return;
    }

    // Procedural high-precision vector rendition of the NOIZZ STUDIO Eye of Horus logo
    float cx = area.getCentreX();
    float cy = area.getCentreY() - 6.0f;
    float r  = juce::jmin (area.getWidth(), area.getHeight()) * 0.40f;

    g.setColour (SciFiColors::neonGreen);
    for (float mult : { 1.0f, 0.90f, 0.80f, 0.70f })
        g.drawEllipse (cx - r * mult, cy - r * mult, r * mult * 2.0f, r * mult * 2.0f, 1.2f);

    // Pyramid Triangle
    juce::Path tri;
    tri.addTriangle (cx, cy - r * 0.65f, cx - r * 0.6f, cy + r * 0.45f, cx + r * 0.6f, cy + r * 0.45f);
    g.strokePath (tri, juce::PathStrokeType (1.5f));

    // Eye of Horus inside Pyramid
    g.fillEllipse (cx - 3.0f, cy - 4.0f, 6.0f, 6.0f);
    g.drawEllipse (cx - 10.0f, cy - 6.0f, 20.0f, 9.0f, 1.5f);

    // NOIZZ STUDIO Typography
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (21.0f, juce::Font::bold));
    g.drawText ("NOIZZ", (int)area.getX(), (int)(cy + r * 0.1f), (int)area.getWidth(), 24, juce::Justification::centred);
    g.setColour (SciFiColors::neonGreen);
    g.setFont (juce::FontOptions (11.0f, juce::Font::italic | juce::Font::bold));
    g.drawText ("STUDIO", (int)area.getX(), (int)(cy + r * 0.1f + 19.0f), (int)area.getWidth(), 16, juce::Justification::centred);
}

void NoizzImagerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (SciFiColors::pureBlack);

    auto bounds = getLocalBounds().toFloat();

    // Sci-Fi Chassis Frame
    g.setColour (SciFiColors::panelGray);
    g.fillRoundedRectangle (bounds.reduced (8.0f), 8.0f);
    g.setColour (SciFiColors::neonGreen.withAlpha (0.6f));
    g.drawRoundedRectangle (bounds.reduced (8.0f), 8.0f, 1.8f);

    // Header Banner
    auto headerArea = bounds.reduced (12.0f).removeFromTop (68.0f);
    g.setColour (SciFiColors::pureBlack);
    g.fillRoundedRectangle (headerArea, 6.0f);
    g.setColour (SciFiColors::dimGreen);
    g.drawRoundedRectangle (headerArea, 6.0f, 1.2f);

    // Draw NOIZZ STUDIO Logo on the Left
    auto logoRect = headerArea.removeFromLeft (130.0f).reduced (4.0f);
    drawNoizzLogo (g, logoRect);

    // Plugin Title "NOIZZ IMAGER"
    auto titleRect = headerArea.removeFromLeft (200.0f);
    g.setColour (SciFiColors::neonGreen);
    g.setFont (juce::FontOptions (24.0f, juce::Font::bold));
    g.drawText ("NOIZZ IMAGER", titleRect.toNearestInt(), juce::Justification::centredLeft);

    // Subtle Sci-Fi Section Dividers
    g.setColour (SciFiColors::dimGreen.withAlpha (0.5f));
    float yTop = 92.0f;
    float yBot = bounds.getHeight() - 18.0f;
    float w = bounds.getWidth();
    g.drawLine (w * 0.21f, yTop, w * 0.21f, yBot, 1.2f);
    g.drawLine (w * 0.79f, yTop, w * 0.79f, yBot, 1.2f);
}

void NoizzImagerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (14);

    // 1. Top Header Bar (Logo on Left + Title + Preset Bar & A/B/Undo/Redo on Right)
    auto header = area.removeFromTop (68);
    header.removeFromLeft (330); // Space reserved for Logo + "NOIZZ IMAGER" title

    auto presetRow = header.reduced (6, 16);
    prevPresetBtn.setBounds    (presetRow.removeFromLeft (28));
    presetRow.removeFromLeft (4);
    presetDisplayBtn.setBounds (presetRow.removeFromLeft (185));
    presetRow.removeFromLeft (4);
    nextPresetBtn.setBounds    (presetRow.removeFromLeft (28));
    presetRow.removeFromLeft (8);
    savePresetBtn.setBounds    (presetRow.removeFromLeft (48));
    presetRow.removeFromLeft (4);
    loadPresetBtn.setBounds    (presetRow.removeFromLeft (48));
    presetRow.removeFromLeft (10);
    undoBtn.setBounds          (presetRow.removeFromLeft (46));
    presetRow.removeFromLeft (4);
    redoBtn.setBounds          (presetRow.removeFromLeft (46));
    presetRow.removeFromLeft (10);
    btnA.setBounds             (presetRow.removeFromLeft (26));
    presetRow.removeFromLeft (3);
    btnB.setBounds             (presetRow.removeFromLeft (26));
    presetRow.removeFromLeft (4);
    copyABBtn.setBounds        (presetRow.removeFromLeft (54));

    area.removeFromTop (10);

    // 2. Main 3-Column Body: Left (Input), Center (Vectorscope + Shuffle/Space/Width), Right (Output)
    int sideWidth = (int) (area.getWidth() * 0.20f);
    auto leftCol  = area.removeFromLeft (sideWidth).reduced (6);
    auto rightCol = area.removeFromRight (sideWidth).reduced (6);
    auto centerCol = area.reduced (10, 4);

    // --- LEFT COLUMN (Input Meter, M/S Switch, Input Trim, Bypass) ---
    auto inBottom = leftCol.removeFromBottom (195);
    inMSBtn.setBounds (leftCol.removeFromTop (24).reduced (18, 0));
    leftCol.removeFromTop (4);
    inputMeter.setBounds (leftCol.reduced (14, 0));

    inTrimLabel.setBounds  (inBottom.removeFromTop (20));
    inTrimSlider.setBounds (inBottom.removeFromTop (125));
    inBottom.removeFromTop (8);
    bypassBtn.setBounds    (inBottom.removeFromTop (32).reduced (12, 0));

    // --- RIGHT COLUMN (Output Meter, M/S Switch, Output Trim, Solo Side) ---
    auto outBottom = rightCol.removeFromBottom (195);
    outMSBtn.setBounds (rightCol.removeFromTop (24).reduced (18, 0));
    rightCol.removeFromTop (4);
    outputMeter.setBounds (rightCol.reduced (14, 0));

    outTrimLabel.setBounds  (outBottom.removeFromTop (20));
    outTrimSlider.setBounds (outBottom.removeFromTop (125));
    outBottom.removeFromTop (8);
    soloSideBtn.setBounds   (outBottom.removeFromTop (32).reduced (12, 0));

    // --- CENTER COLUMN (Polar Vectorscope on Top, SHUFFLE / SPACE / WIDTH Knobs Below) ---
    auto knobsRow = centerCol.removeFromBottom (185);
    vectorscope.setBounds (centerCol.reduced (4));

    int knobW = knobsRow.getWidth() / 3;
    auto shufArea  = knobsRow.removeFromLeft (knobW).reduced (6);
    auto spaceArea = knobsRow.removeFromLeft (knobW).reduced (6);
    auto widthArea = knobsRow.reduced (6);

    shuffleLabel.setBounds  (shufArea.removeFromTop (22));
    shuffleSlider.setBounds (shufArea);

    spaceLabel.setBounds    (spaceArea.removeFromTop (22));
    spaceSlider.setBounds   (spaceArea);

    widthLabel.setBounds    (widthArea.removeFromTop (22));
    widthSlider.setBounds   (widthArea);
}