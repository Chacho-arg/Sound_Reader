// SlidingWindowLUFS.h
#pragma once
#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>

class SlidingWindowLUFS
{
public:
    // momentaryMs: 400ms, shortTermMs: 3000ms per ITU-R BS.1770-4
    explicit SlidingWindowLUFS(double sampleRate, size_t momentaryMs = 400, size_t shortTermMs = 3000);

    void prepare(double sampleRate);
    void reset();

    void addSample(float sample);

    double getMomentaryLUFS() const;
    double getShortTermLUFS() const;

    double toLUFS(double power) const;

    size_t getMomentaryWindowSize() const { return m_momentaryWindowSize; }
    size_t getShortTermWindowSize() const { return m_shortTermWindowSize; }
    bool isMomentaryWindowReady() const { return m_momentaryWindowReady; }
    bool isShortTermWindowReady() const { return m_shortTermWindowReady; }

private:
    void updateMomentaryWindow(float sample);
    void updateShortTermWindow(float sample);

    double sampleRate;
    size_t m_momentaryWindowSize;
    size_t m_shortTermWindowSize;

    std::vector<float> m_momentarySamples;
    std::vector<float> m_shortTermSamples;
    size_t m_momentaryHead = 0;
    size_t m_momentaryTail = 0;
    double m_momentarySumSquares = 0.0;
    bool m_momentaryWindowReady = false;

    size_t m_shortTermHead = 0;
    size_t m_shortTermTail = 0;
    double m_shortTermSumSquares = 0.0;
    bool m_shortTermWindowReady = false;

    static constexpr size_t MAX_SAMPLES = 480000; // ~3s at 48kHz
};