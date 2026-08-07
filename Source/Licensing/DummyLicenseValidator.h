// DummyLicenseValidator.h
#pragma once

#include "ILicenseValidator.h"

class DummyLicenseValidator : public ILicenseValidator
{
public:
    ValidationResult validate() const override;
    juce::String getLicenseeName() const override;
    juce::String getExpiryDate() const override;
    bool isTrialMode() const override;
};
