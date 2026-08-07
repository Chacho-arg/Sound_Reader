// SlidingWindowLUFS.cpp
#include "SlidingWindowLUFS.h"

SlidingWindowLUFS::SlidingWindowLUFS(double sampleRate, size_t momentaryMs, size_t shortTermMs)
    : sampleRate(sampleRate)
    , m_momentaryWindowSize(static_cast<size_t>(momentaryMs * sampleRate))
    , m_shortTermWindowSize(static_cast<size_t>(shortTermMs * sampleRate))
{
    // FIX: Allocate fixed-size buffers at construction time (no dynamic allocation in process)
    m_momentarySamples.resize(m_momentaryWindowSize);
    m_shortTermSamples.resize(m_shortTermWindowSize);
}

void SlidingWindowLUFS::prepare(double sampleRate)
{
    this->sampleRate = sampleRate;
    m_momentaryWindowSize = static_cast<size_t>(400.0 * sampleRate);
    m_shortTermWindowSize = static_cast<size_t>(3000.0 * sampleRate);

    m_momentarySamples.resize(m_momentaryWindowSize, 0.0f);
    m_shortTermSamples.resize(m_shortTermWindowSize, 0.0f);

    m_momentaryHead = 0;
    m_momentaryTail = 0;
    m_momentarySumSquares = 0.0;
    m_momentaryWindowReady = false;

    m_shortTermHead = 0;
    m_shortTermTail = 0;
    m_shortTermSumSquares = 0.0;
    m_shortTermWindowReady = false;
}

void SlidingWindowLUFS::reset()
{
    m_momentaryHead = 0;
    m_momentaryTail = 0;
    m_momentarySumSquares = 0.0;
    m_momentaryWindowReady = false;

    m_shortTermHead = 0;
    m_shortTermTail = 0;
    m_shortTermSumSquares = 0.0;
    m_shortTermWindowReady = false;
}

void SlidingWindowLUFS::addSample(float sample)
{
    double power = static_cast<double>(sample) * static_cast<double>(sample);

    // Momentary window (400ms)
    m_momentarySumSquares += power;
    m_momentarySamples[m_momentaryTail] = sample;
    m_momentaryTail = (m_momentaryTail + 1) % m_momentaryWindowSize;
    if (m_momentaryTail == m_momentaryHead)
        m_momentaryWindowReady = true;
    m_momentaryWindowReady = true;

    // Short-term window (3s)
    m_shortTermSumSquares += power;
    m_shortTermSamples[m_shortTermTail] = sample;
    m_shortTermTail = (m_shortTermTail + 1) % m_shortTermWindowSize;
    if (m_shortTermTail == m_shortTermHead)
        m_shortTermWindowReady = true;
    m_shortTermWindowReady = true;
}

double SlidingWindowLUFS::toLUFS(double power) const
{
    double ref = 20.0 * std::log10(1.0); // -70 LUFS reference
    return 10.0 * std::log10(power / ref);
}

double SlidingWindowLUFS::getMomentaryLUFS() const
{
    if (!m_momentaryWindowReady) return 0.0;

    size_t count = m_momentaryWindowSize;
    // Compute mean power (RMS-like approximation)
    // Note: for a rectangular window, power = sum(power) / count
    double meanPower = m_momentarySumSquares / static_cast<double>(count);
    return toLUFS(meanPower);
}

double SlidingWindowLUFS::getShortTermLUFS() const
{
    if (!m_shortTermWindowReady) return 0.0;

    size_t count = m_shortTermWindowSize;
    double meanPower = m_shortTermSumSquares / static_cast<double>(count);
    return toLUFS(meanPower);
}

void SlidingWindowLUFS::updateMomentaryWindow(float sample)
{
    m_momentarySumSquares += static_cast<double>(sample) * static_cast<double>(sample);
    m_momentarySamples[m_momentaryTail] = sample;
    m_momentaryTail = (m_momentaryTail + 1) % m_momentaryWindowSize;
    if (m_momentaryTail == m_momentaryHead)
        m_momentaryWindowReady = true;
}

void SlidingWindowLUFS::updateShortTermWindow(float sample)
{
    m_shortTermSumSquares += static_cast<double>(sample) * static_cast<double>(sample);
    m_shortTermSamples[m_shortTermTail] = sample;
    m_shortTermTail = (m_shortTermTail + 1) % m_shortTermWindowSize;
    if (m_shortTermTail == m_shortTermHead)
        m_shortTermWindowReady = true;
}