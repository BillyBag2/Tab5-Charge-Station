#pragma once

#include <stdint.h>

struct BatteryStatus {
  float batteryVoltage = 0.0f;   // Volts
  float batteryCurrent = 0.0f;   // Milliamps (positive when charging)
  float batteryPercent = 0.0f;   // Percentage 0-100
  bool isCharging = false;
};

class BatteryEstimator {
 public:
  void begin();

  BatteryStatus sample() const;

  float voltageToPercentage(float voltage) const;

  void setVoltageRange(float min_voltage, float max_voltage);

 private:
  float min_voltage_ = 3.3f;
  float max_voltage_ = 4.2f;
};
