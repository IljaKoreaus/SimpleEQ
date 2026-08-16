#include "PluginProcessor.h"
#include "PluginEditor.h"

void LookAndFeel::drawRotarySlider (juce::Graphics& g,
                                         int x, int y, int width, int height,
                                         float sliderPosProportional,
                                         float rotaryStartAngle,
                                         float rotaryEndAngle,
                                         juce::Slider& slider)
{
    using namespace juce;
    
    auto bounds = Rectangle<float>(x, y, width, height);
    g.setColour(Colour(97u, 18u, 167u));
    g.fillEllipse(bounds);
    
    g.setColour(Colour(255u, 154u, 1u));
    g.drawEllipse(bounds, 1.f);
    
    if (auto* rswl = dynamic_cast<RotaryWithLabels*>(&slider)) {
     
        auto center = bounds.getCentre();
        
        Path p;
        
        Rectangle<float> r;
        r.setLeft(center.getX()-2);
        r.setRight(center.getX()+2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - rswl->getTextHeight() * 2);
        
        p.addRoundedRectangle(r, 2.f);
        
        jassert(rotaryStartAngle < rotaryEndAngle);
        
        auto sliderAngRad = jmap(sliderPosProportional, 0.0f, 1.f, rotaryStartAngle, rotaryEndAngle);
        
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        
        g.setColour(Colours::white);
        g.fillPath(p);
        
        g.setFont(rswl->getTextHeight());
        auto text = rswl->getDisplayString();

        juce::GlyphArrangement glyphs;

        glyphs.addLineOfText(g.getCurrentFont(), text, 0.0f, 0.0f);

        auto strWidth = glyphs.getBoundingBox(0, -1, false).getWidth();
        
        r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
        r.setCentre(bounds.getCentre());
        
        g.setColour(Colours::black);
        g.fillRect(r);
        
        g.setColour(Colours::white);
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

void LookAndFeel::drawToggleButton(juce::Graphics &g,
                                   juce::ToggleButton &toggleButton,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown)
{
    using namespace juce;
    
    if (auto* pb = dynamic_cast<PowerButton*>(&toggleButton))
    {
        Path powerButton;
        auto bounds = toggleButton.getLocalBounds();
        auto size = jmin(bounds.getWidth(), bounds.getHeight() - 6);
        auto r = bounds.withSizeKeepingCentre(size, size).toFloat();
        
        float ang = 30.f;
        
        powerButton.addCentredArc(r.getCentreX(), r.getCentreY(),
                                  size * 0.5,
                                  size * 0.5,
                                  0.f,
                                  degreesToRadians(ang),
                                  degreesToRadians(360-ang),
                                  true);
        
        powerButton.startNewSubPath(r.getCentreX(), r.getY());
        powerButton.lineTo(r.getCentre());
        
        PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);
        
        auto colour = toggleButton.getToggleState() ? Colours::dimgrey : Colours::green;
        
        g.setColour(colour);
        g.strokePath(powerButton, pst);
        g.drawEllipse(r, 2);
        
    } else if (auto* analyzerButton = dynamic_cast<AnalyzerButton*>(&toggleButton)) {
        
        auto colour = ! toggleButton.getToggleState() ? Colours::dimgrey : Colours::green;
        g.setColour(colour);
        
        auto bounds = toggleButton.getLocalBounds();
        g.drawRect(bounds);
        
        g.strokePath(analyzerButton->randomPath, PathStrokeType(1.f));
    }
}

void RotaryWithLabels::paint(juce::Graphics& g)
{
    using namespace juce;
    
    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;
    
    auto range = getRange();
    auto sliderBounds = getSliderBounds();
    
    getLookAndFeel().drawRotarySlider(g, sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0 ),
                                      startAng,
                                      endAng,
                                      *this);
    
    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * 0.5f + getTextHeight() + 5;

    g.setColour(Colour(0u, 172u, 1u));
    g.setFont(getTextHeight());

    auto numChoices = labels.size();

    for (int i = 0; i < numChoices; i++)

    {

        auto pos = labels[i].pos;

        jassert(0.f <= pos);
        jassert(pos <= 1.f);

        auto ang = jmap(pos, 0.0f, 1.0f, startAng, endAng);
        auto c = center.getPointOnCircumference(radius, ang);
        auto str = labels[i].label;

        juce::GlyphArrangement glyphs;

        glyphs.addLineOfText(g.getCurrentFont(), str, 0.0f, 0.0f);

        auto strWidth = glyphs.getBoundingBox(0, -1, false).getWidth();

        Rectangle<float> r;

        r.setSize(strWidth + 4, getTextHeight() + 2);
        r.setCentre(c);
        g.drawFittedText(str,
                         r.toNearestInt(),
                         juce::Justification::centred,
                         1
        );

    }
}

