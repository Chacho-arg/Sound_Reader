// KWeightingFilter.cpp
#include "KWeightingFilter.h"

KWeightingFilter::KWeightingFilter(double sampleRate) : sampleRate(sampleRate)
{
    updateCoefficients();
    reset();
}

void KWeightingFilter::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    updateCoefficients();
    reset();
}

void KWeightingFilter::updateCoefficients()
{
    const double highShelfQ = std::sqrt(2.0) / 2.0;
    const double highPassQ = std::sqrt(2.0) / 2.0;
    
    highShelfCoeffs = *juce::dsp::IIR::Coefficients<float>::createHighShelf(
        sampleRate, highShelfFreq, highShelfQ, juce::dBToGain(highShelfGain));
    
    highPassCoeffs = *juce::dsp::IIR::Coefficients<float>::createHighPass(
        sampleRate, highPassFreq, highPassQ);
}

void KWeightingFilter::reset()
{
    for (int ch = 0; ch < maxChannels; ++ch)
    {
        channels[ch].highShelf.coefficients = highShelfCoeffs;
        channels[ch].highShelf.reset();
        channels[ch].highPass.coefficients = highPassCoeffs;
        channels[ch].highPass.reset();
    }
}

void KWeightingFilter::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = juce::jmin(buffer.getNumChannels(), maxChannels);
    const int numSamples = buffer.getNumSamples();
    
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* samples = buffer.getWritePointer(ch);
        auto& hs = channels[ch].highShelf;
        auto& hp = channels[ch].highPass;
        
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = samples[i];
            sample = hs.processSample(sample);
            sample = hp.processSample(sample);
            samples[i] = sample;
        }
    }
}