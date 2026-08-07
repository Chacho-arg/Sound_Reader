// MeterPanelComponent.h
#pragma once

#include <JuceHeader.h>
#include "../Presets/TargetPresetManager.h"

class MeterPanelComponent  : public juce::Component,
                           private juce::Timer
{
public:
    MeterPanelComponent (TargetPresetManager& presetManager);
    ~MeterPanelComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    // Update loudness metrics from DSP thread (called via atomic snapshots)
    void updateMetrics (double momentaryLUFS,
                        double shortTermLUFS,
                        double integratedLUFS,
                        double lraLU,
                        double momentaryMax,
                        double shortTermMax,
                        double truePeakMax,
                        double psr,
                        double plr);

    // Update current preset display
    void updateCurrentPreset (const juce::String& presetName);

private:
    // Metrics display
    juce::Label momentaryLabel;
    juce::Label shortTermLabel;
    juce::Label integratedLabel;
    juce::Label lraLabel;
    juce::Label momentaryMaxLabel;
    juce::Label shortTermMaxLabel;
    juce::Label truePeakMaxLabel;
    juce::Label psrLabel;
    juce::Label plrLabel;
    juce::Label presetLabel;

    // Current values
    double momentaryLUFS = -70.0;
    double shortTermLUFS = -70.0;
    double integratedLUFS = -70.0;
    double lraLU = 0.0;
    double momentaryMax = -70.0;
    double shortTermMax = -70.0;
    double truePeakMax = -100.0;
    double psr = 0.0;
    double plr = 0.0;
    juce::String currentPreset = "None";

    TargetPresetManager& presetManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeterPanelComponent)
};