juce::Rectangle<int> RotaryWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    
    size -= getTextHeight() * 2;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(2);
    
    return r;
}

juce::String RotaryWithLabels::getDisplayString() const
{
    
    if (auto* choiseParam = dynamic_cast<juce::AudioParameterChoice*>(param))
    {
        return choiseParam->getCurrentChoiceName();
    }
    
    juce::String str;
    auto addK = false;
    
    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
    {
        float val = getValue();
        
        if (val > 999.f) {
            
            val /= 1000.f;
            addK = true;
        }
        
        str = juce::String(val, (addK ? 2 : 0 ));
        
    } else {
        
        jassertfalse; // This shouldn't happen!!
    }
    
    
    if (suffix.isNotEmpty()) {
        
        str << " ";
        if (addK) {
            str << "k";
        }
        
        str << suffix;
    }
    
    return str;
    
}
//==============================================================================
ResponseCurveComponent::ResponseCurveComponent(AudioPluginAudioProcessor& p) :
processorRef(p),
leftPathProducer(processorRef.leftChannelFifo),
rightPathProducer(processorRef.rightChannelFifo)
{
    const auto& params = processorRef.getParameters();
    
    for (auto param : params) {
        
        param->addListener(this);
    }
    
    updateChain();
    
    startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
    const auto& params = processorRef.getParameters();
    
    for (auto param : params) {
        
        param->removeListener(this);
    }
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}

void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate) {

    if (sampleRate <= 0.0 || fftBounds.isEmpty())
        return;

    if (sampleRate <= 0.0 || fftBounds.isEmpty())
        return;
    
    // *** COORDINATING SCSF WITH FFT AND JUCE::PATH ***
    // SCSF
    juce::AudioBuffer<float> tempIncomingBuffer;
    
    while (leftChannelFifo->getNumCompleteBuffersAvailable() > 0) {
        
        // IF THIS CAN BE PULLED, IT'L BE SENT TO THE FFT
        if (leftChannelFifo->getAudioBuffer(tempIncomingBuffer)) {
            
            auto size = tempIncomingBuffer.getNumSamples();
            
            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, 0),
                                              monoBuffer.getReadPointer(0, size),
                                              monoBuffer.getNumSamples() - size);
            
            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),
                                              tempIncomingBuffer.getReadPointer(0, 0),
                                              size);
            
            leftChannelFFTDataGenerator.produceFFTDataForRendering(monoBuffer, -48.f);
        }
    }
    
    
    /* If there are FFT data buffers to pull
        if we can pull a buffer
        generate a path */
    const auto fftSize = leftChannelFFTDataGenerator.getFFTSize();
    const auto binWidth = sampleRate / (double)fftSize;
    
    while (leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0) {
        
        std::vector<float> fftData;
        if (leftChannelFFTDataGenerator.getFFTData(fftData)) {
            pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, -48.f);
        }
    }
    
    /* While there's paths to be pullde
        pull as many as we can
        display the most recent */
    
    while (pathProducer.getNumPathsAvailable()) {
        pathProducer.getPath(leftChannelFFTPath);
    }
}

void ResponseCurveComponent::timerCallback()
{
    
    // RESPONSE CURVE PROCESS
    auto fftBounds = getAnalysisArea().toFloat();
    auto sampleRate = processorRef.getSampleRate();
    rightPathProducer.process(fftBounds, sampleRate);
    leftPathProducer.process(fftBounds, sampleRate);
    
    if (parametersChanged.compareAndSetBool(false, true)) {
        
        // Update monochain
        updateChain();
        // signal a repaint
        //repaint();
        
    }
    
    repaint();
}

