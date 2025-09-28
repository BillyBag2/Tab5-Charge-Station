# Tab5-Charge-Station

See: [Design Specification](DesignSpecification.md)

## A simple charging monitor/controller for the M5Stack Tab5

### Enable/Disable charging

With it disabled the battery does not charge. The 5V supply is used if
available. With no 5V supply the battery discharges.

### Rate (Untested)

A selection of charge rates. Recommend adjusting without power connected.

**Make sure the charge rate is supported by your battery and power supply.**

### Display

* Charging: On/Off
* Rate and target current.
* Estimate of battery status.
* Battery volts/current/watts. (Negative is discharging.)

## Building

Designed for Arduino-CLI and Arduino-IDE.

### On Linux

* `comp.sh` with build using the CLI.
* `flash.sh` will program the TAB5 over USB.
