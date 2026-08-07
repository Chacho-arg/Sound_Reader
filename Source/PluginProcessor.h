// PluginProcessor.h
#pragma once

#include <JuceHeader.h>
#include "DSP/KWeightingFilter.h"
#include "DSP/SlidingWindowLUFS.h"
#include "DSP/IntegratedLoudness.h"
#include "DSP/TruePeakDetector.h"
#include "DSP/DynamicRatios.h"
#include "DSP/FFTAnalyzer.h"
#include "Presets/TargetPresetManager.h"
#include "Licensing/ILicenseValidator.h"
#include "Licensing/DummyLicenseValidator.h"

class SoundReaderAudioProcessor  : public juce::AudioProcessor
{
public:
    SoundReaderAudioProcessor();
    ~SoundReaderAudioProcessor() override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiInputAvailable() const override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    double getTailTime() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    // Public access to DSP components for UI
    SlidingWindowLUFS& getSlidingWindowLUFS() { return slidingWindowLUFS; }
    IntegratedLoudness& getIntegratedLoudness() { return integratedLoudness; }
    TruePeakDetector& getTruePeakDetector() { return truePeakDetector; }
    DynamicRatios& getDynamicRatios() { return dynamicRatios; }
    FFTAnalyzer& getFFTAnalyzer() { return fftAnalyzer; }
    TargetPresetManager& getTargetPresetManager() { return targetPresetManager; }
    KWeightingFilter& getKWeightingFilter() { return kWeightingFilter; }
    
    // License validation
    ILicenseValidator::ValidationResult getLicenseValidationResult() const { return licenseResult; }
    juce::String getLicenseeName() const { return licenseeName; }

    // Metrics snapshot for UI thread
    struct MetricsSnapshot
    {
        double momentaryLUFS = -70.0;
        double shortTermLUFS = -70.0;
        double integratedLUFS = -70.0;
        double lraLU = 0.0;
        double momentaryMax = -70.0;
        double shortTermMax = -70.0;
        double truePeakMax = -100.0;
        double psr = 0.0;
        double plr = 0.0;
        double bandEnergies[4] = {0.0, 0.0, 0.0, 0.0};
    };
    
    MetricsSnapshot getMetricsSnapshot() const;

private:
    // DSP Components
    KWeightingFilter kWeightingFilter;
    SlidingWindowLUFS slidingWindowLUFS;
    IntegratedLoudness integratedLoudness;
    TruePeakDetector truePeakDetector;
    DynamicRatios dynamicRatios;
    FFTAnalyzer fftAnalyzer;
    TargetPresetManager targetPresetManager;
    
    std::unique_ptr<ILicenseValidator> licenseValidator;
    ILicenseValidator::ValidationResult licenseResult{ ILicenseValidator::ValidationResult::Valid };
    juce::String licenseeName;

    juce::AudioProcessorValueTreeState mstate;
    
    // Metrics atomics for lock-free UI access
    std::atomic<double> momentaryLUFS{ -70.0 };
    std::atomic<double> shortTermLUFS{ -70.0 };
    std::atomic<double> integratedLUFS{ -70.0 };
    std::atomic<double> lraLU{ 0.0 };
    std::atomic<double> momentaryMax{ -70.0 };
    std::atomic<double> shortTermMax{ -70.0 };
    std::atomic<double> truePeakMax{ -100.0 };
    std::atomic<double> psr{ 0.0 };
    std::atomic<double> plr{ 0.0 };
    std::atomic<double> bandEnergies[4]{ 0.0, 0.0, 0.0, 0.0 };

    // Process audio with DSP chain
    void processAudioBlock (juce::AudioBuffer<float>& buffer);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundReaderAudioProcessor)
};