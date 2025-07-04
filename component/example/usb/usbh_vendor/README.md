# Example Description

In this application, Ameba is designed as a vendor-specific USB host as a design reference for users to design customized USB applications. The USB vendor-specific host can communicate with vendor-specific USB device as defined in `usbd_vendor` application.

Currently it is only for RD internal debug usage and not available as default.

# HW Configuration

Some Ameba boards need to be reworked to support USB host, refer to AN for details.
Taking RTL8730EA/QFN100 boards for example, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS.

# SW configuration

1. Menuconfig
	Type command `./menuconfig.py` under the project directory and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	Vendor
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[VND] USBH vendor demo start
	```
2. Connect the vendor-specific USB device (e.g. another Ameba board running usbd_vendor_new application) to the USB port of the board with USB cable.

3. Reset and check the test log via LOGUART console, make sure there is no error reported and the transfer success rate is reasonable, e.g.:
	```
	INTR test result: IN/OUT = 100/100:
	0   1   2   3   4   5   6   7   8   9
	...
	90  91  92  93  94  95  96  97  98  99
	
	ISOC test result: IN/OUT = 90/100:
	0   1   2   3   4   5   6   7   8   9
	...
	87  88  90  91  92  93  95  96  97  99
	```

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

- RTL872XD
- RTL8720F
- RTL8721F
- RTL8730E
- RTL8730F
