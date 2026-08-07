// IntegratedLoudness.cpp
#include "IntegratedLoudness.h"
#include <algorithm>
#include <cmath>

IntegratedLoudness::IntegratedLoudness(double sampleRate)
    : sampleRate(sampleRate)
{
    // Initialize with long enough history for LRA (1 hour of 400ms blocks)
    size_t blocksPerSecond = sampleRate / 400.0; // 120 at 48kHz
    size_t historySize = blocksPerSecond * 3600; // 1 hour
    blockHistory.reserve(historySize);
}

void IntegratedLoudness::prepare(double sampleRate)
{
    this->sampleRate = sampleRate;
    reset();
}

void IntegratedLoudness::reset()
{
    ungatedIntegrated = 0.0;
    gatedIntegrated = 0.0;
    lraValue = 0.0;
    gatedEnergySum = 0.0;
    gatedSampleCount = 0;
    totalSampleCount = 0;
    blockHistory.clear();
    blockIndex = 0;
}

void IntegratedLoudness::processBlock(const float* samples, size_t numSamples, double momentaryLUFS)
{
    // Convert momentary LUFS back to linear energy
    double blockPower = std::pow(10.0, momentaryLUFS / 10.0);
    double blockEnergy = blockPower * static_cast<double>(numSamples);
    
    // Add to total energy (ungated)
    gatedEnergySum += blockEnergy;
    gatedSampleCount += numSamples;
    totalSampleCount += numSamples;
    
    // Record block LUFS for LRA calculation
    updateBlockHistory(momentaryLUFS);
    
    // For next iteration - we'll need the integrated value to compute relative threshold
    // This will be handled when getIntegratedLUFS() is called
}

double IntegratedLoudness::getIntegratedLUFS() const
{
    // First pass: get un-gated integrated loudness
    double unGatedLUFS = -10.0 * std::log10(gatedEnergySum / static_cast<double>(gatedSampleCount));
    
    // Second pass: apply gating - compute relative threshold
    double relativeThreshold = unGatedLUFS + relativeThresholdOffset;
    double effectiveThreshold = std::max(absoluteThreshold, relativeThreshold);
    
    // Third pass: re-calculate with only samples above effective threshold
    // Since we don't store individual blocks, we approximate
    // In practice, we would need to keep track of which blocks were above threshold
    double gatedLUFS = unGatedLUFS; // placeholder - would need actual gated calculation
    
    return gatedLUFS;
}

double IntegratedLoudness::getLRA() const
{
    if (blockHistory.size() < 2) return 0.0;
    return calculateLRA();
}

void IntegratedLoudness::updateBlockHistory(double blockLUFS)
{
    if (blockIndex >= blockHistory.size())
    {
        blockHistory.push_back(blockLUFS);
    }
    else
    {
        blockHistory[blockIndex] = blockLUFS;
    }
    blockIndex = (blockIndex + 1) % blockHistory.size();
}

double IntegratedLoudness::calculatePercentile(const std::vector<double>& data, double percentile) const
{
    if (data.empty()) return 0.0;
    
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    
    size_t index = static_cast<size_t>(std::ceil(percentile / 100.0 * sorted.size()) - 1);
    index = std::min(index, sorted.size() - 1);
    
    return sorted[index];
}

void IntegratedLoudness::calculateLRA()
{
    // LRA is difference between 95th and 10th percentile of gated loudness
    double low = calculatePercentile(blockHistory, lowPercentile);
    double high = calculatePercentile(blockHistory, highPercentile);
    lraValue = high - low;
}