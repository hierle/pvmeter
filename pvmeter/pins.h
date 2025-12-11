#pragma once
//
// pins.h
//

// pin mappings / GPIO#

// status led
#define statusled 4 // new

// sensor / transformer
// ADC1 (ADC2 not usable w/ wifi)
#define transformer 3

// TFT display SPI
#define TFT_CS         10
#define TFT_RST        9
#define TFT_DC         7
#define TFT_SCK          12
#define TFT_MOSI         13
//#define TFT_MISO         11
#define TFT_BL  14
//#define TFT_BL  1