void ResponseCurveComponent::updateChain() {
    
    // Update monochain
    const auto sampleRate = processorRef.getSampleRate();
    if (sampleRate <= 0.0)
    {
        parametersChanged.set(true);
        return;
    }

    auto chainSettings = getChainSettings(processorRef.apvts);
    
    monoChain.setBypassed<ChainPositions::LowCut>(chainSettings.lowCutBypassed);
    monoChain.setBypassed<ChainPositions::HighCut>(chainSettings.highCutBypassed);
    monoChain.setBypassed<ChainPositions::Peak>(chainSettings.peakBypassed);
    
    auto peakCoefficients = makePeakFilter(chainSettings, sampleRate);
    updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
    
    auto lowCutCoefficients = makeLowCutFilter(chainSettings, sampleRate);
    auto highCutCoefficients = makeHighCutFilter(chainSettings, sampleRate);
    updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);
    updateCutFilter(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);
    
}

void ResponseCurveComponent::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);
    
    g.drawImage(background, getLocalBounds().toFloat());
    
    auto responseArea = getAnalysisArea();
    auto width = responseArea.getWidth();
    
    auto& lowcut = monoChain.get<ChainPositions::LowCut>();
    auto& peak = monoChain.get<ChainPositions::Peak>();
    auto& highcut = monoChain.get<ChainPositions::HighCut>();
    
    auto sampleRate = processorRef.getSampleRate();
    if (sampleRate <= 0.0)
    {
        g.setColour(Colours::orange);
        g.drawRoundedRectangle(getRenderArea().toFloat(), 4.f, 1.f);
        return;
    }
    
    std::vector<double> mags;
    
    mags.resize(width);
    
    for (int i = 0; i < width; i++)
    {
        double mag = 1.f;
        auto freq = mapToLog10(double(i) / double(width), 20.0, 20000.0);
        
        if (! monoChain.isBypassed<ChainPositions::Peak>())
        {
            mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }
        
        if (!monoChain.isBypassed<ChainPositions::LowCut>()) {
            
            if (! lowcut.isBypassed<0>())
            {
                mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (! lowcut.isBypassed<1>())
            {
                mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (! lowcut.isBypassed<2>())
            {
                mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (! lowcut.isBypassed<3>())
            {
                mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
        }
        
        if (!monoChain.isBypassed<ChainPositions::HighCut>()) {
            
            if (! highcut.isBypassed<0>())
            {
                mag *= highcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (! highcut.isBypassed<1>())
            {
                mag *= highcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (! highcut.isBypassed<2>())
            {
                mag *= highcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (! highcut.isBypassed<3>())
            {
                mag *= highcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
        }
        
        mags[i] = Decibels::gainToDecibels(mag);
    }
    
    // Drawing response curve
    Path responseCurve;
    
    const double outputMin = responseArea.getHeight();
    const double outputMax = 0.0;
    auto map = [outputMin, outputMax](double input)
    {
        return jmap(jlimit(-24.0, 24.0, input), -24.0, 24.0, outputMin, outputMax);
    };
    
    responseCurve.startNewSubPath(0.0f, map(mags.front()));
    
    for (size_t i = 0; i < mags.size(); i++) {
        responseCurve.lineTo(static_cast<float>(i), map(mags[i]));
    }
    
    // PAINTING RESPONSE CURVE
    auto leftChannelFFTPath = leftPathProducer.getPath();
    auto rightChannelFFTPath = rightPathProducer.getPath();
    const auto analysisAreaOffset = AffineTransform::translation(responseArea.getX(), responseArea.getY());
    g.saveState();
    g.reduceClipRegion(responseArea);
    g.setColour(Colours::rebeccapurple);
    g.strokePath(leftChannelFFTPath, PathStrokeType(1.f), analysisAreaOffset);
    g.setColour(Colours::red);
    g.strokePath(rightChannelFFTPath, PathStrokeType(1.f), analysisAreaOffset);
    
    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2), analysisAreaOffset);
    g.restoreState();

    g.setColour(Colours::orange);
    g.drawRoundedRectangle(getRenderArea().toFloat(), 4.f, 1.f);
}

void ResponseCurveComponent::resized()
{
    using namespace juce;
    background = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);
    
    Graphics g(background);
    
    Array<float> freqs {
        20, 50, 100,
        200, 500, 1000,
        2000, 5000, 10000,
        20000
    };
    
    auto renderArea = getAnalysisArea();
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();
    
    Array<float> xs;
    
    for (auto freq : freqs) {
        
        auto normX = mapFromLog10(freq, 20.f, 20000.f);
        xs.add(left + width * normX);
    }
    
    g.setColour(Colours::dimgrey);
    
    for (auto x : xs) {
        
        g.drawVerticalLine(x, top, bottom);
    }
    
    
    Array<float> gain {
        
        -24, -12, 0, 12, 24
    };
    
    for (auto gDb : gain) {
        
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));
        
        g.setColour(gDb == 0 ? Colour(97u, 18u, 167u) : Colours::darkgrey);
        g.drawHorizontalLine(y, left, right);
    }
    
    g.setColour(Colours::lightgrey);
    const int fontHeight = 10;
    g.setFont(fontHeight);
    
    int freqSize = freqs.size();
    
    for (int i = 0; i < freqSize; i++) {
        
        auto freq = freqs[i];
        auto x = xs[i];
        
        bool addK = false;
        String str;
        
        if (freq > 999.9f) {
            
            addK = true;
            freq /= 1000.f;
        }
        
        str << freq;
        
        if (addK) {
            str << "k";
        }
        str << "Hz";
        
        juce::GlyphArrangement glyphs;
        
        glyphs.addLineOfText(g.getCurrentFont(), str, 0.0f, 0.0f);
        
        auto strWidth = glyphs.getBoundingBox(0, -1, false).getWidth();
        
        Rectangle<float> r;
        r.setSize(strWidth, fontHeight);
        r.setCentre(x, 0);
        r.setY(1);
        g.drawFittedText(str,
                         r.toNearestInt(),
                         juce::Justification::centred,
                         1
                         );
    }
    
    for (auto gDb : gain) {
        
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));
        String str;
        
        if (gDb > 0) {
            str << "+";
        }
        
        str << gDb;
        
        juce::GlyphArrangement glyphs;
        
        glyphs.addLineOfText(g.getCurrentFont(), str, 0.0f, 0.0f);
        
        auto strWidth = glyphs.getBoundingBox(0, -1, false).getWidth();
        
        Rectangle<float> r;
        r.setSize(strWidth, fontHeight);
        r.setX(getWidth() - strWidth - 3);
        r.setCentre(r.getCentreX(), y);
        g.drawFittedText(str,
                         r.toNearestInt(),
                         juce::Justification::centred,
                         1
                         );
        
        g.setColour(gDb == 0 ? Colour(97u, 18u, 167u) : Colours::lightgrey);
        g.drawFittedText(str,
                         r.toNearestInt(),
                         juce::Justification::centred,
                         1
                         );
        
        str.clear();
        str << (gDb - 24.f);
        r.setX(1);
        strWidth = glyphs.getBoundingBox(0, -1, false).getWidth();
        r.setSize(strWidth, fontHeight);
        g.setColour(Colours::lightgrey);
        g.drawFittedText(str,
                         r.toNearestInt(),
                         juce::Justification::centred,
                         1
                         );
    }
}

