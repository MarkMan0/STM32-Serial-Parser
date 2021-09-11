# STM32-Serial-Parser

## Motivation
I chose to create this project to learn the following things:
 + STM32 development using HAL library
 + FreeRTOS
 + Developing in PlatformIO IDE
 + PlatformIO testing
 + Writing documentation (doxygen)
 + Automatic code formatting (clang-format)
 + Driver development

## Hardware
### MCU
The firmware runs on a [NUCLEO-F303K8](https://www.st.com/en/evaluation-tools/nucleo-f303k8.html) board. The board has an integrated STLink debugger
### Display
Popular OLED 128x64 monochrome OLED display with SSD1306 driver over I2C. [Example](https://www.aliexpress.com/item/32896971385.html?spm=a2g0o.search0304.0.0.68653754I8wojC&algo_pvid=1b2b944c-7cae-4e4c-afc9-6fdb74c5257a&algo_exp_id=1b2b944c-7cae-4e4c-afc9-6fdb74c5257a-2)

### RTC
Popular DS3231 RTC. [Example](https://www.aliexpress.com/item/32957753268.html?spm=a2g0o.productlist.0.0.5d4c238eb4dIhM&algo_pvid=bec2c0ce-de49-4c1e-9261-f42a6e407671&algo_exp_id=bec2c0ce-de49-4c1e-9261-f42a6e407671-0&pdp_ext_f=%7B%22sku_id%22%3A%2266447210279%22%7D)


## Highlights
+ No external libraries other than ST HAL
+ Unit tests for certain parts of the code
+ printf-like format for UART communication, using C libraries
+ printf-like format for the OLED display using my own implementation
+ GCode parser for UART communication
+ Buffered UART TX, which can be used from ISRs
+ Automating formatting using *clang-format*
+ Documentation using Doxygen
