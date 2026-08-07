// PluginEditor.cpp
#include "PluginEditor.h"

PluginEditor::PluginEditor (SoundReaderAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor(p),
      presetSelectorBar(p.getTargetPresetManager()),
      meterPanel(p.getTargetPresetManager()),
      tonalCurve(p.getTargetPresetManager())
{
    setResizable(true, false);
    setSize(1000, 600);
    setOpaque(true);
    
    addAndMakeVisible(&presetSelectorBar);
    addAndMakeVisible(&meterPanel);
    addAndMakeVisible(&tonalCurve);
    
    presetSelectorBar.setSavePresetCallback([this](const juce::String& name)
    {
        saveCurrentPreset(name);
    });
    
    startTimerHz(60);
}

PluginEditor::~PluginEditor()
{
    stopTimer();
}

void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF1B222C));
}

void PluginEditor::resized()
{
    auto bounds = getLocalBounds();
    
    int headerHeight = 50;
    presetSelectorBar.setBounds(bounds.removeFromTop(headerHeight));
    
    int panelWidth = 320;
    meterPanel.setBounds(bounds.removeFromLeft(panelWidth));
    
    tonalCurve.setBounds(bounds);
}

void PluginEditor::timerCallback()
{
    updateMetrics();
}

void PluginEditor::updateMetrics()
{
    auto snap = processor.getMetricsSnapshot();
    
    meterPanel.updateMetrics(
        snap.momentaryLUFS,
        snap.shortTermLUFS,
        snap.integratedLUFS,
        snap.lraLU,
        snap.momentaryMax,
        snap.shortTermMax,
        snap.truePeakMax,
        snap.psr,
        snap.plr
    );
    
    tonalCurve.updateBandEnergies(
        snap.bandEnergies[0],
        snap.bandEnergies[1],
        snap.bandEnergies[2],
        snap.bandEnergies[3]
    );
    
    juce::String currentPreset = "None";
    auto& presetManager = processor.getTargetPresetManager();
    auto names = presetManager.getPresetNames();
    if (!names.empty())
        currentPreset = names[0];
    
    meterPanel.updateCurrentPreset(currentPreset);
    tonalCurve.updatePreset(currentPreset);
}

void PluginEditor::saveCurrentPreset(const juce::String& presetName)
{
    auto snap = processor.getMetricsSnapshot();
    auto& presetManager = processor.getTargetPresetManager();
    
    std::vector<PresetBand> bands = {
        {"Low", snap.bandEnergies[0], 3.0, 20.0, 250.0},
        {"LowMid", snap.bandEnergies[1], 2.5, 250.0, 2000.0},
        {"HighMid", snap.bandEnergies[2], 2.0, 2000.0, 8000.0},
        {"High", snap.bandEnergies[3], 2.0, 8000.0, 20000.0}
    };
    
    bool success = presetManager.addCustomPreset(
        presetName,
        "User-defined preset captured from live audio",
        snap.integratedLUFS,
        1.0,
        4.0,
        16.0,
        bands
    );
    
    if (success)
    {
        presetSelectorBar.populatePresetList();
        DBG("Saved preset: " + presetName);
    }
    else
    {
        DBG("Failed to save preset: " + presetName);
    }
}
