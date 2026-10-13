#include "RcVehicleControl.h"
#include "TeleConConfig.h"
#include "pins.h"

#if defined(TELECON_DEBUG)
#include "TeleConDebug.h"
#endif

#ifndef TELECON_VEHICLE_IO
#define TELECON_VEHICLE_IO 1
#endif

// rx −100…100 maps around a fixed 1500 µs pulse. Phone trim offsets rx before send.
constexpr int kSteerCenterUs = 1500;
constexpr int kSteerTravelUs = 500;

uint32_t steerDutyFromUs(int us) {
  int pulse = constrain(us, 500, 2500);
  return (uint32_t)((long)pulse * 4095L / 20000L);
}

uint32_t steerDutyFromRx(int rx) {
  int s = constrain(rx, -100, 100);
  int us = (int)map(s, -100, 100, kSteerCenterUs - kSteerTravelUs, kSteerCenterUs + kSteerTravelUs);
  return steerDutyFromUs(us);
}

namespace {

RcVehicleState gState;
unsigned long buzzerUntilMs = 0;
bool failSafeActive = true;

#if TELECON_VEHICLE_IO
void setupPwm(int pin, int channel, int freqHz, int resolutionBits) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  (void)channel;
  ledcAttach(pin, freqHz, resolutionBits);
#else
  ledcSetup(channel, freqHz, resolutionBits);
  ledcAttachPin(pin, channel);
#endif
}

void pwmWrite(int pin, int channel, uint32_t duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  (void)channel;
  ledcWrite(pin, duty);
#else
  (void)pin;
  ledcWrite(channel, duty);
#endif
}

uint32_t servoDutyUs(int knob0to1023) {
  int k = constrain(knob0to1023, 0, 1023);
  return (uint32_t)map(k, 0, 1023, 205, 410);
}

void applyMotor(int throttleNeg100to100, int steerBias) {
  int t = constrain(throttleNeg100to100, -100, 100);
  int s = steerBias;
  int left = constrain(t + s, -100, 100);
  int right = constrain(t - s, -100, 100);

  auto drive = [](int v, int pwmPin, int pwmCh, int dirPin) {
    if (abs(v) < 8) {
      pwmWrite(pwmPin, pwmCh, 0);
      digitalWrite(dirPin, LOW);
      return;
    }
    digitalWrite(dirPin, v >= 0 ? HIGH : LOW);
    pwmWrite(pwmPin, pwmCh, (uint32_t)map(abs(v), 0, 100, 0, 255));
  };

  drive(left, PIN_DRIVE_L_PWM, CH_DRIVE_L, PIN_DRIVE_L_DIR);
  drive(right, PIN_DRIVE_R_PWM, CH_DRIVE_R, PIN_DRIVE_R_DIR);
}
#endif

void applyOutputs() {
#if !TELECON_VEHICLE_IO
  return;
#else
  if (failSafeActive) {
    pwmWrite(PIN_DRIVE_L_PWM, CH_DRIVE_L, 0);
    pwmWrite(PIN_DRIVE_R_PWM, CH_DRIVE_R, 0);
    digitalWrite(PIN_DRIVE_L_DIR, LOW);
    digitalWrite(PIN_DRIVE_R_DIR, LOW);
    digitalWrite(PIN_LIGHTS, LOW);
    digitalWrite(PIN_BUZZER, LOW);
    // Safe idle steer = fixed 1500 µs center.
    pwmWrite(PIN_STEER_SERVO, CH_STEER, steerDutyFromUs(kSteerCenterUs));
    return;
  }

  applyMotor(gState.ly, constrain(gState.rx, -100, 100));
  pwmWrite(PIN_STEER_SERVO, CH_STEER, steerDutyFromRx(gState.rx));
  pwmWrite(PIN_PAN_SERVO, CH_PAN, servoDutyUs(gState.rk));
  digitalWrite(PIN_LIGHTS, (gState.sw & 0x01) ? HIGH : LOW);

  if (buzzerUntilMs != 0 && millis() < buzzerUntilMs) {
    digitalWrite(PIN_BUZZER, HIGH);
  } else {
    digitalWrite(PIN_BUZZER, LOW);
    buzzerUntilMs = 0;
  }
#endif
}

}  // namespace

