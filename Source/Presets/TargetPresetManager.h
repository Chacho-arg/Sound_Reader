// TargetPresetManager.h
#pragma once

#include <JuceHeader.h>
#include <map>
#include <optional>
#include <vector>
#include <string>

struct PresetBand
{
    juce::String name;
    double target = 0.0;
    double tolerance = 2.0;
    double minFreq = 20.0;
    double maxFreq = 20000.0;
    
    static std::optional<PresetBand> fromVar (const juce::var& json)
    {
        if (!json.isObject()) return std::nullopt;
        
        const juce::DynamicObject* obj = json.getDynamicObject();
        PresetBand band;
        
        band.name = obj->getProperty("name").toString();
        band.target = static_cast<double>(obj->getProperty("target"));
        band.tolerance = static_cast<double>(obj->getProperty("tolerance"));
        band.minFreq = static_cast<double>(obj->getProperty("minFreq"));
        band.maxFreq = static_cast<double>(obj->getProperty("maxFreq"));
        
        return band;
    }
    
    juce::var toVar() const
    {
        juce::var obj = juce::createJsonObject();
        obj->setProperty("name", name);
        obj->setProperty("target", target);
        obj->setProperty("tolerance", tolerance);
        obj->setProperty("minFreq", minFreq);
        obj->setProperty("maxFreq", maxFreq);
        return obj;
    }
};

struct PresetTarget
{
    juce::String name;
    juce::String description;
    double targetLUFS = -14.0;
    double toleranceLU = 1.0;
    double plrMin = 8.0;
    double plrMax = 12.0;
    std::vector<PresetBand> bands;
    
    static std::optional<PresetTarget> fromVar (const juce::var& json)
    {
        if (!json.isObject()) return std::nullopt;
        
        const juce::DynamicObject* obj = json.getDynamicObject();
        PresetTarget target;
        
        target.name = obj->getProperty("name").toString();
        target.description = obj->getProperty("description").toString();
        
        const juce::var loudnessVar = obj->getProperty("loudness");
        if (loudnessVar.isObject())
        {
            const juce::DynamicObject* loudObj = loudnessVar.getDynamicObject();
            target.targetLUFS = static_cast<double>(loudObj->getProperty("targetLUFS"));
            target.toleranceLU = static_cast<double>(loudObj->getProperty("toleranceLU"));
            
            const juce::var plrRangeVar = loudObj->getProperty("plrRange");
            if (plrRangeVar.isObject())
            {
                const juce::DynamicObject* pr = plrRangeVar.getDynamicObject();
                target.plrMin = static_cast<double>(pr->getProperty("min"));
                target.plrMax = static_cast<double>(pr->getProperty("max"));
            }
        }
        
        const juce::var tonalVar = obj->getProperty("tonalBalance");
        if (tonalVar.isObject())
        {
            const juce::DynamicObject* tonalObj = tonalVar.getDynamicObject();
            const juce::var bandsVar = tonalObj->getProperty("bands");
            
            if (bandsVar.isArray())
            {
                for (const auto& band : *bandsVar.getArray())
                {
                    if (band.isObject())
                    {
                        auto parsedBand = PresetBand::fromVar(band);
                        if (parsedBand.has_value())
                            target.bands.push_back(*parsedBand);
                    }
                }
            }
        }
        
        return target;
    }
    
    juce::var toVar() const
    {
        juce::var obj = juce::createJsonObject();
        obj->setProperty("name", name);
        obj->setProperty("description", description);
        
        juce::var loudnessObj = juce::createJsonObject();
        loudnessObj->setProperty("targetLUFS", targetLUFS);
        loudnessObj->setProperty("toleranceLU", toleranceLU);
        
        juce::var plrObj = juce::createJsonObject();
        plrObj->setProperty("min", plrMin);
        plrObj->setProperty("max", plrMax);
        loudnessObj->setProperty("plrRange", plrObj);
        
        obj->setProperty("loudness", loudnessObj);
        
        juce::var tonalObj = juce::createJsonObject();
        juce::var bandsArray = juce::createJsonArray();
        for (const auto& band : bands)
            bandsArray.add(band.toVar());
        tonalObj->setProperty("bands", bandsArray);
        obj->setProperty("tonalBalance", tonalObj);
        
        return obj;
    }
};

class TargetPresetManager
{
public:
    TargetPresetManager();
    
    bool loadFactoryPresets();
    bool loadCustomPresets();
    bool loadCustomPreset(const juce::File& file);
    bool saveCustomPresets();
    
    std::optional<PresetTarget> getPreset(const juce::String& name) const;
    std::vector<juce::String> getPresetNames() const;
    std::vector<PresetBand> getTonalBandTargets(const juce::String& presetName) const;
    
    bool addCustomPreset(const juce::String& name,
                        const juce::String& description,
                        double targetLUFS,
                        double toleranceLU,
                        double plrMin,
                        double plrMax,
                        const std::vector<PresetBand>& bands);
    
    bool isLUFSWithinTarget(float lufs, const juce::String& presetName) const;
    bool isPLRWithinTarget(float plr, const juce::String& presetName) const;
    bool isTruePeakWithinTarget(float truePeak_dBFS, double targetLUFS) const;
    
    float getTargetLUFS(const juce::String& presetName) const;
    std::pair<float, float> getPLRRange(const juce::String& presetName) const;
    
private:
    std::map<juce::String, PresetTarget> presets;
    juce::File userPresetsFile;
    
    void parseFactoryPresets();
};