// LoudnessEngine.h
#pragma once

#include <JuceHeader.h>
#include "KWeightingFilter.h"
#include <atomic>
#include <array>
#include <vector>
#include <cmath>

class LoudnessEngine
{
public:
    explicit LoudnessEngine(double sampleRate = 48000.0);
    
    void prepare(double sampleRate, int maxBlockSize);
    void reset();
    
    // Process audio block through K-weighting filter and compute loudness metrics
    void processBlock(const juce::AudioBuffer<float>& buffer);
    
    // Getters for loudness metrics (LUFS)
    double getMomentaryLUFS() const { return momentaryLUFS.load(); }
    double getShortTermLUFS() const { return shortTermLUFS.load(); }
    double getIntegratedLUFS() const { return integratedLUFS.load(); }
    double getLRA() const { return lraLU.load(); }
    double getMomentaryMax() const { return momentaryMax.load(); }
    double getShortTermMax() const { return shortTermMax.load(); }
    
    // Dynamic ratios
    double getPSR() const { return psr.load(); }
    double getPLR() const { return plr.load(); }
    
    // True Peak (dBTP)
    double getTruePeakMax() const { return truePeakMax.load(); }
    
    // Thread-safe snapshot for UI thread
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
    };
    
    MetricsSnapshot getSnapshot() const;

private:
    // K-weighting filter
    KWeightingFilter kWeightingFilter;
    
    // Sliding window buffers for LUFS calculation
    struct WindowData
    {
        static constexpr int momentaryMs = 400;
        static constexpr int shortTermMs = 3000;
        
        std::vector<float> ringBuffer;
        size_t writeIndex = 0;
        double sumSquares = 0.0;
        bool bufferFull = false;
        size_t samplesPerWindow = 0;
        
        void reset() { 
            std::fill(ringBuffer.begin(), ringBuffer.end(), 0.0f); 
            writeIndex = 0; 
            sumSquares = 0.0; 
            bufferFull = false; 
        }
    };
    
    // Momentary (400ms) and Short-Term (3s) windows
    WindowData momentaryWindow;
    WindowData shortTermWindow;
    
    // Integrated Loudness gating
    struct GatedLoudness
    {
        double sumSquares = 0.0;
        size_t sampleCount = 0;
        std::vector<float> historyBuffer; // stores block LUFS for LRA
        size_t historyIndex = 0;
        double absoluteThreshold = -70.0;
        double relativeThreshold = 0.0;
        double ungatedLevel = -70.0;
        
        void reset() { 
            sumSquares = 0.0; 
            sampleCount = 0; 
            ungatedLevel = -70.0; 
            relativeThreshold = -80.0; 
            std::fill(historyBuffer.begin(), historyBuffer.end(), -70.0f); 
        }
    } gatedLoudness;
    
    // True Peak detector with 4x oversampling
    class TruePeakDetector
    {
    public:
        explicit TruePeakDetector(double sampleRate);
        void prepare(double newSampleRate);
        void reset();
        void processBlock(const juce::AudioBuffer<float>& buffer);
        double getTruePeak() const { return truePeakMax.load(); }
        
    private:
        // 4x oversampling using polyphase FIR interpolation
        juce::dsp::Oversampling<float> oversampler;
        std::atomic<double> truePeakMax{ -100.0 };
        double sampleRate;
    };
    
    TruePeakDetector truePeakDetector;
    
    // Atomic metrics for lock-free UI access
    std::atomic<double> momentaryLUFS{ -70.0 };
    std::atomic<double> shortTermLUFS{ -70.0 };
    std::atomic<double> integratedLUFS{ -70.0 };
    std::atomic<double> lraLU{ 0.0 };
    std::atomic<double> momentaryMax{ -70.0 };
    std::atomic<double> shortTermMax{ -70.0 };
    std::atomic<double> psr{ 0.0 };
    std::atomic<double> plr{ 0.0 };
    
    // Helper functions
    double linearToLUFS(double power) const;
    double computeLRA();
    void updateIntegratedLoudness(double blockLUFS);
    void updateDynamicRatios();
    
    double sampleRate = 48000.0;
    int maxBlockSize = 512;
};