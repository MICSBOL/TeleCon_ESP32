#pragma once

#include <Arduino.h>
#include "pins.h"

/*
 * Phone CH1…CH8 are RC:PLOT v0…v7 and binary CC 33 bytes 0…7.
 * Edit kTelemetryChannels[] to choose what each channel reports (0–255).
 *
 * TEL_VEHICLE  built-in signal (temp, throttle, battery, steer, lights, knobs)
 * TEL_ADC      analogRead(pin), 0–4095 scaled to 0–255
 * TEL_GPIO     digitalRead(pin) → 0 or 255 (an output pin reports what was written)
 * TEL_PWM      LEDC duty on pin. dutyMin maps to 0, dutyMax maps to 255.
 *              8-bit motor PWM uses 0…255. 12-bit servo travel 1000–2000 µs uses 204…409.
 */

enum TelKind : uint8_t {
  TEL_VEHICLE = 0,
  TEL_ADC,
  TEL_GPIO,
  TEL_PWM,
};

enum TelVehicle : uint8_t {
  TEL_TEMP = 0,
  TEL_THROTTLE,
  TEL_BATT,
  TEL_STEER,
  TEL_LIGHTS,
  TEL_KNOB_L,
  TEL_KNOB_R,
  TEL_RY,
};

struct TelChannel {
  TelKind kind;
  TelVehicle vehicle;
  int pin;
  int ledcChannel;
  int dutyMin;
  int dutyMax;
};

// Index 0 = CH1 / v0 … index 7 = CH8 / v7.
static const TelChannel kTelemetryChannels[8] = {
  { TEL_VEHICLE, TEL_TEMP,     -1,             -1,         0,   1 },
  { TEL_VEHICLE, TEL_THROTTLE, -1,             -1,         0,   1 },
  { TEL_VEHICLE, TEL_BATT,     -1,             -1,         0,   1 },
  { TEL_VEHICLE, TEL_STEER,    -1,             -1,         0,   1 },
  { TEL_PWM,     TEL_THROTTLE, PIN_DRIVE_L_PWM, CH_DRIVE_L, 0, 255 },
  { TEL_PWM,     TEL_THROTTLE, PIN_DRIVE_R_PWM, CH_DRIVE_R, 0, 255 },
  { TEL_GPIO,    TEL_LIGHTS,   PIN_LIGHTS,     -1,         0,   1 },
  { TEL_PWM,     TEL_KNOB_R,   PIN_PAN_SERVO,  CH_PAN,   204, 409 },
};

using TelVehicleFn = uint8_t (*)(TelVehicle id);

inline uint8_t telemetryReadPwm(const TelChannel& ch) {
  if (ch.dutyMax <= ch.dutyMin) return 0;
  uint32_t duty = 0;
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  if (ch.pin >= 0) duty = ledcRead((uint8_t)ch.pin);
#else
  if (ch.ledcChannel >= 0) duty = ledcRead((uint8_t)ch.ledcChannel);
#endif
  long scaled = constrain((long)duty, ch.dutyMin, ch.dutyMax);
  return (uint8_t)map(scaled, ch.dutyMin, ch.dutyMax, 0, 255);
}

inline uint8_t telemetryReadOne(const TelChannel& ch, TelVehicleFn vehicle) {
  switch (ch.kind) {
    case TEL_ADC:
      if (ch.pin < 0) return 0;
      return (uint8_t)map(constrain(analogRead(ch.pin), 0, 4095), 0, 4095, 0, 255);
    case TEL_GPIO:
      if (ch.pin < 0) return 0;
      return digitalRead(ch.pin) ? 255 : 0;
    case TEL_PWM:
      return telemetryReadPwm(ch);
    default:
      return vehicle ? vehicle(ch.vehicle) : 0;
  }
}

inline void telemetryReadAll(uint8_t out[8], TelVehicleFn vehicle) {
  for (int i = 0; i < 8; i++) out[i] = telemetryReadOne(kTelemetryChannels[i], vehicle);
}