void rcVehicleBegin() {
#if TELECON_VEHICLE_IO
  pinMode(PIN_DRIVE_L_DIR, OUTPUT);
  pinMode(PIN_DRIVE_R_DIR, OUTPUT);
  pinMode(PIN_LIGHTS, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_STATUS_LED, OUTPUT);
  digitalWrite(PIN_DRIVE_L_DIR, LOW);
  digitalWrite(PIN_DRIVE_R_DIR, LOW);
  digitalWrite(PIN_LIGHTS, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_STATUS_LED, LOW);

  setupPwm(PIN_DRIVE_L_PWM, CH_DRIVE_L, 20000, 8);
  setupPwm(PIN_DRIVE_R_PWM, CH_DRIVE_R, 20000, 8);
  setupPwm(PIN_STEER_SERVO, CH_STEER, 50, 12);
  setupPwm(PIN_PAN_SERVO, CH_PAN, 50, 12);

  pwmWrite(PIN_STEER_SERVO, CH_STEER, steerDutyFromRx(0));
  pwmWrite(PIN_PAN_SERVO, CH_PAN, servoDutyUs(512));
#if TELECON_DEBUG
  DBG_PRINTLN("[IO] RC Vehicle ready");
#endif
#else
#if TELECON_DEBUG
  DBG_PRINTLN("[IO] vehicle GPIO disabled (CAM board / dual-board Option A)");
#endif
#endif

  gState = RcVehicleState{};
  failSafeActive = true;
  applyOutputs();
}

void rcVehicleApply(const RcVehicleState& state) {
  gState = state;
  failSafeActive = false;
  applyOutputs();
}

void rcVehicleHandleBtn(int id) {
  if (id == 2) {
    buzzerUntilMs = millis() + 200;
#if TELECON_VEHICLE_IO
    if (!failSafeActive) digitalWrite(PIN_BUZZER, HIGH);
#endif
  } else if (id == 4) {
    gState.rk = 512;
    gState.rk12 = 512;
#if TELECON_VEHICLE_IO
    if (!failSafeActive) pwmWrite(PIN_PAN_SERVO, CH_PAN, servoDutyUs(512));
#endif
  }
}

void rcVehicleFailSafe() {
  failSafeActive = true;
  gState.lx = gState.ly = gState.rx = gState.ry = 0;
  gState.lx12 = gState.ly12 = gState.rx12 = gState.ry12 = 2048;
  buzzerUntilMs = 0;
  applyOutputs();
#if TELECON_DEBUG
  DBG_PRINTLN("[IO] fail-safe — motors off");
#endif
}

void rcVehicleLoop() {
#if TELECON_VEHICLE_IO
  if (buzzerUntilMs != 0 && millis() >= buzzerUntilMs) {
    buzzerUntilMs = 0;
    digitalWrite(PIN_BUZZER, LOW);
  }
#else
  (void)buzzerUntilMs;
#endif
}

const RcVehicleState& rcVehicleState() {
  return gState;
}

uint16_t rcVehicleSpeedX10() {
  return (uint16_t)(abs(gState.ly) * 25 / 10);
}

uint8_t rcVehicleBatteryPercent() {
#if TELECON_VEHICLE_IO
  int raw = analogRead(PIN_BATT_ADC);
  int pct = map(constrain(raw, 0, 4095), 0, 4095, 0, 100);
  return (uint8_t)constrain(pct, 0, 100);
#else
  // Demo battery when no ADC on CAM board
  return 76;
#endif
}

uint8_t rcVehicleMotorTempAnalog() {
#if TELECON_VEHICLE_IO
  int raw = analogRead(PIN_MOTOR_TEMP_ADC);
  return (uint8_t)map(constrain(raw, 0, 4095), 0, 4095, 0, 255);
#else
  return 90;
#endif
}

uint8_t rcVehicleLedMask() {
  return gState.sw;
}
