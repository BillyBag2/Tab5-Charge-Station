#include "ChargeController.h"

#include <algorithm>

namespace {
constexpr const char* kPrefsNamespace = "charge";
constexpr const char* kPrefsEnabledKey = "enabled";
constexpr const char* kPrefsRateKey = "rate";
constexpr uint16_t kSlowCurrentmA = 500;
constexpr uint16_t kNormalCurrentmA = 1000;
constexpr uint16_t kFastCurrentmA = 2000;
}

const char* ChargeRateToString(ChargeRate rate) {
  switch (rate) {
    case ChargeRate::kSlow:
      return "Slow";
    case ChargeRate::kNormal:
      return "Normal";
    case ChargeRate::kFast:
      return "Fast";
    default:
      return "Unknown";
  }
}

void ChargeController::begin() {
  prefs_.begin(kPrefsNamespace, false /* readOnly */);

  charge_enabled_ = prefs_.getBool(kPrefsEnabledKey, true);
  auto stored_rate = static_cast<ChargeRate>(prefs_.getUChar(
      kPrefsRateKey, static_cast<uint8_t>(ChargeRate::kNormal)));  
  switch (stored_rate) {
    case ChargeRate::kSlow:
    case ChargeRate::kNormal:
    case ChargeRate::kFast:
      charge_rate_ = stored_rate;
      break;
    default:
      charge_rate_ = ChargeRate::kNormal;
      break;
  }

  applyChargeSettings();
}

void ChargeController::setChargingEnabled(bool enable) {
  if (charge_enabled_ == enable) {
    return;
  }
  charge_enabled_ = enable;
  applyChargeSettings();
  persistState();
}

void ChargeController::setChargeRate(ChargeRate rate) {
  if (charge_rate_ == rate) {
    return;
  }
  charge_rate_ = rate;
  applyChargeSettings();
  persistState();
}

ChargeRate ChargeController::cycleChargeRate() {
  switch (charge_rate_) {
    case ChargeRate::kSlow:
      setChargeRate(ChargeRate::kNormal);
      break;
    case ChargeRate::kNormal:
      setChargeRate(ChargeRate::kFast);
      break;
    case ChargeRate::kFast:
    default:
      setChargeRate(ChargeRate::kSlow);
      break;
  }
  return charge_rate_;
}

uint16_t ChargeController::targetChargeCurrent() const {
  switch (charge_rate_) {
    case ChargeRate::kSlow:
      return kSlowCurrentmA;
    case ChargeRate::kFast:
      return kFastCurrentmA;
    case ChargeRate::kNormal:
    default:
      return kNormalCurrentmA;
  }
}

void ChargeController::applyChargeSettings() {
  const uint16_t current = charge_enabled_ ? targetChargeCurrent() : 0;
  M5.Power.setChargeCurrent(current);
}

void ChargeController::persistState() {
  prefs_.putBool(kPrefsEnabledKey, charge_enabled_);
  prefs_.putUChar(kPrefsRateKey, static_cast<uint8_t>(charge_rate_));
}
