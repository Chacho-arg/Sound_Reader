# TASKS.md - Executable Task Breakdown

## Legend & Status Tracking
- [ ] Pending
- [/] In Progress
- [x] Completed

---

## Phase 0: Project Setup & Repository Infrastructure
> **Goal**: Establish the CMake build system, JUCE dependency mapping, and CI/CD pipelines.

- [x] **Task 0.1**: Create repository folder structure (`docs/specs/`, `Source/DSP/`, `Source/UI/`, `Source/Licensing/`, `Source/Presets/`).
- [x] **Task 0.2**: Create root `CMakeLists.txt` configured for JUCE 7+ targetting Standalone, VST3, and AU formats (macOS arm64 & Windows x64).
- [x] **Task 0.3**: Implement minimal `PluginProcessor` and `PluginEditor` boilerplate to verify target compilation on local machine.
- [x] **Task 0.4**: Configure GitHub Actions CI workflow to build Standalone, VST3, and AU binaries on both macOS and Windows runners.

---

## Phase 1: Core DSP Engine (Lock-Free & Un-rendered)
> **Goal**: Implement mathematical meters and FFT algorithms with zero dynamic allocations in the audio process loop.

- [x] **Task 1.1**: Implement lock-free Single-Producer Single-Consumer (SPSC) Ring Buffer (`AbstractRingBuffer`) for audio sample streaming to analysis threads.
- [x] **Task 1.2**: Implement ITU-R BS.1770-4 K-weighting filter chain (High-Shelf + High-Pass) in `Source/DSP/KWeightingFilter.cpp`.
- [x] **Task 1.3**: Implement sliding window logic for Momentary (400ms) and Short-Term (3s) LUFS calculation.
- [x] **Task 1.4**: Implement Gated Integrated Loudness engine (-70 LUFS absolute / -10 dB relative thresholds) and Loudness Range (LRA).
- [x] **Task 1.5**: Implement True Peak detection with 4x polyphase FIR oversampling.
- [x] **Task 1.6**: Implement Dynamic Ratios (PSR and PLR calculation modules).
- [x] **Task 1.7**: Implement `juce::dsp::FFT` spectral analyzer with Hann windowing and 1/3rd octave smoothing aggregated into 4 macro-bands (Low, Low-Mid, High-Mid, High).
- [x] **Task 1.8**: Create DSP Unit Tests feeding synthetic calibrated sine waves (-12 dBFS) and pink noise to validate LUFS and True Peak accuracy.

---

## Phase 2: Preset System & Target Boundaries
> **Goal**: Create JSON target curve parser and metric range boundaries engine.

- [x] **Task 2.1**: Define JSON schema for Tonal Balance Target Envelopes and Loudness/PLR Ranges in `FactoryPresets.json`.
- [x] **Task 2.2**: Implement `TargetPresetManager` to load and parse presets.
- [x] **Task 2.3**: Map dynamic preset thresholds to real-time metric comparison methods.

---

## Phase 3: Graphical User Interface (60 FPS & Responsive)
> **Goal**: Build high-DPI custom UI components reading lock-free snapshots from DSP.

- [x] **Task 3.1**: Design main layout container (Fixed/Flex 1000x600 grid: Left panel for metrics, Right canvas for FFT Tonal Curve).
- [x] **Task 3.2**: Implement `MeterPanelComponent` (Left Panel) to display SHORT TERM, INTEGRATED, LRA, PSR, PLR, MOMENTARY MAX, and TRUE PEAK MAX using high-legibility typography.
- [x] **Task 3.2.1**: Integrate Preset Manager to show current preset in `MeterPanel` (show preset name and status).
- [x] **Task 3.3**: Implement dynamic color state warnings in `MeterPanel` when metrics breach preset targets (amber/red status).
- [x] **Task 3.4**: Implement `TonalCurveComponent` (Right Panel) using OpenGL/JUCE Graphics vector path rendering for 4-band Target Envelopes and real-time spectral trace line.
- [x] **Task 3.5**: Add Header Bar with Preset Selector dropdown (show active preset), Reset Meters button, and Settings popup.
- [x] **Task 3.5.1**: Add Save Preset button in Header Bar to save current settings as new preset.
- [x] **Task 3.6**: Connect `juce::Timer` polling thread capped at 60 FPS to refresh UI components smoothly without high CPU overhead (<5% GUI load).

---

## Phase 4: Licensing Abstraction Layer
> **Goal**: Decouple activation check logic behind interface.

- [x] **Task 4.1**: Create `ILicenseValidator` pure virtual interface class.
- [x] **Task 4.2**: Implement `DummyLicenseValidator` for MVP phase (always returns valid state).
- [x] **Task 4.3**: Integrate validation status check into application launch lifecycle.

---

## Phase 5: Installers & Release Packaging
> **Goal**: Generate signed and notarized end-user installers.

- [x] **Task 5.1**: Configure Inno Setup script for Windows installer (`.exe`) bundling Standalone + VST3 binary.
- [x] **Task 5.2**: Configure macOS `.dmg` packaging script with Apple `codesign` and `notarytool` integration.
- [x] **Task 5.3**: Perform full cross-platform QA audit (Logic Pro AU validation on Apple Silicon, Reaper VST3 on Windows, Standalone operation).