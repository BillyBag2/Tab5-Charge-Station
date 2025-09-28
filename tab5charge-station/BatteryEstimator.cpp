#include "BatteryEstimator.h"

#include <algorithm>

#include <M5Unified.h>

void BatteryEstimator::begin() {
  // Nothing required for now, but provides symmetry with other modules.
}

BatteryStatus BatteryEstimator::sample() const {
  BatteryStatus status;

  float raw_voltage = M5.Power.getBatteryVoltage();
  status.batteryVoltage = (raw_voltage > 20.0f) ? (raw_voltage * 0.001f) : raw_voltage;

  status.batteryCurrent = M5.Power.getBatteryCurrent();

  // Heuristic: positive current means charging.
  status.isCharging = status.batteryCurrent > 10.0f;  // mA threshold to suppress noise.

  status.batteryPercent = voltageToPercentage(status.batteryVoltage);
  return status;
}

float BatteryEstimator::voltageToPercentage(float voltage) const {
  if (max_voltage_ <= min_voltage_) {
    return 0.0f;
  }
  float clamped = std::clamp(voltage, min_voltage_, max_voltage_);
  float percent = ((clamped - min_voltage_) / (max_voltage_ - min_voltage_)) * 100.0f;
  return std::clamp(percent, 0.0f, 100.0f);
}

void BatteryEstimator::setVoltageRange(float min_voltage, float max_voltage) {
  min_voltage_ = min_voltage;
  max_voltage_ = max_voltage;
}
