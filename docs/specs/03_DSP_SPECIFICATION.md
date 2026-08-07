# 03_DSP_SPECIFICATION.md - Signal Processing & Mathematical Specification

## 1. Loudness & True Peak Algorithms (ITU-R BS.1770-4)

### 1.1 K-Weighting Filter Chain
Audio signals must pass through a two-stage K-weighting filter before energy aggregation:
1. **Stage 1 (Stage High-Shelf Filter)**: Pre-filter modeling head acoustic transfer function (+4 dB high shelf @ ~1.5 kHz).
2. **Stage 2 (High-Pass Filter)**: Revised head model RLB filter (1st order HPF @ ~38 Hz).

### 1.2 Meter Metrics Formulas
- **Momentary Loudness ($LUFS_M$)**: Mean square energy computed over a sliding 400 ms rectangular window.
- **Short-Term Loudness ($LUFS_{ST}$)**: Mean square energy computed over a sliding 3-second window.
- **Integrated Loudness ($LUFS_{INT}$)**: Gated measurement using an absolute threshold (-70 LUFS) and relative threshold (-10 dB below ungated level).
- **True Peak Max ($dBTP$)**: Audio signal oversampled by 4x using polyphase FIR interpolation to detect inter-sample peaks.
- **PSR (Peak-to-Short-Term Ratio)**:
  $$\text{PSR} = \text{True Peak (dBFS)} - \text{Short-Term Loudness (LUFS)}$$
- **PLR (Peak-to-Loudness Ratio)**:
  $$\text{PLR} = \text{True Peak (dBFS)} - \text{Integrated Loudness (LUFS)}$$

---

## 2. Tonal Balance & Spectral Processing

### 2.1 FFT Processing Engine
- **FFT Size**: 8192 samples (or 4096 with zero-padding) running Hann windowing with 75% overlap.
- **Smoothing**: Variable fractional octave smoothing (1/3rd octave smoothing) across the audible spectrum (20 Hz - 20 kHz).

### 2.2 4-Band Energy Distribution
Frequency spectrum is divided into 4 main macro-bands:
1. **Low**: 20 Hz - 250 Hz
2. **Low-Mid**: 250 Hz - 2.0 kHz
3. **High-Mid**: 2.0 kHz - 8.0 kHz
4. **High**: 8.0 kHz - 20.0 kHz

### 2.3 Crest Envelope & Target Curves
- Target curves are represented as upper and lower spectral boundaries $E_{upper}(f)$ and $E_{lower}(f)$.
- Deviation $\Delta(f)$ is computed in real time. If the analyzed curve drifts outside the target envelope for a specified duration (>500ms), the UI triggers an out-of-balance highlight for that frequency band.
