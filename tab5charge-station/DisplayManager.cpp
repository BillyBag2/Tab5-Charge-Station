#include "DisplayManager.h"

#include <M5GFX.h>

namespace {
constexpr uint16_t kBackgroundColor = TFT_BLACK;
constexpr uint16_t kTextColor = TFT_WHITE;
constexpr uint16_t kButtonEnabledColor = TFT_OLIVE;
constexpr uint16_t kButtonDisabledColor = TFT_DARKGREY;
constexpr uint16_t kButtonSecondaryColor = TFT_NAVY;
constexpr uint16_t kButtonBorderColor = TFT_WHITE;
constexpr int16_t kCornerRadius = 14;
constexpr int16_t kSelectedBorderPadding = 4;
constexpr int16_t kButtonHeight = 70;
constexpr int16_t kButtonMargin = 20;
constexpr int16_t kButtonGap = 16;
}

void DisplayManager::begin() {
  auto& display = M5.Display;
  display.setRotation(1);
  display.fillScreen(kBackgroundColor);
  display.setFont(&lgfx::v1::fonts::FreeSansBold18pt7b);
  display.setTextSize(1);
  display.setTextColor(kTextColor, kBackgroundColor);

  const int16_t screen_width = display.width();

  const int16_t screen_height = display.height();

  const int16_t available_width = screen_width - (2 * kButtonMargin);
  const int16_t total_gap = 3 * kButtonGap;
  const int16_t button_width = (available_width - total_gap) / 4;
  const int16_t bottom_y = screen_height - kButtonMargin - kButtonHeight;

  toggle_button_.x = kButtonMargin;
  toggle_button_.y = bottom_y;
  toggle_button_.w = button_width;
  toggle_button_.h = kButtonHeight;

  for (int i = 0; i < 3; ++i) {
    rate_buttons_[i].x = toggle_button_.x + (button_width + kButtonGap) * (i + 1);
    rate_buttons_[i].y = bottom_y;
    rate_buttons_[i].w = button_width;
    rate_buttons_[i].h = kButtonHeight;
  }
}

void DisplayManager::render(const ChargerUiState& state) {
  auto& display = M5.Display;
  display.waitDisplay();
  display.startWrite();
  display.fillScreen(kBackgroundColor);

  display.setFont(&lgfx::v1::fonts::FreeSansBold18pt7b);
  display.setTextSize(1);
  display.setTextColor(kTextColor, kBackgroundColor);

  int cursor_y = 40;
  display.setCursor(20, cursor_y);
  display.printf("Charging: %s", state.chargingEnabled ? "On" : "Off");

  cursor_y += 55;
  display.setCursor(20, cursor_y);
  display.printf("Rate/Target: %s / %u mA", ChargeRateToString(state.chargeRate),
                 static_cast<unsigned>(state.targetCurrent));

  cursor_y += 55;
  display.setCursor(20, cursor_y);
  display.printf("Battery: %.0f %% (%s)", state.battery.batteryPercent,
                 state.battery.isCharging ? "Charging" : "Idle");

  cursor_y += 55;
  display.setCursor(20, cursor_y);
  display.printf("Volt/Current: %.2f V / %.0f mA", state.battery.batteryVoltage,
                 state.battery.batteryCurrent);

  drawButton(toggle_button_,
             state.chargingEnabled ? kButtonEnabledColor : kButtonDisabledColor,
             state.chargingEnabled ? "Disable" : "Enable");

  const char* rate_labels[3] = {"Slow", "Normal", "Fast"};
  const ChargeRate rate_values[3] = {ChargeRate::kSlow, ChargeRate::kNormal,
                                     ChargeRate::kFast};

  for (int i = 0; i < 3; ++i) {
    const bool selected = (state.chargeRate == rate_values[i]);
    drawButton(rate_buttons_[i], kButtonSecondaryColor, rate_labels[i], selected,
               kButtonBorderColor);
  }

  display.endWrite();
  display.waitDisplay();
}

UiAction DisplayManager::processInput() {
  uint16_t x = 0;
  uint16_t y = 0;
  if (M5.Display.getTouch(&x, &y)) {
    if (touch_active_) {
      return UiAction::kNone;
    }
    touch_active_ = true;

    if (toggle_button_.contains(x, y)) {
      return UiAction::kToggleCharge;
    }

    if (rate_buttons_[0].contains(x, y)) {
      return UiAction::kSetRateSlow;
    }
    if (rate_buttons_[1].contains(x, y)) {
      return UiAction::kSetRateNormal;
    }
    if (rate_buttons_[2].contains(x, y)) {
      return UiAction::kSetRateFast;
    }
  } else {
    touch_active_ = false;
  }
  return UiAction::kNone;
}

void DisplayManager::drawButton(const Button& button,
                                uint16_t fill_color,
                                const char* label,
                                bool selected,
                                uint16_t border_color) {
  auto& display = M5.Display;

  if (selected) {
    display.fillRoundRect(button.x - kSelectedBorderPadding,
                          button.y - kSelectedBorderPadding,
                          button.w + (kSelectedBorderPadding * 2),
                          button.h + (kSelectedBorderPadding * 2),
                          kCornerRadius + kSelectedBorderPadding, border_color);
  }

  display.fillRoundRect(button.x, button.y, button.w, button.h, kCornerRadius, fill_color);
  display.drawRoundRect(button.x, button.y, button.w, button.h, kCornerRadius, border_color);

  display.setFont(&lgfx::v1::fonts::FreeSansBold12pt7b);
  display.setTextDatum(textdatum_t::middle_center);
  display.setTextColor(TFT_WHITE, fill_color);
  display.drawString(label, button.x + button.w / 2, button.y + button.h / 2);

  display.setFont(&lgfx::v1::fonts::FreeSansBold18pt7b);
  display.setTextDatum(textdatum_t::top_left);
  display.setTextColor(kTextColor, kBackgroundColor);
}
