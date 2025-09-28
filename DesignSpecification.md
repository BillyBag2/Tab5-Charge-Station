# Design specification

- [ ] Target the M5Stack TAB5.
- [ ] Leverage the standard libraries M5Unified, and M5GFX where possible.
- [ ] Create a display on the TAB5 screen indication the various available
      voltages and currents associated with charging and running the device.
- [ ] Offer the ability to turn on or off charging using a Display GUI.
- [ ] Offer the ability to change the charge rate.
- [ ] If power charging power is lost and then restored the TAB5 should continue
      charging in the same way as before the power loss.
- [ ] Estimate the percentage battery charge state from the battery voltage.
- [ ] Use the Arduino IDE.
- [ ] Code in c++.
- [ ] Create separate source files for different features of the code and
      include files with APIs to those features.
- [ ] Follow the Arduino IDE coding style.

## Questions

- What GPIO or power management primitive should be used on the TAB5 to truly enable/disable the charger hardware?
- Are there validated slow/normal/fast charge current targets we should enforce for battery longevity on this design?
- Should the display also surface input power metrics (USB/solar/etc.) or logging beyond the live telemetry currently shown?
