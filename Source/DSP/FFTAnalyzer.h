// FFTAnalyzer.h
#pragma once
#include <JuceHeader.h>
#include <vector>
#include <cmath>

class FFTAnalyzer
{
public:
    FFTAnalyzer();
    
    void prepare(double sampleRate, int fftSize, int windowSize);
    void reset();
    
    // Run FFT on a buffer, return energy per band
    struct BandEnergy
    {
        double low;        // 20-250 Hz
        double lowMid;     // 250-2 kHz
        double highMid;    // 2-8 kHz
        double high;       // 8-20 kHz
    };
    
    BandEnergy computeBands(const float* buffer, int numSamples);
    
    // Apply Hann window and 75% overlap
    void processBlock(const float* input, float* output);

private:
    double sampleRate;
    int fftSize;
    int windowSize;
    
    std::vector<double> hannWindow;
    std::vector<double> buffer;
};