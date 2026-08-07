// DynamicRatios.h
#pragma once

#include <atomic>

class DynamicRatios
{
public:
    DynamicRatios();
    
    // Update with current measurements (in dBFS/LUFS)
    void update(double truePeak_dBFS, double shortTermLUFS, double integratedLUFS);
    
    // PSR = True Peak (dBFS) - Short-Term Loudness (LUFS)
    double getPSR() const;
    
    // PLR = True Peak (dBFS) - Integrated Loudness (LUFS)
    double getPLR() const;

private:
    std::atomic<double> truePeak{ -100.0 };
    std::atomic<double> shortTerm{ -70.0 };
    std::atomic<double> integrated{ -70.0 };
};