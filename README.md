# Mjonir — Automatic Guitar Tuner

A handheld automatic guitar tuner built on an ESP32. A MEMS microphone listens to
the plucked string, an FFT finds its pitch, and a DC motor turns the tuning peg
until the string is in tune. An SSD1306 OLED and a 4-button D-pad provide the UI.

> **Status: work in progress.** The hardware is still being developed, so the
> firmware is incomplete and has not been fully tested on the final board.

## Features

- Pitch detection with a 2048-point FFT (ESP-DSP) on the INMP441 I2S microphone
- Motor drive through a DRV8833, with speed proportional to the tuning error
- Tuning presets: **Default** (E A D G B E), **All Flat** (Eb), and **Custom**
  (choose any note for each string)
- **Manual** mode (you pick the string) and **Auto** mode (detects the closest string)
- On-screen tuning meter showing the target note, its neighbours, and a ±2% in-tune window
- Battery level indicator

## Repository layout

```
firmware/   ESP-IDF project (C++)
  main/                 app entry point and page stack loop
  components/tunerLib/  I2S microphone driver, FIR filter + downsampling, FFT pitch detection
  components/control/   DRV8833 motor control (LEDC PWM)
  components/page/      UI pages: menu, tuning presets, custom tuning, tuner
  components/display/   SSD1306 helpers
  components/system/    D-pad input, cursor, battery ADC, note/frequency utilities
hardware/   KiCad 9 project: main PCB, D-pad PCB, BOM, PDFs, STEP models
cad/        Mechanical enclosure model (SolidWorks; to be added)
```

## Hardware

| Part | Notes |
|---|---|
| ESP32-WROOM-32 | Main MCU |
| INMP441 | I2S MEMS microphone |
| DRV8833 module | Motor driver for the peg winder |
| SSD1306 OLED 128×64 | I2C display |
| 4× tactile switch | D-pad on a separate PCB |
| YB-8 module, battery, power switch, buzzer | See `hardware/2. Project Tuner.csv` for the full BOM |

### Pin map

| Function | GPIO |
|---|---|
| OLED SDA / SCL | 21 / 22 |
| I2S WS / SCK / SD | 12 / 14 / 13 |
| Motor AIN1 / AIN2 | 18 / 19 |
| D-pad Up / Right (OK) / Down / Left (Back) | 25 / 26 / 32 / 33 |
| Battery sense (ADC1_CH7) | 35 |

## Building the firmware

Requires **ESP-IDF v5.1.x**. The `esp-dsp` and `ssd1306` components are
downloaded automatically by the IDF component manager.

```bash
. ~/esp/esp-idf/export.sh
cd firmware
idf.py set-target esp32   # first time only
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

> If you use Anaconda, its `python3` can conflict with ESP-IDF's Python
> environment. Put the system Python first: `export PATH=/usr/bin:$PATH` before
> sourcing `export.sh`.

## Controls

| Button | Action |
|---|---|
| Up / Down | Move the cursor, or change the value while editing |
| Right | OK / select |
| Left | Back / cancel |

## Known limitations / TODO

- Bass tuning is listed in the menu but not implemented yet
- Noise calibration is disabled (`calibration_time = 0`)
- Pitch resolution is about 1 Hz per FFT bin with no interpolation, which is coarse
  for low strings
- Uses legacy ESP-IDF I2S, ADC and SSD1306 drivers (deprecated in IDF 5.x)
- The buzzer is on the PCB but not used by the firmware yet
- Mechanical CAD model not yet in the repo

> ⚠️ When testing, keep the motor disconnected from the peg until you have
> confirmed the direction of rotation, so it cannot over-tighten a string.
