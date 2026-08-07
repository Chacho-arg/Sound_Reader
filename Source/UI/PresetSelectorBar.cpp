// PresetSelectorBar.cpp
#include "PresetSelectorBar.h"

PresetSelectorBar::PresetSelectorBar(TargetPresetManager& manager) : presetManager(manager) 
{
    addAndMakeVisible(&presetSelector);
    presetSelector.addListener(this);
    populatePresetList();
}

void PresetSelectorBar::buttonClicked(juce::Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == &resetButton)
    {
        // Reset metrics - to be implemented
    }
    else if (buttonThatWasClicked == &saveButton)
    {
        juce::String newPresetName = "Custom " + juce::String(presets.size() + 1);
        
        if (savePresetCallback)
            savePresetCallback(newPresetName);
    }
}

void PresetSelectorBar::populatePresetList()
{
    presetSelector.clear();
    std::vector<juce::String> names = presetManager.getPresetNames();
    for (const auto& name : names)
    {
        if (!name.isEmpty())
            presetSelector.addItem(name, -1);
    }
}