# TeleCon — BLE Binary (Advanced)

Complete architecture for **Advanced** DevKit users (BLE NUS + binary RC).  
Role A video overlay: flash `ESP32_cam/WiFi/TeleCon_CAM_SoftAP_Video/` on a separate CAM.

## Board

ESP32 DevKit (no camera)

## Bluetooth

- Device name: `ESP32-TC-RC-BLE-Binary`
- App prefix: `RC`
- Android Control Panel or **RC Vehicle Pro** → Board **DevKit** → **Advanced** → **BLE Binary**

## Protocol

- Handshake: `RC:CONNECT,proto,binary` within ~2.5 s
- Success: `RC:ACK,app,RC`
- Wrong proto/app: `RC:NAK,reason,proto_mismatch|app_mismatch,...`

Binary: `AA 55` (18 B) control, `BB 66` buttons, `CC 11`/`CC 22` telemetry + optional `CC 33` debug plot (no `CC 44`).

## Control map

| UI | Wire | Action |
|----|------|--------|
| Left stick Y | `ly` | Throttle (differential drive) |
| Right stick X | `rx` | Steering / mix |
| Right knob | `rk` | Camera pan servo (center 512) |
| Lights | switch1 / `sw` bit0 | Lights GPIO |
| Buzzer | BTN 2 | Horn pulse |
| Camera front | BTN 4 | Center pan |
| E-stop / disconnect | sticks 0 / link drop | Motors off |

## Telemetry

- Speed km/h × 10 → panel `left` / `CC 11` left (`lo=1`)
- Battery % → `batt` / `CC 22`
- Motor temp → `analog` 0–255 / `CC 22`
- Plot → `CC 33` count=8 (~20 Hz, 14 bytes). CH1…CH8. See **Telemetry channels** below.

## Telemetry channels

`kTelemetryChannels[]` in `TelemetryChannels.h` chooses the eight bytes in each `CC 33` plot (the same bus as Simple `RC:PLOT` `v0`…`v7`). Row 0 is phone **CH1**; row 7 is **CH8**. The phone channel menus already bind those bytes to plots, radar, gauges, and HUD badges. The app keeps eight channels; a ninth sample is dropped.

A row is `{ kind, vehicle, pin, ledcChannel, dutyMin, dutyMax }`. Only the fields for `kind` are read.

| `kind` | What is sent | Fields that matter |
|--------|----------------|--------------------|
| `TEL_VEHICLE` | Built-in signal: temp ADC, `\|ly\|`, battery scaled 0–255, `\|rx\|`, lights bit, knobs, `\|ry\|` | `vehicle` |
| `TEL_ADC` | `analogRead(pin)`, 0–4095 → 0–255 | `pin` |
| `TEL_GPIO` | `digitalRead(pin)` → 0 or 255. An output pin reports the level last written. | `pin` |
| `TEL_PWM` | LEDC duty. `dutyMin` → 0, `dutyMax` → 255. Core 3.x reads `pin`; core 2.x reads `ledcChannel`. The pin must already be attached. | `pin`, `ledcChannel`, `dutyMin`, `dutyMax` |

On a pin row the `vehicle` name is ignored. Throttle and steer vehicle signals are magnitudes (`abs`), so they do not show left versus right.

Stock rows while `TELECON_VEHICLE_IO` is 1:

| Phone | Source |
|-------|--------|
| CH1 | Motor temperature |
| CH2 | Throttle magnitude |
| CH3 | Battery, scaled to 0–255 |
| CH4 | Steering-stick magnitude |
| CH5 | Left-drive PWM, GPIO 25, duty 0…255 |
| CH6 | Right-drive PWM, GPIO 27, duty 0…255 |
| CH7 | Lights GPIO 15 |
| CH8 | Pan-servo PWM, GPIO 12. Duty 204…409 is 1000–2000 µs (center ≈ 128). |

Both axes of both sticks fit by replacing four rows. Map a signed axis with center 128: `map(axis, -100, 100, 0, 255)`. The plot stays one `CC 33` every 50 ms; using more of the eight bytes does not add a packet.

Serial inject (after handshake): `RC:DATA` / `RC:PLOT` → one-shot `CC` frames (Control Panel style).


## Wiring (DevKit — `TELECON_VEHICLE_IO=1`)

| Function | GPIO |
|----------|------|
| Drive L PWM / DIR | 25 / 26 |
| Drive R PWM / DIR | 27 / 14 |
| Steer servo | 13 |
| Camera pan servo | 12 |
| Lights | 15 |
| Buzzer | 2 |
| Status LED | 4 |
| Battery ADC | 34 |
| Motor temp ADC | 35 |

Edit `pins.h` for your motor driver.

## Camera

This board has no Wi‑Fi stream. For Role A video, flash `TeleCon_CAM_SoftAP_Video` and join `TeleCon-RC-CAM-Starter`.

## Flash

1. Board: ESP32 DevKit  
2. Open this folder’s `.ino`  
3. Upload; Serial Monitor @ 115200  
4. Pair/connect from Android as BLE Binary (`ESP32-TC-RC-BLE-Binary`)

## Steering trim (Android)

Tune tickers on **RC Vehicle Pro** shift `rx` before it is sent. Check only closes that mode. This sketch does not store a center: `rx = 0` is 1500 µs (±500 µs at full stick).

## Fail-safe

On BT/BLE disconnect, motors are zeroed immediately via `rcVehicleFailSafe()`.
