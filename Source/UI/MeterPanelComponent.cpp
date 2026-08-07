// MeterPanelComponent.cpp
#include "MeterPanelComponent.h"
#include "TargetPresetManager.h"
#include <juce_core/juce_core.h>

MeterPanelComponent::MeterPanelComponent(TargetPresetManager& presetManager)
    : presetManager(presetManager)
{
    addChildComponent(&momentaryLabel);
    addChildComponent(&shortTermLabel);
    addChildComponent(&integratedLabel);
    addChildComponent(&lraLabel);
    addChildComponent(&momentaryMaxLabel);
    addChildComponent(&shortTermMaxLabel);
    addChildComponent(&truePeakMaxLabel);
    addChildComponent(&psrLabel);
    addChildComponent(&plrLabel);
    addChildComponent(&presetLabel);

    setBounds(10, 10, 300, 420);

    startTimer(33);
    
    presetLabel.setFont(12.0f);
    presetLabel.setColour(juce::Colours::blue);
    presetLabel.setEditable(false);
    presetLabel.setText("Preset: Loading...", juce::dontSendNotification);
    presetLabel.setJustification(juce::Justification::Left);
    addAndMakeVisible(&presetLabel);
}

MeterPanelComponent::~MeterPanelComponent() = default;

void MeterPanelComponent::paint (juce::Graphics& g)
{
    // Background
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(getLocalBounds().toFloat());

    // Title
    g.setColour(juce::Colours::darkblue);
    g.setFont(16.0f);
    g.drawText("SoundReader - Meter Panel", 20, 10, getWidth() - 40, 20, juce::JustifyLeft);

    // Metrics display
    g.setFont(12.0f);
    const int y = 40;

    // Format numeric values nicely
    auto formatNumber = [&](double value) -> juce::String {
        return juce::String(value, 2);
    };

    g.setColour(juce::Colours::white);
    g.drawText("Momentary LUFS", 20, y, 180, 20, juce::JustifyLeft);
    g.drawText(formatNumber(momentaryLUFS) + " LUFS", 20, y + 25, 180, 20, juce::JustifyLeft);

    g.drawText("Short-Term LUFS", 20, y + 50, 180, 20, juce::JustifyLeft);
    g.drawText(formatNumber(shortTermLUFS) + " LUFS", 20, y + 75, 180, 20, juce::JustifyLeft);

    g.drawText("Integrated LUFS", 20, y + 100, 180, 20, juce::JustifyLeft);
    g.drawText(formatNumber(integratedLUFS) + " LUFS", 20, y + 125, 180, 20, juce::JustifyLeft);

    g.drawText("LRA (LU)", 20, y + 150, 180, 20, juce::JustifyLeft);
    g.drawText(formatNumber(lraLU) + " LU", 20, y + 175, 180, 20, juce::JustifyLeft);

    g.drawText("Momentary Max", 20, y + 200, 180, 20, juce::JustifyLeft);
    g.drawText(formatNumber(momentaryMax) + " dBFS", 20, y + 225, 180, 20, juce::JustifyLeft);

    g.drawText("Short-Term Max", 20, y + 250, 180, 20, juce::JustifyLeft);
    g.drawText(formatNumber(shortTermMax) + " dBFS", 20, y + 275, 180, 20, juce::JustifyLeft);

    g.drawText("True Peak Max", 20, y + 300, 180, 20, juce::JustifyLeft);
    g.drawText(formatNumber(truePeakMax) + " dBFS", 20, y + 325, 180, 20, juce::JustifyLeft);

    g.drawText("PSR (dB)", 20, y + 350, 180, 20, juce::JustifyLeft);
    g.drawText(formatNumber(psr) + " dB", 20, y + 375, 180, 20, juce::JustifyLeft);

    g.drawText("PLR (dB)", 20, y + 400, 180, 20, juce::JustifyLeft);
    g.drawText(formatNumber(plr) + " dB", 20, y + 425, 180, 20, juce::JustifyLeft);

    // Preset status
    g.setColour(juce::Colours::blue);
    juce::String presetText = "Current Preset: " + (currentPreset.isEmpty() ? "None" : currentPreset);
    g.drawText(presetText, 20, y + 445, getWidth() - 40, 20, juce::JustifyLeft);
}

void MeterPanelComponent::updateMetrics (
    double newMomentaryLUFS,
    double newShortTermLUFS,
    double newIntegratedLUFS,
    double newLRA,
    double newMomentaryMax,
    double newShortTermMax,
    double newTruePeakMax,
    double newPSR,
    double newPLR)
{
    momentaryLUFS = newMomentaryLUFS;
    shortTermLUFS = newShortTermLUFS;
    integratedLUFS = newIntegratedLUFS;
    lraLU = newLRA;
    momentaryMax = newMomentaryMax;
    shortTermMax = newShortTermMax;
    truePeakMax = newTruePeakMax;
    psr = newPSR;
    plr = newPLR;

    // Check limits and update colors (amber/red warnings)
    juce::Colour textColour = juce::Colours::white;
    if (momentaryLUFS > 10.0f) // High LUFS threshold
        textColour = juce::Colours::orange;
    if (momentaryLUFS > 12.0f) // High LUFS warning
        textColour = juce::Colours::red;

    /* Color logic would be applied here based on preset thresholds
       (not shown for brevity - would depend on loaded preset) */

    repaint();
}

void MeterPanelComponent::updateCurrentPreset (const juce::String& presetName)
{
    currentPreset = presetName;
    repaint();
}

void MeterPanelComponent::timerCallback()
{
    // This would typically be called with new data from DSP thread
    // For now, just maintain current state
    repaint();
}

/* Unused components - remove these empty functions */

void MeterPanelComponent::resized() { /* Implement layout */ }

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeterPanelComponent)