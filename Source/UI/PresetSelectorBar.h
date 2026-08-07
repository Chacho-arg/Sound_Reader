// PresetSelectorBar.h
#pragma once

#include <JuceHeader.h>
#include "../Presets/TargetPresetManager.h"

class PresetSelectorBar  : public juce::Component,
                          private juce::ComboBox::Listener,
                          private juce::Button::Listener
{
public:
    using SavePresetCallback = std::function<void(const juce::String& presetName)>;
    
    PresetSelectorBar (TargetPresetManager& presetManager);
    ~PresetSelectorBar() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    // Get currently selected preset name
    juce::String getCurrentPreset() const;
    
    void setSavePresetCallback(SavePresetCallback cb) { savePresetCallback = std::move(cb); }

private:
    // UI Components
    juce::ComboBox presetSelector;
    juce::TextButton resetButton;
    juce::TextButton saveButton;
    juce::TextButton settingsButton;

    TargetPresetManager& presetManager;
    SavePresetCallback savePresetCallback;

    // Callbacks
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;

    // Helper
    void populatePresetList();
    void loadPreset (const juce::String& presetName);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetSelectorBar)
};