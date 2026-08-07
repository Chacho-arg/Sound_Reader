// TruePeakDetectorTest.cpp
#include "TruePeakDetector.h"
#include "SlidingWindowLUFS.h"
#include <iostream>
#include <cmath>

void testSineWaveAtMinus12dBFS()
{
    const double sampleRate = 48000.0;
    const double frequency = 1000.0; // 1 kHz
    const double amplitude = std::pow(10.0, -12.0 / 20.0); // -12 dBFS

    std::cout << "=== Test: Sine wave at -12 dBFS ===" << std::endl;

    TruePeakDetector tp(sampleRate);
    tp.prepare(sampleRate);

    SlidingWindowLUFS lufs(sampleRate);
    lufs.prepare(sampleRate);

    const int numSamples = 48000; // 1 second
    double maxTruePeak = -100.0;

    for (int i = 0; i < numSamples; ++i)
    {
        double phase = 2.0 * M_PI * frequency * i / sampleRate;
        float sample = amplitude * std::sin(phase);

        // K-weighting
        // kw.processSample(sample); // (commented out for simplicity)

        // LUFS sliding windows
        lufs.addSample(sample);

        // True peak
        double tpVal = tp.processSample(sample);
        if (tpVal > maxTruePeak) maxTruePeak = tpVal;
    }

    std::cout << "True Peak (dBFS): " << maxTruePeak << " (expected: ~-12 dBTP)" << std::endl;
    std::cout << "Momentary LUFS: " << lufs.getMomentaryLUFS() << " (expected: ~-12 LUFS)" << std::endl;
    std::cout << "Short-Term LUFS: " << lufs.getShortTermLUFS() << " (expected: ~-12 LUFS)" << std::endl;
    std::cout << std::endl;
}

int main()
{
    testSineWaveAtMinus12dBFS();
    return 0;
}
