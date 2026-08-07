#pragma once

#include <JuceHeader.h>

class TruePeakDetector
{
public:
    explicit TruePeakDetector (double sampleRate);
    void prepare (double newSampleRate);
    void reset();
    double processSample (float sample);
    double getTruePeak() const;

private:
    double sampleRate;
    double peakHoldingValue = 0.0;
    juce::dsp::Oversampling<float> oversampler;
    std::atomic<double> truePeak_dBFS{-100.0};

    void generateHighOrderCoefficients (juce::dsp::Oversampling<float>& interpolator,
                                        juce::dsp::OversamplingCoefficients<float>& coeffs,
                                        double upSampleRatio);

    void upsample4x (const float* source, int numSamples,
                     juce::dsp::Array<float>& dest);

    juce::dsp::OversamplingCoefficients<float> getCoefficients (int order,
duplicatePreviousValueIfOddIndex) const;
};