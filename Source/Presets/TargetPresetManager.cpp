// TargetPresetManager.cpp
#include "TargetPresetManager.h"
#include "../Presets/FactoryPresets.json"
#include <fstream>

TargetPresetManager::TargetPresetManager()
{
    userPresetsFile = juce::File::getCurrentWorkingDirectory()
                     .getChildFile("UserPresets.json");
}

void TargetPresetManager::parseFactoryPresets()
{
    juce::File factoryFile = juce::File::getCurrentWorkingDirectory()
                            .getChildFile("Source/Presets/FactoryPresets.json");
    
    if (!factoryFile.existsAsFile())
        return;
    
    juce::String jsonContent = factoryFile.loadFileAsString();
    juce::var parsed = juce::JSON::parse(jsonContent);
    
    if (!parsed.isObject())
        return;
    
    const juce::DynamicObject* root = parsed.getDynamicObject();
    const juce::var presetsVar = root->getProperty("presets");
    
    if (!presetsVar.isArray())
        return;
    
    for (const auto& presetVar : *presetsVar.getArray())
    {
        auto preset = PresetTarget::fromVar(presetVar);
        if (preset.has_value() && !preset->name.isEmpty())
            presets[preset->name] = *preset;
    }
}

bool TargetPresetManager::loadFactoryPresets()
{
    parseFactoryPresets();
    return !presets.empty();
}

bool TargetPresetManager::loadCustomPresets()
{
    if (!userPresetsFile.existsAsFile())
        return false;
    
    juce::String jsonContent = userPresetsFile.loadFileAsString();
    juce::var parsed = juce::JSON::parse(jsonContent);
    
    if (!parsed.isObject())
        return false;
    
    const juce::DynamicObject* root = parsed.getDynamicObject();
    const juce::var presetsVar = root->getProperty("presets");
    
    if (!presetsVar.isArray())
        return false;
    
    for (const auto& presetVar : *presetsVar.getArray())
    {
        auto preset = PresetTarget::fromVar(presetVar);
        if (preset.has_value() && !preset->name.isEmpty())
            presets[preset->name] = *preset;
    }
    
    return true;
}

bool TargetPresetManager::loadCustomPreset(const juce::File& file)
{
    if (!file.existsAsFile())
        return false;
    
    juce::String jsonContent = file.loadFileAsString();
    juce::var parsed = juce::JSON::parse(jsonContent);
    
    if (!parsed.isObject())
        return false;
    
    auto preset = PresetTarget::fromVar(parsed);
    if (preset.has_value() && !preset->name.isEmpty())
    {
        presets[preset->name] = *preset;
        return true;
    }
    
    return false;
}

bool TargetPresetManager::saveCustomPresets() const
{
    juce::var root = juce::createJsonObject();
    juce::var presetsArray = juce::createJsonArray();
    
    for (const auto& [name, preset] : presets)
    {
        presetsArray.add(preset.toVar());
    }
    
    root->setProperty("presets", presetsArray);
    root->setProperty("version", "1.0");
    
    return userPresetsFile.replaceWithText(root.asJson());
}

std::optional<PresetTarget> TargetPresetManager::getPreset(const juce::String& name) const
{
    auto it = presets.find(name);
    if (it != presets.end())
        return it->second;
    return std::nullopt;
}

std::vector<juce::String> TargetPresetManager::getPresetNames() const
{
    std::vector<juce::String> names;
    for (const auto& [name, preset] : presets)
        names.push_back(name);
    return names;
}

std::vector<PresetBand> TargetPresetManager::getTonalBandTargets(const juce::String& presetName) const
{
    auto preset = getPreset(presetName);
    if (preset.has_value())
        return preset->bands;
    return {};
}

bool TargetPresetManager::addCustomPreset(const juce::String& name,
                                          const juce::String& description,
                                          double targetLUFS,
                                          double toleranceLU,
                                          double plrMin,
                                          double plrMax,
                                          const std::vector<PresetBand>& bands)
{
    if (name.isEmpty() || presets.find(name) != presets.end())
        return false;
    
    PresetTarget newPreset;
    newPreset.name = name;
    newPreset.description = description;
    newPreset.targetLUFS = targetLUFS;
    newPreset.toleranceLU = toleranceLU;
    newPreset.plrMin = plrMin;
    newPreset.plrMax = plrMax;
    newPreset.bands = bands.empty() ? std::vector<PresetBand>{
        PresetBand{"Low", 0.0, 3.0, 20.0, 250.0},
        PresetBand{"LowMid", 0.0, 2.5, 250.0, 2000.0},
        PresetBand{"HighMid", 0.0, 2.0, 2000.0, 8000.0},
        PresetBand{"High", 0.0, 2.0, 8000.0, 20000.0}
    } : bands;
    
    presets[name] = newPreset;
    return saveCustomPresets();
}

bool TargetPresetManager::isLUFSWithinTarget(float lufs, const juce::String& presetName) const
{
    auto preset = getPreset(presetName);
    if (!preset.has_value()) return false;
    
    return lufs >= (preset->targetLUFS - preset->toleranceLU) &&
           lufs <= (preset->targetLUFS + preset->toleranceLU);
}

bool TargetPresetManager::isPLRWithinTarget(float plr, const juce::String& presetName) const
{
    auto preset = getPreset(presetName);
    if (!preset.has_value()) return false;
    
    return plr >= preset->plrMin && plr <= preset->plrMax;
}

bool TargetPresetManager::isTruePeakWithinTarget(float truePeak_dBFS, double targetLUFS)
{
    double maxAllowedTP = targetLUFS + 1.0;
    return truePeak_dBFS <= maxAllowedTP;
}

float TargetPresetManager::getTargetLUFS(const juce::String& presetName) const
{
    auto preset = getPreset(presetName);
    if (preset.has_value())
        return static_cast<float>(preset->targetLUFS);
    return -14.0f;
}

std::pair<float, float> TargetPresetManager::getPLRRange(const juce::String& presetName) const
{
    auto preset = getPreset(presetName);
    if (preset.has_value())
        return {static_cast<float>(preset->plrMin), static_cast<float>(preset->plrMax)};
    return {8.0f, 12.0f};
}
