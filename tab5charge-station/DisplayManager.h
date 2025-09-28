#pragma once

#include <M5Unified.h>

#include "BatteryEstimator.h"
#include "ChargeController.h"

struct ChargerUiState {
  bool chargingEnabled = true;
  ChargeRate chargeRate = ChargeRate::kNormal;
  BatteryStatus battery;
  uint16_t targetCurrent = 0;  // milliamps
};

enum class UiAction {
  kNone = 0,
  kToggleCharge = 1,
  kSetRateSlow = 2,
  kSetRateNormal = 3,
  kSetRateFast = 4,
};

class DisplayManager {
 public:
  void begin();

  void render(const ChargerUiState& state);

  UiAction processInput();

 private:
  struct Button {
    int16_t x = 0;
    int16_t y = 0;
    int16_t w = 0;
    int16_t h = 0;

    bool contains(uint16_t px, uint16_t py) const {
      return px >= x && px < (x + w) && py >= y && py < (y + h);
    }
  };

  void drawButton(const Button& button,
                  uint16_t fill_color,
                  const char* label,
                  bool selected = false,
                  uint16_t border_color = TFT_WHITE);

  Button toggle_button_;
  Button rate_buttons_[3];
  bool touch_active_ = false;
};
