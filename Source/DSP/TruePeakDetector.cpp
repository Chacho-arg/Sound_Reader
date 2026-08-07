#include "TruePeakDetector.h"

TruePeakDetector::TruePeakDetector(double sr)
    : sampleRate(sr)
    , oversampler(2, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR)
{
    oversampler.reset();
}

void TruePeakDetector::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    oversampler.reset();
}

void TruePeakDetector::reset()
{
    peakHoldingValue = 0.0;
    oversampler.reset();
    truePeak_dBFS.store(-100.0);
}

double TruePeakDetector::processSample(float sample)
{
    if (!std::isfinite(sample)) return truePeak_dBFS.load();

    juce::AudioBuffer<float> tempBuffer;
    tempBuffer.setSize(1, 1);
    tempBuffer.setSample(0, 0, sample);

    auto* upsampled = oversampler.processSamplesUpsampling(tempBuffer);
    const int upsampledSamples = upsampled.getNumSamples();

    double localPeak = truePeak_dBFS.load();
    for (int i = 0; i < upsampledSamples; ++i)
    {
        float upSampled = upsampled.getSample(0, i);
        if (std::abs(upSampled) > std::abs(peakHoldingValue))
        {
            peakHoldingValue = upSampled;
            localPeak = juce::jmap(std::abs(upSampled), 0.0f, 1.0f, -100.0, 0.0);
            truePeak_dBFS.store(localPeak);
        }
        else
        {
            peakHoldingValue *= 0.999f;
        }
    }
    return localPeak;
}

double TruePeakDetector::getTruePeak() const
{
    return truePeak_dBFS.load();
}