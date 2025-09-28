#include <M5Unified.h>

#include "BatteryEstimator.h"
#include "ChargeController.h"
#include "DisplayManager.h"

namespace {
constexpr uint32_t kDisplayRefreshMs = 500;

ChargeController g_charge_controller;
BatteryEstimator g_battery_estimator;
DisplayManager g_display_manager;
ChargerUiState g_ui_state;

uint32_t g_last_render_ms = 0;

void refreshUi() {
  g_ui_state.chargingEnabled = g_charge_controller.isChargingEnabled();
  g_ui_state.chargeRate = g_charge_controller.getChargeRate();
  g_ui_state.targetCurrent = g_ui_state.chargingEnabled
                                 ? g_charge_controller.targetChargeCurrent()
                                 : 0;
  g_ui_state.battery = g_battery_estimator.sample();
  g_ui_state.targetWatts = (g_ui_state.targetCurrent * g_ui_state.battery.batteryVoltage) / 1000.0f;
  g_display_manager.render(g_ui_state);
  g_last_render_ms = millis();
}

void handleUiAction(UiAction action) {
  if (action == UiAction::kNone) {
    return;
  }

  switch (action) {
    case UiAction::kToggleCharge:
      g_charge_controller.setChargingEnabled(
          !g_charge_controller.isChargingEnabled());
      break;
    case UiAction::kSetRateSlow:
      g_charge_controller.setChargeRate(ChargeRate::kSlow);
      break;
    case UiAction::kSetRateNormal:
      g_charge_controller.setChargeRate(ChargeRate::kNormal);
      break;
    case UiAction::kSetRateFast:
      g_charge_controller.setChargeRate(ChargeRate::kFast);
      break;
    case UiAction::kNone:
    default:
      break;
  }

  refreshUi();
}
}  // namespace

void setup() {
  auto config = M5.config();
  config.serial_baudrate = 115200;
  config.output_power = true;

  M5.begin(config);
  M5.Display.setBrightness(200);

  g_charge_controller.begin();
  g_battery_estimator.begin();
  g_display_manager.begin();

  refreshUi();
}

void loop() {
  M5.update();

  handleUiAction(g_display_manager.processInput());

  const uint32_t now = millis();
  if (now - g_last_render_ms >= kDisplayRefreshMs) {
    refreshUi();
  }
}
