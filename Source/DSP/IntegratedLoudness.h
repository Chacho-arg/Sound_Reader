// IntegratedLoudness.h
#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <atomic>

class IntegratedLoudness
{
public:
    IntegratedLoudness(double sampleRate);
    
    void prepare(double sampleRate);
    void reset();
    
    // Process a block of K-weighted samples and update integrated loudness
    void processBlock(const float* samples, size_t numSamples, double momentaryLUFS);
    
    // Get integrated loudness in LUFS (gated)
    double getIntegratedLUFS() const;
    
    // Get Loudness Range (LRA) in LU
    double getLRA() const;
    
    // Threshold values per ITU-R BS.1770-4
    static constexpr double absoluteThreshold = -70.0; // dB (LUFS value)
    static constexpr double relativeThresholdOffset = -10.0; // dB below ungated level

    struct LRAWindow
    {
        std::vector<double> quietTimes;    // Block values below -70 LUFS after gating
        std::vector<double> loudTimes;     // Block values above ungated - 10 dB
    };

    struct GatedSample
    {
        double energy = 0.0;
        bool passesGate = false;
    };

private:
    double sampleRate;
    double ungatedIntegrated = 0.0;
    double gatedIntegrated = 0.0;
    double lraValue = 0.0;
    
    // History of processed block LUFS values for LRA
    std::vector<double> blockHistory;
    size_t blockIndex = 0;
    size_t lraWindowSize = 3600; // 1 hour of 400ms blocks = 9,000,000 samples at 48kHz
                                        // but for practical purposes, use a rolling window
    
    // Statistics
    double gatedEnergySum = 0.0;
    size_t gatedSampleCount = 0;
    size_t totalSampleCount = 0;
    
    // LRA calculation windows (95th and 10th percentile)
    double lowPercentile = 10.0;
    double highPercentile = 95.0;

    void updateBlockHistory(double blockLUFS);
    double calculatePercentile(const std::vector<double>& data, double percentile) const;
    void calculateLRA();
};