# Ameba Audio

## Table of Contents

- [Ameba Audio](#ameba-audio)
	- [Table of Contents](#table-of-contents)
	- [About ](#about-)
	- [Supported IC ](#supported-ic-)
	- [Configurations ](#configurations-)
		- [Hardware configurations](#hardware-configurations)
	- [How to use ](#how-to-use-)

## About <a name = "about"></a>

Ameba audio project can achieve:
1. audio record.
2. before using this example, please check the application note to see how to choose audio architecture and compile.
3. please check the application note to see how to use interfaces.

## Supported IC <a name = "supported-ic"></a>
1. AmebaSmart
2. AmebaLite
3. AmebaDplus

## Configurations <a name = "configurations"></a>

Please see discriptions in component/soc/usrcfg/xx/include/ameba_audio_hw_usrcfg.h, ameba_audio_hw_usrcfg.h is for audio hardware configurations.

### Hardware configurations

1. Setup the hardware devices, like dmic devices and so on.
2. Define using pll clock or xtal clock for record.
3. Please refer to component/audio/audio_hal/xx/README.md.

## How to use <a name = "How to use"></a>

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

### Note

1. Use CMD `./build.py -a audio_hal_capture -p` to compile this example.
2. Compile and run. For capture run command and parameters, please refer to app_example.c.
3. To run the example, the default command in serial log UART is: AudioHalCapture.

