// DummyLicenseValidator.cpp
#include "DummyLicenseValidator.h"

ILicenseValidator::ValidationResult DummyLicenseValidator::validate() const
{
    return ValidationResult::Valid;
}

juce::String DummyLicenseValidator::getLicenseeName() const
{
    return "MVP User";
}

juce::String DummyLicenseValidator::getExpiryDate() const
{
    return "2099-12-31";
}

bool DummyLicenseValidator::isTrialMode() const
{
    return false;
}
