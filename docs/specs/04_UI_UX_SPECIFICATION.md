# 04_UI_UX_SPECIFICATION.md - Graphical User Interface & UX Specification

## 1. Visual Layout & Grid System

Dimensions: Recommended default size **1000 x 600 px** (Resizable, maintaining 16:9 or custom auto-flex layout).
+-------------------------------------------------------------------------+
| [Preset: Online Broadcast ▼] [Reset Meters] [Settings]                   |
+-------------------+-----------------------------------------------------+
| SHORT TERM        | TONAL BALANCE GRAPH (20Hz - 20kHz)                  |
| -13.9 LUFS        |                                                     |
|                   |  Low    |  Low-Mid   |  High-Mid   |  High          |
| INTEGRATED        | +------+ +----------+ +-----------+ +------+        |
| -13.9 LUFS        | |      | |          | |           | |      |        |
|                   | |~~~~~~| |~~~~~~~~~~| |~~~~~~~~~~~| |~~~~~~|        |
| LOUDNESS RANGE    | +------+ +----------+ +-----------+ +------+        |
| 5.2 LU            |                                                     |
|                   | Real-Time Curve: Light Cyan Line                    |
| AVG DYNAMICS(PLR) | Target Crest Envelope: Dark Teal Shaded Band         |
| 12.8 LU           |                                                     |
|                   |                                                     |
| MOMENTARY MAX     |                                                     |
| -9.2 LUFS         |                                                     |
|                   |                                                     |
| TRUE PEAK MAX     |                                                     |
| -1.1 dB           |                                                     |
+-------------------+-----------------------------------------------------+

---

## 2. Color Palette & Typography
- **Background**: Dark Navy Slate (`#12181F` / `#1B222C`) for high contrast during long mastering sessions.
- **Metrics Display (Left)**: High-legibility sans-serif numerical font (e.g., Inter / Roboto Bold).
  - Normal Status: Off-white / Silver (`#E0E6ED`).
  - Target Warning Status: Soft Amber (`#FFB74D`) or Red Crimson (`#FF5252`) when exceeding Target True Peak / PSR bounds.
- **Tonal Curve Canvas (Right)**:
  - Envelope Shadow: Gradient Cyan/Teal (`#00838F` to `#00ACC1` with 30% alpha).
  - Real-time Trace Line: Bright Cyan/White (`#80DEEA`).

---

## 3. Target Range Indicators (Dynamic Feedback)
- When **Online Broadcast** preset is selected:
  - LUFS target bounds: **-14 LUFS (±1.0 LU)**.
  - Target PLR range: **8 LU - 12 LU**.
- When **FM Broadcast** preset is selected:
  - LUFS target bounds: **-9 LUFS to -11 LUFS**.
  - Target PLR range: **4 LU - 6 LU**.
- Values outside target boundaries automatically shift numerical background color to alert the sound operator.