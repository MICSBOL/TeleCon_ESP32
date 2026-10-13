# TeleCon ESP32 firmware

Shared `RC:` sketches for Android **Control Panel** and **RC Vehicle Pro**. Control is Bluetooth on a DevKit. Video, when used, is a separate ESP32-CAM.

Programmers writing firmware for **any** Bluetooth or TCP device (not only these sketches): see [Protocol_Documentation](Protocol_Documentation/README.md).

Android app users (Home, first connection, Control panel, RC Vehicle): see [General_Documentation](General_Documentation/README.md) (full user guide + fast guide).

Open a sketch folder in Arduino IDE (the folder that contains the `.ino`).
Pin maps, BT/SSID identity, and `YOUR CODE HERE` stay in that sketch's `README.md`.

## Product matrix

| Sketch | Control Panel | RC Vehicle Pro | Path |
|--------|---------------|----------------|------|
| DevKit Classic Simple | Yes | Yes | `ESP32_noCam/Classic_Simple/TeleCon_Classic_Simple/` |
| DevKit Classic Binary | Yes | Yes | `ESP32_noCam/Classic_Binary/TeleCon_Classic_Binary/` |
| DevKit BLE Binary | Yes | Yes | `ESP32_noCam/BLE/TeleCon_BLE_Binary/` |
| CAM video-only (with DevKit Bluetooth) | Yes | Yes | `ESP32_cam/WiFi/TeleCon_CAM_SoftAP_Video/` |

## Identities

| Build | Name / SSID | Handshake |
|-------|-------------|-----------|
| Classic Simple | BT `ESP32-TC-RC-BT-Simple` | `proto,simple` |
| Classic Binary | BT `ESP32-TC-RC-BT-Binary` | `proto,binary` |
| BLE Binary | `ESP32-TC-RC-BLE-Binary` | `proto,binary` |
| CAM video-only | SoftAP `TeleCon-RC-CAM-Starter` | HTTP `/stream` only |

Password for the camera SoftAP: `telecon1234`. Sticks stay on Bluetooth. The camera board does not open TCP port 3333.

## Sketch ZIPs

`zips/` has one Arduino IDE archive per connection. Each archive contains the sketch folder (the `.ino` and its sources). The Android app downloads these files. After you change a sketch, replace the ZIP that has the same file name and push.

## Telemetry channels (Bluetooth sketches)

Classic Simple, Classic Binary, and BLE Binary send eight samples (phone CH1–CH8) about every 50 ms. Edit `kTelemetryChannels[]` in the Classic Simple `.ino`, or `TelemetryChannels.h` in the binary sketches. Each row is one byte: a built-in vehicle signal, an analog pin, a digital pin, or a PWM duty. The phone channel menus bind those bytes; there is no ninth channel. Per-sketch tables are in each sketch `README.md`.

## Steering trim

On **RC Vehicle Pro**, Tune → tickers shifts the steering stick on the phone (`rightTrimX`). Check only closes that mode. Stock sketches do not store a center: stick `rx = 0` is 1500 µs. A later vehicle sketch can store one servo center by itself if a second transmitter must share the same straight-ahead.
