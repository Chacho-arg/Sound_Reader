// TestTargetPresetManager.cpp
#include "../Source/Presets/TargetPresetManager.h"
#include "juce_core/juce_core.h"

void runTests()
{
    TargetPresetManager manager;
    
    std::vector<TargetPresetManager::PresetBand> bands = {
        {"Low", 0.0, 3.0, 20.0, 250.0},
        {"LowMid", 0.0, 2.5, 250.0, 2000.0},
        {"HighMid", 0.0, 2.0, 2000.0, 8000.0},
        {"High", 0.0, 2.0, 8000.0, 20000.0}
    };
    
    bool success = manager.addCustomPreset(
        "TestPreset",
        "Test Preset",
        -14.0,
        1.0,
        8.0,
        12.0,
        bands
    );
    
    juce::String loadedName = manager.getPresetNames()[0];
    
    if (success && juce::String(manager.getPresetNames()[0]) == "TestPreset") 
    {
        DBG("Unit test PASSED: Preset creation and retrieval");
    }
    else
    {
        DBG("Unit test FAILED");
    }
}

int main()
{
    runTests();
    return 0;
}