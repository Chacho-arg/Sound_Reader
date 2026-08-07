// TonalCurveComponent.cpp
#include "TonalCurveComponent.h"
#include <cmath>

TonalCurveComponent::TonalCurveComponent (TargetPresetManager& presetManager)
    : presetManager(presetManager)
{
    setBounds(350, 10, 640, 400);
    startTimer(33); // ~30 FPS for real-time updates
    
    setOpaque(true);
}

TonalCurveComponent::~TonalCurveComponent() = default;

void TonalCurveComponent::paint (juce::Graphics& g)
{
    // Background
    g.setColour(juce::Colour(0xFF12181F)); // Dark Navy Slate
    g.fillRect(getLocalBounds().toFloat());

    // Draw frequency grid lines
    g.setColour(juce::Colour(0xFF1B222C));
    g.setFont(10.0f);

    const int plotLeft = margin;
    const int plotRight = getWidth() - margin;
    const int plotTop = margin;
    const int plotBottom = getHeight() - margin;

    // Vertical frequency lines (logarithmic scale)
    for (double freq : {20.0, 50.0, 100.0, 200.0, 500.0, 1000.0, 2000.0, 5000.0, 10000.0, 20000.0})
    {
        int x = freqToX(freq);
        g.drawVerticalLine(x, plotTop, plotBottom);
        
        juce::String freqText = (freq >= 1000) ? juce::String(freq/1000.0, 1) + "k" : juce::String((int)freq);
        g.drawText(freqText, x - 15, plotBottom + 2, 30, 15, juce::JustifyCentre);
    }

    // Horizontal dB lines
    for (int db : {-24, -18, -12, -6, 0, 6, 12})
    {
        int y = plotTop + (plotBottom - plotTop) * (1.0 - (db + 24.0) / 36.0);
        g.drawHorizontalLine(y, plotLeft, plotRight);
        
        g.drawText(juce::String(db) + " dB", 5, y - 8, margin - 5, 16, juce::JustifyRight);
    }

    // Draw target envelopes (crest curves)
    g.setColour(juce::Colour(0xFF00838F)); // Dark Teal
    g.setOpacity(0.3f);
    
    juce::Path envelopePath = createEnvelopePath();
    g.fillPath(envelopePath);
    
    // Draw target envelope outline
    g.setOpacity(1.0f);
    g.setColour(juce::Colour(0xFF00ACC1)); // Cyan/Teal
    g.strokePath(envelopePath, juce::PathStrokeType(2.0f));

    // Draw real-time energy trace
    g.setColour(juce::Colour(0xFF80DEEA)); // Bright Cyan
    juce::Path energyPath = createEnergyPath();
    g.strokePath(energyPath, juce::PathStrokeType(2.0f));

    // Draw band labels
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    
    const int bandY = plotBottom + 20;
    double freqStart = minFreq;
    for (const auto& band : bandTargets)
    {
        int xCenter = (freqToX(freqStart) + freqToX(band.maxFreq)) / 2;
        g.drawText(band.name, xCenter - 30, bandY, 60, 20, juce::JustifyCentre);
        freqStart = band.maxFreq;
    }

    // Legend
    g.setFont(10.0f);
    g.setColour(juce::Colour(0xFF00ACC1));
    g.drawText("Target Envelope", 20, 20, 120, 15, juce::JustifyLeft);
    g.setColour(juce::Colour(0xFF80DEEA));
    g.drawText("Real-Time Trace", 150, 20, 120, 15, juce::JustifyLeft);
}

void TonalCurveComponent::resized()
{
    // No child components, just custom painting
}

void TonalCurveComponent::updateBandEnergies (double low, double lowMid, double highMid, double high)
{
    currentEnergies[0] = low;
    currentEnergies[1] = lowMid;
    currentEnergies[2] = highMid;
    currentEnergies[3] = high;
}

void TonalCurveComponent::updatePreset (const juce::String& presetName)
{
    auto targets = presetManager.getTonalBandTargets(presetName);
    bandTargets.clear();
    
    for (const auto& t : targets)
    {
        BandTarget bt;
        bt.name = t.name;
        bt.target = t.target;
        bt.tolerance = t.tolerance;
        bt.minFreq = t.minFreq;
        bt.maxFreq = t.maxFreq;
        bandTargets.push_back(bt);
    }
    
    repaint();
}

// Helper: Map frequency to X coordinate (logarithmic scale)
int TonalCurveComponent::freqToX (double freq) const
{
    const int plotLeft = margin;
    const int plotRight = getWidth() - margin;
    
    double logMin = std::log10(minFreq);
    double logMax = std::log10(maxFreq);
    double logFreq = std::log10(juce::jlimit((double)minFreq, (double)maxFreq, freq));
    
    double ratio = (logFreq - logMin) / (logMax - logMin);
    return plotLeft + (int)(ratio * (plotRight - plotLeft));
}

// Helper: Map energy (dB) to Y coordinate
double TonalCurveComponent::energyToY (double energy) const
{
    const int plotTop = margin;
    const int plotBottom = getHeight() - margin;
    
    // Energy in dB range [-24, +12] dB
    double db = juce::jlimit(-24.0, 12.0, energy);
    double ratio = (db + 24.0) / 36.0; // 0 to 1
    
    return plotBottom - ratio * (plotBottom - plotTop);
}

// Create path for target envelope
juce::Path TonalCurveComponent::createEnvelopePath() const
{
    juce::Path path;
    
    if (bandTargets.empty())
        return path;
    
    const int plotLeft = margin;
    const int plotRight = getWidth() - margin;
    const int plotTop = margin;
    const int plotBottom = getHeight() - margin;
    
    // Build upper envelope (target + tolerance)
    path.startNewSubPath(plotLeft, plotBottom);
    
    for (const auto& band : bandTargets)
    {
        int xStart = freqToX(band.minFreq);
        int xEnd = freqToX(band.maxFreq);
        
        double upperDb = band.target + band.tolerance;
        double upperY = energyToY(upperDb);
        
        path.lineTo(xStart, upperY);
        path.lineTo(xEnd, upperY);
    }
    
    // Close path at bottom
    path.lineTo(plotRight, plotBottom);
    path.closeSubPath();
    
    return path;
}

// Create path for real-time energy trace
juce::Path TonalCurveComponent::createEnergyPath() const
{
    juce::Path path;
    
    if (bandTargets.empty())
        return path;
    
    // Draw connecting lines between band energies
    for (size_t i = 0; i < bandTargets.size(); ++i)
    {
        const auto& band = bandTargets[i];
        double energy = currentEnergies[i];
        
        int xStart = freqToX(band.minFreq);
        int xEnd = freqToX(band.maxFreq);
        
        double energyY = energyToY(energy);
        
        if (i == 0)
            path.startNewSubPath(xStart, energyY);
        else
            path.lineTo(xStart, energyY);
        
        path.lineTo(xEnd, energyY);
    }
    
    return path;
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TonalCurveComponent)