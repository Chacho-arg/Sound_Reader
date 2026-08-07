// ILicenseValidator.h
#pragma once

#include <JuceHeader.h>

class ILicenseValidator
{
public:
    virtual ~ILicenseValidator() = default;
    
    enum class ValidationResult
    {
        Valid,
        Invalid,
        TrialExpired,
        NetworkError
    };
    
    virtual ValidationResult validate() const = 0;
    virtual juce::String getLicenseeName() const = 0;
    virtual juce::String getExpiryDate() const = 0;
    virtual bool isTrialMode() const = 0;
};
