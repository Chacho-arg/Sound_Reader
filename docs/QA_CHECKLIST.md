# Cross-Platform QA Checklist

## Purpose
Validate SoundReader plugin builds and runtime behavior across all supported platforms and hosts.

---

## 1. Apple Silicon AU Validation (Logic Pro)

### Build Verification
- [ ] `cmake -B build-arm64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0` succeeds
- [ ] `cmake --build build-arm64 --config Release` completes without errors
- [ ] AU bundle exists at `build-arm64/SoundReader.component`
- [ ] Code signing passes (if signing configured)
- [ ] Notarization passes via `apple-actions/notarize-and-authenticate`
- [ ] DMG creation succeeds via `build-dmg.sh`

### Logic Pro Validation (macOS 14+ Apple Silicon)
- [ ] Plugin scans and appears in Logic Pro AU list
- [ ] Plugin loads without crash or validation error
- [ ] Audio processing activates when audio plays through channel strip
- [ ] Meter panel renders correctly at 60 FPS
- [ ] Tonal curve component renders without graphical artifacts
- [ ] Preset selector dropdown populates with factory presets
- [ ] Save preset button creates `UserPresets.json` in working directory
- [ ] Custom preset persists after Logic Pro restart
- [ ] License validation banner/status displays correctly
- [ ] No audio glitches or dropouts during processing

### Performance Targets
- [ ] CPU usage < 5% on M1/M2/M3 during idle
- [ ] CPU usage < 15% during active audio processing
- [ ] Memory usage stable (no leaks over 30-minute session)
- [ ] UI latency < 16ms (60 FPS smooth)

---

## 2. Windows VST3 Validation (Reaper)

### Build Verification
- [ ] `cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release` succeeds
- [ ] `cmake --build build --config Release` completes without errors
- [ ] VST3 bundle exists at `build/SoundReader.vst3`
- [ ] Code signing passes with `signtool` (if configured)
- [ ] Inno Setup installer builds via `ISCC.exe installer.iss`
- [ ] Installer runs and copies files to `Program Files\SoundReader`

### Reaper Validation (Windows 10/11 x64)
- [ ] Plugin scans and appears in Reaper VST3 list
- [ ] Plugin loads without crash or validation dialog
- [ ] Audio processing activates on track arm/play
- [ ] Meter panel displays all metrics correctly
- [ ] Tonal curve renders without artifacts
- [ ] Preset selector lists factory presets
- [ ] Save preset creates `UserPresets.json`
- [ ] Custom preset loads after Reaper restart
- [ ] License status displays correctly
- [ ] No audio glitches or dropouts

### Performance Targets
- [ ] CPU usage < 5% idle
- [ ] CPU usage < 15% during processing
- [ ] Memory stable over 30-minute session
- [ ] UI smooth at 60 FPS

---

## 3. Standalone Operation

### macOS Standalone
- [ ] `build-arm64/SoundReader.app` launches from Finder
- [ ] App passes Gatekeeper / notarization check
- [ ] Audio device selection works (CoreAudio)
- [ ] Input meter activates with system audio or mic input
- [ ] All UI components render correctly
- [ ] Settings/preferences persist between launches
- [ ] App quits cleanly without crash logs

### Windows Standalone
- [ ] `SoundReader.exe` launches from Start Menu/Desktop
- [ ] Audio device selection works (ASIO/WASAPI)
- [ ] Input meter activates with audio input
- [ ] All UI components render correctly
- [ ] Settings/preferences persist between launches
- [ ] App exits cleanly without crash logs

### Cross-Platform Behavior
- [ ] Preset save/load works identically on all platforms
- [ ] `UserPresets.json` format is portable (line endings, encoding)
- [ ] Metrics display identical values across platforms
- [ ] FFT spectral analysis produces identical results
- [ ] License validation behaves identically

---

## 4. Regression Checks

### Audio Accuracy
- [ ] -12 dBFS sine wave reads -12 dBTP ±0.5 dB
- [ ] Integrated LUFS matches EBU R128 test vectors
- [ ] PLR calculation matches reference implementation
- [ ] K-weighting filter frequency response matches spec

### UI/UX
- [ ] Dark theme renders correctly on macOS Light Mode
- [ ] Dark theme renders correctly on Windows Light/Dark Mode
- [ ] High-DPI scaling works on Retina displays
- [ ] Window resize preserves layout integrity
- [ ] Plugin editor opens correctly in all hosts

### Crash Regression
- [ ] No crash on rapid preset switching
- [ ] No crash on save during audio playback
- [ ] No crash on editor close during processing
- [ ] No crash on license validation failure simulation

---

## 5. Signing & Notarization

### macOS
- [ ] Developer ID Application certificate valid
- [ ] Hardened runtime enabled
- [ ] Notarytool submission succeeds
- [ ] Stapled ticket in app bundle
- [ ] `spctl --assess --verbose=4` returns accepted

### Windows
- [ ] EV code signing certificate valid
- [ ] SHA-256 timestamp applied
- [ ] SmartScreen reputation established (initial runs may warn)
- [ ] Inno Setup installer signed

---

## 6. Known Limitations / Deferred Items

- [ ] Task 1.6–1.8 unit tests not yet automated in CI
- [ ] OpenGL rendering fallback not tested on Windows without GPU
- [ ] AU validation in Logic Pro requires physical Apple Silicon machine
- [ ] VST3 validation in Reaper requires physical Windows machine

---

## Execution Notes
- Check each item as `PASS`, `FAIL`, or `N/A`
- Document failures with host version, OS version, and reproduction steps
- Attach crash logs and console output for any `FAIL` items
- Block release if any `P1` item fails
