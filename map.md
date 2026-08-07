# Project Map

## Directory Structure

```
Sound_Reader/
├── CONSTITUTION.md
├── CMakeLists.txt
├── map.md
├── docs/
│   └── specs/
│       ├── 01_REQUIREMENTS.md
│       ├── 02_ARCHITECTURE.md
│       ├── 03_DSP_SPECIFICATION.md
│       ├── 04_UI_UX_SPECIFICATION.md
│       └── tasks.md
└── Source/
    ├── DSP/
    │   ├── LoudnessEngine/          # ITU-R BS.1770-4, True Peak, LRA, PSR/PLR
    │   ├── TonalBalanceEngine/      # FFT Windowing, Psychoacoustic Band Energy Aggregator
    │   ├── TargetPresetManager/     # JSON parser & curve interpolator
    │   └── KWeightingFilter/        # High-Shelf + High-Pass K-weighting chain
    ├── UI/
    │   ├── MeterPanelComponent/     # Left panel: Short Term, Integrated, LRA, PSR, PLR, True Peak
    │   ├── TonalCurveComponent/     # Right canvas: 4-band target envelope & spectral trace
    │   ├── PresetSelectorBar/       # Header toolbar with dropdown, reset, settings
    │   └── MainLayoutContainer/     # Flex/grid responsive layout (1000x600 default)
    ├── Licensing/
    │   ├── ILicenseValidator/       # Pure virtual interface for license abstraction
    │   └── DummyLicenseValidator/   # MVP implementation (always returns valid)
    └── Presets/
        ├── FactoryPresets/          # Online Broadcast, FM Broadcast, EDM, Pop, Rock, Acoustic
        └── CustomPresets/           # User saved .json target profiles
```

## Architectural Modules (from ARCHITECTURE.md)

```
AppRoot
├── DSP Module
│   ├── LoudnessEngine (ITU-R BS.1770-4, True Peak, LRA, PSR/PLR)
│   ├── TonalBalanceEngine (FFT Windowing, Psychoacoustic Band Energy Aggregator)
│   └── TargetPresetManager (JSON parser & curve interpolator)
├── Licensing Module
│   └── ILicenseValidator (Interface -> DummyValidator [MVP], CloudValidator [v1.0])
└── UI Module
    ├── MeterPanelComponent (Left Column - Youlean style metrics)
    ├── TonalCurveComponent (Main Canvas - Ozone style target envelope)
    └── PresetSelectorBar (Top/Header Toolbar)
```

## Document Specifications

- **CONSTITUTION.md**: Core principles, hard rules (audio thread hygiene, licensing abstraction, DoD)
- **01_REQUIREMENTS.md**: Vision, platforms, feature set (loudness + tonal balance), non-functional reqs
- **02_ARCHITECTURE.md**: Tech stack, threading model, class blueprint, CI/CD pipeline
- **03_DSP_SPECIFICATION.md**: K-weighting filter chain, meter formulas, FFT processing, 4-band energy, crest envelopes
- **04_UI_UX_SPECIFICATION.md**: Layout grid (1000x600), color palette, target range indicators
- **tasks.md**: 5-phase executable task breakdown with status tracking