// FFTAnalyzer.cpp
#include "FFTAnalyzer.h"
#include <algorithm>

FFTAnalyzer::FFTAnalyzer()
{
}

void FFTAnalyzer::prepare(double sampleRate, int fftSize, int windowSize)
{
    this->sampleRate = sampleRate;
    this->fftSize = fftSize;
    this->windowSize = windowSize;
    
    // Precompute Hann window
    hannWindow.resize(fftSize);
    for (int i = 0; i < fftSize; ++i)
    {
        double windowFactor = 0.5 * (1.0 - std::cos(2.0 * M_PI * i / (fftSize - 1)));
        hannWindow[i] = windowFactor;
    }
}

void FFTAnalyzer::reset()
{
    // Clear any internal state
    hannWindow.clear();
    buffer.clear();
}

FFTAnalyzer::BandEnergy FFTAnalyzer::computeBands(const float* input, int numSamples)
{
    FFTAnalyzer::BandEnergy bands = {0.0, 0.0, 0.0, 0.0};
    
    if (fftSize == 0 || hannWindow.empty())
        return bands;
    
    int processSize = juce::jmin(numSamples, fftSize);
    if (processSize <= 0)
        return bands;
    
    std::vector<double> windowed(fftSize, 0.0);
    for (int i = 0; i < processSize; ++i)
        windowed[i] = static_cast<double>(input[i]) * hannWindow[i];
    
    juce::dsp::FFT fft(fftSize);
    fft.performForward(windowed.data());
    
    int hop = fftSize / 4;
    for (int band = 0; band < 4; ++band)
    {
        double bandEnergy = 0.0;
        int startIdx = band * hop;
        int endIdx = juce::jmin((band + 1) * hop, fftSize / 2);
        
        for (int i = startIdx; i < endIdx; ++i)
        {
            double real = fft.getRealPart(i);
            double imag = fft.getImaginaryPart(i);
            double magnitude = std::sqrt(real * real + imag * imag);
            bandEnergy += magnitude * magnitude;
        }
        
        if (band == 0) bands.low += bandEnergy;
        else if (band == 1) bands.lowMid += bandEnergy;
        else if (band == 2) bands.highMid += bandEnergy;
        else bands.high += bandEnergy;
    }
    
    return bands;
}

void FFTAnalyzer::processBlock(const float* input, float* output)
{
    // Copy input to internal buffer
    for (int i = 0; i < fftSize; ++i)
    {
        buffer[i] = static_cast<double>(input[i]);
    }
    
    // Apply Hann window
    for (int i = 0; i < fftSize; ++i)
    {
        output[i] = buffer[i] * hannWindow[i];
    }
}