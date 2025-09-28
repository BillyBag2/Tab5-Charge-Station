#include "BatteryEstimator.h"

#include <algorithm>

#include <M5Unified.h>
#include <utility/Power_Class.hpp>

void BatteryEstimator::begin() {
  // Nothing required for now, but provides symmetry with other modules.
}

BatteryStatus BatteryEstimator::sample() const {
  BatteryStatus status;

  float raw_voltage = M5.Power.getBatteryVoltage();
  status.batteryVoltage = (raw_voltage > 20.0f) ? (raw_voltage * 0.001f) : raw_voltage;

  float raw_current = M5.Power.getBatteryCurrent();
  status.batteryCurrent = -raw_current;  // Invert so charging currents read positive.
  status.batteryWatts = (status.batteryVoltage * status.batteryCurrent) / 1000.0f;

  // Heuristic: positive current means charging.
  status.isCharging = status.batteryCurrent > 10.0f;  // mA threshold to suppress noise.

  const auto pmic_type = M5.Power.getType();
  if (pmic_type == m5::Power_Class::pmic_axp2101 ||
      pmic_type == m5::Power_Class::pmic_axp192) {
    const float vbus_mv = static_cast<float>(M5.Power.getVBUSVoltage());
    status.vbusVoltage = vbus_mv > 0.0f ? vbus_mv * 0.001f : 0.0f;

    float vbus_ma = 0.0f;
#if !defined(CONFIG_IDF_TARGET_ESP32C3) && !defined(CONFIG_IDF_TARGET_ESP32C6) && !defined(CONFIG_IDF_TARGET_ESP32P4)
    if (pmic_type == m5::Power_Class::pmic_axp2101) {
      vbus_ma = M5.Power.Axp2101.getVBUSCurrent();
    }
#endif
#if !defined(CONFIG_IDF_TARGET_ESP32S3) && !defined(CONFIG_IDF_TARGET_ESP32C3) && !defined(CONFIG_IDF_TARGET_ESP32C6) && !defined(CONFIG_IDF_TARGET_ESP32P4)
    if (pmic_type == m5::Power_Class::pmic_axp192) {
      vbus_ma = M5.Power.Axp192.getVBUSCurrent();
    }
#endif
    status.vbusCurrent = vbus_ma;
    status.vbusWatts = status.vbusVoltage * (status.vbusCurrent / 1000.0f);
  } else {
    status.vbusVoltage = 0.0f;
    status.vbusCurrent = 0.0f;
    status.vbusWatts = 0.0f;
  }

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
