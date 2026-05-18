#pragma once

#ifndef __has_include
#define __has_include(x) 0
#warning "__has_include not supported"
#else
#include "user_config.h"
#endif

#if __has_include("user_config.h")
#include "user_config.h"
#else
#warning "user_config.h not found"
#endif

// #define ENABLE_WEBSOCKETS

#if defined(BUTTON_PINS)
#define ENABLE_BUTTON

#ifndef BUTTON_DEBOUNCE_DELAY
#define BUTTON_DEBOUNCE_DELAY 10
#endif

#endif    // BUTTON_PINS

#if defined(I2C_SDA) && defined(I2C_SCL)
#define ENABLE_I2C

#ifndef I2C_CLOCK_SPEED
#define I2C_CLOCK_SPEED 400000
#endif

// OLED Display Config
#define ENABLE_SSD1306

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 128
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 64
#endif

#ifndef SCREEN_COLOR_SHIFT_HEIGHT
#define SCREEN_COLOR_SHIFT_HEIGHT 16
#endif

#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR 0x3C
#endif

#ifndef SSD1306_RESET_PIN
#define SSD1306_RESET_PIN -1
#endif

#endif    // I2C_SDA && I2C_SCL

#ifndef USE_THREAD_NAMES
#error "USE_THREAD_NAMES must be defined in platformio.ini build_flags"
#endif

#ifdef ENABLE_OTA
#ifndef OTA_PASSWORD
#warning "OTA_PASSWORD is not defined this is not secure, please define OTA_PASSWORD in user_config.h"
#endif
#endif

#if defined(ARDUINO_USB_CDC_ON_BOOT) && ARDUINO_USB_CDC_ON_BOOT
#define IS_USB_SERIAL
#endif
