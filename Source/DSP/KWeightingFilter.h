// KWeightingFilter.h
#pragma once

#include <JuceHeader.h>

class KWeightingFilter
{
public:
    explicit KWeightingFilter(double sampleRate = 48000.0);
    
    void prepare(double newSampleRate);
    void reset();
    void process(juce::AudioBuffer<float>& buffer);
    
    static constexpr double highShelfFreq = 1681.0;
    static constexpr double highShelfGain = 4.0;
    static constexpr double highPassFreq = 38.135;

private:
    static constexpr int maxChannels = 2;
    
    struct ChannelFilters
    {
        juce::dsp::IIR::Filter<float> highShelf;
        juce::dsp::IIR::Filter<float> highPass;
    };
    
    ChannelFilters channels[maxChannels];
    juce::dsp::IIR::Coefficients<float> highShelfCoeffs;
    juce::dsp::IIR::Coefficients<float> highPassCoeffs;
    double sampleRate;
    
    void updateCoefficients();
};