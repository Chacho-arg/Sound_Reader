# 01_REQUIREMENTS.md - Real-Time Loudness & Tonal Balance Meter

## 1. Vision & Purpose
A unified real-time audio analysis tool combining precision loudness/dynamics metering (Short Term, Integrated, LRA, PSR/PLR, True Peak) with a 4-band spectral tonal balance target curve analyzer. 

Designed specifically for real-time broadcast (Online/FM) monitoring, mastering, and mixing, running both as a Standalone App and VST3/AU Plugin on macOS (Apple Silicon M1+) and Windows (x64).

---

## 2. Target Platforms & Architectures
- **macOS**: Native Apple Silicon (`arm64`), macOS 12+ (Universal Binary optional for Universal Intel compatibility). Formats: Standalone, VST3, AU.
- **Windows**: Windows 10/11 (`x64`). Formats: Standalone, VST3.
- **Distribution / Installers**: Automated `.dmg` (macOS, notarized) and `.exe` / `.msi` (Windows installer with silent install flags).

---

## 3. Key Feature Set & User Stories

### 3.1 Loudness & Dynamics Module (Left Panel)
- **Real-Time Metrics Display**:
  - Short-Term Loudness (3s window, LUFS)
  - Integrated Loudness (Gated, LUFS)
  - Loudness Range (LRA, LU)
  - Dynamics Peak-to-Short-term Ratio (PSR, LU)
  - Average Dynamics Peak-to-Loudness Ratio (PLR, LU)
  - Momentary Max (LUFS)
  - Short-Term Max (LUFS)
  - True Peak Max (dBTP, inter-sample peak detection with 4x oversampling)
- **Target Ranges & Threshold Visuals**:
  - Preset-driven LU target boundaries (e.g., Online Broadcast: -14 LUFS / 8-10 LU PLR; FM Broadcast: -9 to -11 LUFS / lower dynamic range).
  - Color-coded indicator warnings when audio exceeds/underperforms preset boundaries.

### 3.2 Tonal Balance Control Module (Main Graphic Display)
- **4-Band Target Spectrum Analysis**:
  - Bands: Low (20Hz - 250Hz), Low-Mid (250Hz - 2kHz), High-Mid (2kHz - 8kHz), High (8kHz - 20kHz).
  - Real-time energy curve overlay against target tolerance envelopes ("crest curves").
- **Presets Engine**:
  - Target profiles for Music Genres (EDM, Pop, Rock, Acoustic) and Broadcast Profiles (Online Streaming, FM Processing Output).
  - Ability to save/load custom reference targets (`.json` or extracted audio file profile).

### 3.3 System Architecture & Performance
- Zero latency in bypass or direct monitoring modes for real-time broadcast inserting.
- Low CPU usage (<3% on Apple M1 / Core i7).
- Lock-free ring buffers for data transmission between DSP thread and UI rendering thread.

---

## 4. Non-Functional Requirements
- **Licensing Abstraction**: Architecture must decouple licensing logic via an interface (`ILicenseValidator`) so CDKey/JWT activation can be enabled in v1.0 without refactoring core DSP/UI logic.
- **UI Refresh Rate**: Smooth 60 FPS rendering using lock-free FIFO structures.