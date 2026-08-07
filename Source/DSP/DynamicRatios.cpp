// DynamicRatios.cpp
#include "DynamicRatios.h"

void DynamicRatios::update(double tp_dBFS, double st_lufs, double i_lufs)
{
    truePeak.store(tp_dBFS);
    shortTerm.store(st_lufs);
    integrated.store(i_lufs);
}

double DynamicRatios::getPSR() const
{
    // PSR = True Peak (dBFS) - Short-Term Loudness (LUFS)
    return truePeak.load() - shortTerm.load();
}

double DynamicRatios::getPLR() const
{
    // PLR = True Peak (dBFS) - Integrated Loudness (LUFS)
    return truePeak.load() - integrated.load();
}