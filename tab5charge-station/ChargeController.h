#pragma once

#include <Preferences.h>
#include <stdint.h>

#include <M5Unified.h>

// Represents available charge rates for the TAB5 charger control.
enum class ChargeRate : uint8_t {
  kSlow = 0,
  kNormal = 1,
  kFast = 2,
};

// Maps a charge rate to a human readable string.
const char* ChargeRateToString(ChargeRate rate);

class ChargeController {
 public:
  void begin();

  void setChargingEnabled(bool enable);
  bool isChargingEnabled() const { return charge_enabled_; }

  void setChargeRate(ChargeRate rate);
  ChargeRate getChargeRate() const { return charge_rate_; }

  // Convenience helper to cycle between the available charge rates.
  ChargeRate cycleChargeRate();

  // Target charge current in milliamps based on the current charge rate.
  uint16_t targetChargeCurrent() const;

 private:
  void applyChargeSettings();
  void persistState();

  bool charge_enabled_ = true;
  ChargeRate charge_rate_ = ChargeRate::kNormal;
  Preferences prefs_;
};
