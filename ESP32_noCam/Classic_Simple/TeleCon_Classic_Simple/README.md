# TeleCon — Classic Simple (Normal)

Shared Bluetooth Classic + SIMPLE text starter for ESP32 DevKit (no camera).

## Flash

1. Open `TeleCon_Classic_Simple.ino` in Arduino IDE.
2. Board: **ESP32 Dev Module**.
3. Upload, open Serial Monitor at **115200**.

## Android

1. Pair Bluetooth **`ESP32-TC-RC-BT-Simple`** (PIN **1234** if asked).
2. Control Panel or **RC Vehicle Pro** → Board **DevKit** → **Normal** → **Classic + Simple**.
3. Connect — handshake `RC:CONNECT,proto,simple` → `RC:ACK,app,RC`.

## What to edit

Put motor / servo code in `onControl()` and `onButton()` where it says **YOUR CODE HERE**.

| UI | Wire | Typical use |
|----|------|-------------|
| Left stick Y | `ly` | Throttle |
| Right stick X | `rx` | Steering |
| Right knob | `rk` | Camera pan |
| Lights | `sw` bit0 | Lights |
| BTN 2 | horn | Short buzzer pulse |
| BTN 4 | center pan | Pan to center |

## Telemetry

Every **500 ms** the board sends vehicle HUD data:

```
RC:DATA,left,<|ly|*25/10>,right,0,lo,1,ro,0,lg,1,rg,0,analog,90,batt,76,led,<sw>
```

- `left` = speed km/h × 10 (from throttle `|ly|`)
- `batt` / `analog` = demo values (76% / temp 90) until you wire sensors
- `ENABLE_PLOT 1` → `RC:PLOT` about every **50 ms** with eight samples. See **Telemetry channels** below.

## Telemetry channels

`kTelemetryChannels[]` in `TeleCon_Classic_Simple.ino` is the board-to-phone analog bus. Row 0 is phone **CH1** (`v0`); row 7 is **CH8** (`v7`). Each sample is one byte, 0–255. The phone already lists CH1–CH8 in the channel menus for plots, radar, gauges, and HUD badges. There is no ninth channel: extra keys are dropped.

A row is `{ kind, vehicle, pin, ledcChannel, dutyMin, dutyMax }`. Only the fields for `kind` are read.

| `kind` | What is sent | Fields that matter |
|--------|----------------|--------------------|
| `TEL_VEHICLE` | A built-in signal from `telemetryVehicle()` | `vehicle` |
| `TEL_ADC` | `analogRead(pin)`, 0–4095 scaled to 0–255 | `pin` |
| `TEL_GPIO` | `digitalRead(pin)` → 0 or 255. An output pin reports the level last written. | `pin` |
| `TEL_PWM` | LEDC duty already running on the pin. `dutyMin` becomes 0 and `dutyMax` becomes 255. Attach the pin first (`ledcAttach`, or `ledcSetup` + `ledcAttachPin`). | `pin`, `ledcChannel` (Arduino-ESP32 2.x), `dutyMin`, `dutyMax` |

On a `TEL_ADC`, `TEL_GPIO`, or `TEL_PWM` row the `vehicle` name is ignored. On a `TEL_VEHICLE` row, `pin` and the duty pair are ignored.

Built-in signals: `TEL_TEMP` (demo 90), `TEL_THROTTLE` (`|ly|`), `TEL_BATT` (demo 76% scaled), `TEL_STEER` (`|rx|`), `TEL_LIGHTS` (switch bit 0 → 0 or 255), `TEL_KNOB_L` / `TEL_KNOB_R` (0–1023 → 0–255), `TEL_RY` (`|ry|`). Throttle, steer, and `TEL_RY` are magnitudes, so they do not show direction.

Stock rows:

| Phone | Source |
|-------|--------|
| CH1 | Demo temperature |
| CH2 | Throttle magnitude |
| CH3 | Demo battery |
| CH4 | Steering-stick magnitude |
| CH5 | Lights switch |
| CH6 | Left knob |
| CH7 | Right knob |
| CH8 | Steering-servo PWM on `PIN_STEER_SERVO`. Duty 204…409 is the 1000–2000 µs travel (center ≈ 128). |

8-bit motor PWM uses `dutyMin = 0` and `dutyMax = 255`. Both axes of both sticks fit in four of these rows. Map a signed axis with center 128: `map(axis, -100, 100, 0, 255)`. The plot period stays 50 ms either way; filling more of the eight bytes does not add packets. One Simple line is about 80 bytes (~1.6 KB/s).

```cpp
{ TEL_ADC,  TEL_TEMP,     34, -1, 0, 1 },    // GPIO 34, vehicle name ignored
{ TEL_GPIO, TEL_LIGHTS,   15, -1, 0, 1 },    // GPIO 15
{ TEL_PWM,  TEL_THROTTLE, 25,  0, 0, 255 },  // 8-bit PWM, LEDC channel 0
```

Serial inject (after handshake, Newline): paste one `RC:DATA,...` or `RC:PLOT,...` line.

## After Connect works (optional)

To hide Serial prints, set **`ENABLE_DEBUG 0`**, **`ENABLE_SERIAL_INJECT 0`**, and **`ENABLE_PLOT 0`** (if unused), then flash the sketch to the ESP32 again (Arduino IDE → Upload). You may leave them at **`1`** if you still want logs.

## Steering trim (Android)

Tune tickers on **RC Vehicle Pro** shift `rx` before it is sent. Check only closes that mode. This sketch does not store a center: `rx = 0` is 1500 µs (±500 µs at full stick) on `PIN_STEER_SERVO`.

## Fail-safe

On Bluetooth disconnect, sticks reset to 0; with `ENABLE_DEBUG 1`, Serial prints `[IO] fail-safe`.

## Advanced

For binary protocol + fuller vehicle IO architecture, see **`../../Classic_Binary/`**.
