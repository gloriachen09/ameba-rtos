# Example Description

This example describes how to get ADC sample data in vbat channel by mbed API.

# HW Configuration

In this example, ADC is configured to sample data in `vbat channel`.

* On AmebaSmart
	- Connect `VBAT_MEAS` to `to-be-measured signal`
	- Connect `GND` to `GND` of to-be-measured device
* On AmebaDplus
	- Connect `VBAT_MEAS` to `to-be-measured signal`
	- Connect `GND` to `GND` of to-be-measured device

For more info of `VBAT_MEAS`, refer to EVB schematic design file.

# SW Configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

ADC sample data and calculated voltage value in `mV` would be printed out `periodically`.

# Note

1. Input voltage of vbat channel: `0 ~ 5V`.
2. Vbat channel `is not supported` on `AmebaLite`.

# Supported IC

* AmebaSmart
* AmebaDplus


