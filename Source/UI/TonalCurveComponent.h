// TonalCurveComponent.h
#pragma once

#include <JuceHeader.h>
#include "../Presets/TargetPresetManager.h"

class TonalCurveComponent  : public juce::Component,
                           private juce::Timer
{
public:
    TonalCurveComponent (TargetPresetManager& presetManager);
    ~TonalCurveComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    // Update with real-time band energies from DSP (4 bands)
    void updateBandEnergies (double low, double lowMid, double highMid, double high);

    // Update target preset for envelope display
    void updatePreset (const juce::String& presetName);

private:
    // Real-time band energies (from DSP FFT)
    double currentEnergies[4] = {0.0, 0.0, 0.0, 0.0};

    // Target envelope for current preset
    struct BandTarget
    {
        double target = 0.0;
        double tolerance = 0.0;
        juce::String name;
        double minFreq = 0.0;
        double maxFreq = 0.0;
    };
    std::vector<BandTarget> bandTargets;

    // Visual constants
    static constexpr int margin = 40;
    static constexpr int minFreq = 20;
    static constexpr int maxFreq = 20000;

    TargetPresetManager& presetManager;

    // Helper functions
    int freqToX (double freq) const;
    double energyToY (double energy) const;
    juce::Path createEnvelopePath() const;
    juce::Path createEnergyPath() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TonalCurveComponent)
};