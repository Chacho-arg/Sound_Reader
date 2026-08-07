// PluginEditor.h
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/MeterPanelComponent.h"
#include "UI/TonalCurveComponent.h"
#include "UI/PresetSelectorBar.h"

class PluginEditor  : public juce::AudioProcessorEditor,
                     private juce::Timer
{
public:
    PluginEditor (SoundReaderAudioProcessor& p);
    ~PluginEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateMetrics();
    void saveCurrentPreset(const juce::String& presetName);

    SoundReaderAudioProcessor& processor;
    
    PresetSelectorBar presetSelectorBar;
    MeterPanelComponent meterPanel;
    TonalCurveComponent tonalCurve;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
