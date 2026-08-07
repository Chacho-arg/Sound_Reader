# 02_ARCHITECTURE.md - System & Pipeline Architecture

## 1. Tech Stack
- **Framework**: JUCE 7+ (C++20 standard).
- **Audio API Abstractions**: CoreAudio (macOS), WASAPI / ASIO (Windows).
- **Build System**: CMake (v3.22+) integrated with JUCE CMake API.
- **CI/CD Pipeline**: GitHub Actions generating signed installers (`.dmg` for macOS, `.exe` via Inno Setup or WiX for Windows).

---

## 2. Core Threading Model
To ensure absolute stability and zero audio dropouts during heavy UI repaints:
[ Audio Thread (Priority 1) ]
│
▼ (Lock-free FIFO / AbstractRingBuffer)
[ DSP Analyzer Engine ]
│
▼ (Atomic Variable Snapshots @ 60 Hz)
[ UI Rendering Thread (Message Thread) ]

1. **Audio Thread (`juce::AudioProcessor`)**:
   - Executes real-time filtering, oversampling, and energy integration.
   - Strictly **allocation-free** and **lock-free** (no `malloc`, no mutexes, no file I/O).
   - Writes processed analytical frames into a lock-free Single-Producer Single-Consumer (SPSC) ring buffer.

2. **UI Thread (`juce::AudioProcessorEditor`)**:
   - Reads data snapshots at 60 FPS via `juce::Timer`.
   - Repaints meters, spectral curves, and status flags.

---

## 3. Class Architecture Blueprint
AppRoot
├── DSP Module
│    ├── LoudnessEngine (ITU-R BS.1770-4, True Peak, LRA, PSR/PLR)
│    ├── TonalBalanceEngine (FFT Windowing, Psychoacoustic Band Energy Aggregator)
│    └── TargetPresetManager (JSON parser & curve interpolator)
├── Licensing Module
│    └── ILicenseValidator (Interface -> DummyValidator [MVP], CloudValidator [v1.0])
└── UI Module
├── MeterPanelComponent (Left Column - Youlean style metrics)
├── TonalCurveComponent (Main Canvas - Ozone style target envelope)
└── PresetSelectorBar (Top/Header Toolbar)

---

## 4. Deployment & Build Pipeline (CI/CD)

### macOS Workflow (`.dmg`)
1. Multi-architecture build (`arm64` native for M1/M2/M3/M4).
2. Code signing via Apple Developer Certificate (`codesign`).
3. Notarization via `xcrun notarytool`.
4. Packaging into a custom styled `.dmg` installer containing Standalone app + `.vst3` + `.component` (AU).

### Windows Workflow (`.exe`)
1. MSVC compilation (`x64` Release).
2. Code signing using `signtool`.
3. Inno Setup bundling into an executable installer installing Standalone `.exe` + `.vst3` to `C:\Program Files\Common Files\VST3`.