juce::Rectangle<int> ResponseCurveComponent::getRenderArea() {
    
    auto bounds = getLocalBounds();
    
    bounds.reduce(10, 8);
    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);
    
    return bounds;
}

juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea() {
    
    auto bounds = getRenderArea();
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    
    return bounds;
}

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
peakFreqSlider(*processorRef.apvts.getParameter("Peak Freq"), "Hz"),
peakGainSlider(*processorRef.apvts.getParameter("Peak Gain"), "Db"),
peakQualitySlider(*processorRef.apvts.getParameter("Peak Quality"), ""),
lowCutFreqSlider(*processorRef.apvts.getParameter("LowCut Freq"), "Hz"),
highCutFreqSlider(*processorRef.apvts.getParameter("HighCut Freq"), "Hz"),
lowCutSlopeSlider(*processorRef.apvts.getParameter("LowCut Slope"), "Db/Oct"),
highCutSlopeSlider(*processorRef.apvts.getParameter("HighCut Slope"), "Db/Oct"),
responseCurveComponent(processorRef),
peakFreqSliderAttachment(processorRef.apvts, "Peak Freq", peakFreqSlider),
peakGainSliderAttachment(processorRef.apvts, "Peak Gain", peakGainSlider),
peakQualitySliderAttachment(processorRef.apvts, "Peak Quality", peakQualitySlider),
lowCutFreqSliderAttachment(processorRef.apvts, "LowCut Freq", lowCutFreqSlider),
highCutFreqSliderAttachment(processorRef.apvts, "HighCut Freq", highCutFreqSlider),
lowCutSlopeSliderAttachment(processorRef.apvts, "LowCut Slope", lowCutSlopeSlider),
highCutSlopeSliderAttachment(processorRef.apvts, "HighCut Slope", highCutSlopeSlider),
lowCutBypassButtonAttachment(processorRef.apvts, "LowCut Bypassed", lowCutBypassButton),
highCutBypassButtonAttachment(processorRef.apvts, "HighCut Bypassed", highCutBypassButton),
peakBypassButtonAttachment(processorRef.apvts, "Peak Bypassed", peakBypassButton),
analyzerEnabledButtonAttachment(processorRef.apvts, "Analyzer Enabled", analyzerEnabledButton)
{
    
    peakFreqSlider.labels.add({0.0f, "20Hz"});
    peakFreqSlider.labels.add({1.0f, "20kHz"});
    
    peakGainSlider.labels.add({0.0f, "-24Db"});
    peakGainSlider.labels.add({1.0f, "+24Db"});
    
    peakQualitySlider.labels.add({0.0f, "0.1"});
    peakQualitySlider.labels.add({1.0f, "10.0"});
    
    lowCutFreqSlider.labels.add({0.0f, "20Hz"});
    lowCutFreqSlider.labels.add({1.0f, "20kHz"});
    
    highCutFreqSlider.labels.add({0.0f, "20Hz"});
    highCutFreqSlider.labels.add({1.0f, "20kHz"});
    
    lowCutSlopeSlider.labels.add({0.0f, "12"});
    lowCutSlopeSlider.labels.add({1.0f, "48"});
    
    highCutSlopeSlider.labels.add({0.0f, "12"});
    highCutSlopeSlider.labels.add({1.0f, "48"});
    
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (auto* comp : getComps()) {
        
        addAndMakeVisible(comp);
    }
    
    peakBypassButton.setLookAndFeel(&lnf);
    highCutBypassButton.setLookAndFeel(&lnf);
    lowCutBypassButton.setLookAndFeel(&lnf);
    analyzerEnabledButton.setLookAndFeel(&lnf);
    
    setSize (600, 480);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    peakBypassButton.setLookAndFeel(nullptr);
    highCutBypassButton.setLookAndFeel(nullptr);
    lowCutBypassButton.setLookAndFeel(nullptr);
    analyzerEnabledButton.setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    
    auto analyzerEnabledArea = bounds.removeFromTop(25);
    analyzerEnabledArea.setWidth(100);
    analyzerEnabledArea.setX(5);
    analyzerEnabledArea.removeFromTop(2);
    analyzerEnabledButton.setBounds(analyzerEnabledArea);
    
    bounds.removeFromTop(5);
    
    float hRatio = 25.f / 100.f;
    auto responseArea = bounds.removeFromTop((bounds.getHeight() * hRatio));
    
    responseCurveComponent.setBounds(responseArea);
    
    bounds.removeFromTop(10);
    
    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);
    
    
    lowCutBypassButton.setBounds(lowCutArea.removeFromTop(25));
    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    lowCutSlopeSlider.setBounds(lowCutArea);
    
    highCutBypassButton.setBounds(highCutArea.removeFromTop(25));
    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds(highCutArea);
    
    peakBypassButton.setBounds(bounds.removeFromTop(25));
    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    peakQualitySlider.setBounds(bounds);
}

std::vector<juce::Component*> AudioPluginAudioProcessorEditor::getComps()
{
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutFreqSlider,
        &highCutFreqSlider,
        &lowCutSlopeSlider,
        &highCutSlopeSlider,
        &responseCurveComponent,
        
        &lowCutBypassButton,
        &highCutBypassButton,
        &peakBypassButton,
        &analyzerEnabledButton
    };
    
}
