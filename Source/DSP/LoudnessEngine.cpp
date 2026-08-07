#include "LoudnessEngine.h"
#include <algorithm>
#include <atomic>

// LoudnessEngine implementation
LoudnessEngine::LoudnessEngine(double sampleRate) : kWeightingFilter(sampleRate), truePeakDetector(sampleRate)
{
    prepareMethods = [](double sr, int bs) { /* fallback */ };
}

void LoudnessEngine::prepare(double sampleRate, int maxBlockSize)
{
    this->sampleRate = sampleRate;
    this->maxBlockSize = maxBlockSize;
    
    // Initialize sliding windows
    const int momentaryBufferSize = sampleRate * 0.4; // 400ms
    const int shortTermBufferSize = sampleRate * 3.0; // 3s
    
    momentaryWindow.reset();
    shortTermWindow.reset();
    
    momentaryWindow.ringBuffer.resize(momentaryBufferSize);
    shortTermWindow.ringBuffer.resize(shortTermBufferSize);
    
    momentaryWindow.writeIndex = 0;
    shortTermWindow.writeIndex = 0;
    
    gatedLoudness.reset();
    
    // Initialize true peak detector
    truePeakDetector.prepare(sampleRate);
    truePeakDetector.reset();
    
    // Initialize prepare callbacks
    prepareMethods = [this](double sr, int bs) {
        reset();
        momentaryWindow.reset();
        shortTermWindow.reset();
        gatedLoudness.reset();
        truePeakDetector.reset();
    };
}

void LoudnessEngine::reset()
{
    kWeightingFilter.reset();
    momentaryWindow.buffer400msSize = 0;
    shortTermWindow.buffer3sSize = 0;
    gatedLoudness.reset();
    truePeakDetector.reset();
}

void LoudnessEngine::processBlock(const juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() == 0) return;
    
    auto numChannels = buffer.getNumChannels();
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* samples = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            // Apply K-weighting filter
            float filteredSample = samples[i];
            samples[i] = kWeightingFilter.processSample(ch, samples[i]); // This would need to be adapted
        }
    }
    
    // Process each channel separately for thread safety considerations
    const int numChannels = buffer.getNumChannels();
    
    bool bufferFull_400ms = false;
    bufferFull_3s = shortTermWindow.bufferFull = false;
    
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* samples = buffer.getWritePointer(ch);
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float sample = samples[i];
            float filteredSample = sample;
            
            // Apply K-weighting filter (simplified - in practice would use actual filter instances)
            // This is a placeholder - actual implementation would use the KWeightingFilter properly
            filteredSample = sample * 1.414f; // Approximate K-weighting effect
            
            // Update squared power sums
            float power = filteredSample * filteredSample;
            
            // Store filtered sample in momentary window buffer
            size_t idx400 = momentaryWindow.writeIndex;
            momentaryWindow.ringBuffer[idx400] = filteredSample;
            momentaryWindow.sumSquares += power;
            
            // Update momentary statistics
            momentaryWindow.writeIndex = (momentaryWindow.writeIndex + 1) % momentaryWindow.ringBuffer.size();
            if (momentaryWindow.writeIndex == 0) momentaryWindow.bufferFull = true;
            
            // Similarly for short term buffer
            size_t idx3 = shortTermWindow.writeIndex;
            shortTermWindow.ringBuffer[idx3] = filteredSample;
            shortTermWindow.sumSquares += power;
            
            shortTermWindow.writeIndex = (shortTermWindow.writeIndex + 1) % shortTermWindow.ringBuffer.size();
            if (shortTermWindow.writeIndex == 0) shortTermWindow.bufferFull = true;
            
            // Update momentary max
            if (filteredSample > momentaryMax) 
                momentaryMax.store(filteredSample);
            
            // Update short term max
            if (filteredSample > shortTermMax) 
                shortTermMax.store(filteredSample);
            
            // Gated integrated loudness processing
            float sampleLUS = 20.0f * std::log10f(filteredSample);
            samples[i] = filteredSample; // Store processed sample back
        }
    }
    
    // Calculate LUFS values periodically or on demand
    // This would normally be done on a regular timer or separate thread for performance
}