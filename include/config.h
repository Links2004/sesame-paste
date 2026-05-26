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

#define ENABLE_WEBSERVER

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
#define ENABLE_I2C_DISPLAY

#ifndef DISPLAY_WIDTH
#define DISPLAY_WIDTH 128
#endif

#ifndef DISPLAY_HEIGHT
#define DISPLAY_HEIGHT 64
#endif

#ifndef DISPLAY_COLOR_SHIFT_HEIGHT
#define DISPLAY_COLOR_SHIFT_HEIGHT 16
#endif

#ifndef DISPLAY_I2C_ADDR
#define DISPLAY_I2C_ADDR 0x3C
#endif

#ifndef DISPLAY_RESET_PIN
#define DISPLAY_RESET_PIN U8X8_PIN_NONE
#endif

#ifndef DISPLAY_TYPE
#define DISPLAY_TYPE U8G2_SSD1306_128X64_NONAME_F_HW_I2C
#endif

#ifndef FONT_7
#define FONT_7 u8g2_font_5x7_mf
#endif

#ifndef FONT_16
#define FONT_16 u8g2_font_9x15B_tf
#endif

#ifndef FONT_32
#define FONT_32 u8g2_font_logisoso20_tr
#endif

#ifndef FONT_48
#define FONT_48 u8g2_font_logisoso32_tr
#endif

#ifndef FONT_ICONS_12
#define FONT_ICONS_12 u8g2_font_siji_t_6x10
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

#ifndef NTP_SERVER_1
#define NTP_SERVER_1 "0.pool.ntp.org"
#endif

#ifndef NTP_SERVER_2
#define NTP_SERVER_2 "1.pool.ntp.org"
#endif

#ifndef NTP_SERVER_3
#define NTP_SERVER_3 "2.pool.ntp.org"
#endif

#ifndef NTP_TIMEZONE
#define NTP_TIMEZONE "UTC"
#endif

#if defined(SERVO_POWER_EN_PIN) && !defined(SERVO_POWER_EN_ACTIVE)
#define SERVO_POWER_EN_ACTIVE HIGH
#endif

#ifndef SERVO_SHUTDOWN_TIMEOUT_MS
#define SERVO_SHUTDOWN_TIMEOUT_MS 10000
#endif

#ifndef SERVO_SHUTDOWN_POWEROFF_TIMEOUT_MS
#define SERVO_SHUTDOWN_POWEROFF_TIMEOUT_MS 2000
#endif

#if defined(CABLE_CONNECTED_PIN) && !defined(CABLE_CONNECTED_ACTIVE)
#define CABLE_CONNECTED_ACTIVE HIGH
#endif

#if defined(BATTERY_ADC_PIN) && !defined(ENABLE_BATTERY)
#define ENABLE_BATTERY
#endif

#ifndef BATTERY_CRITICAL_THRESHOLD
#define BATTERY_CRITICAL_THRESHOLD 10
#endif

#ifndef BATTERY_LOW_THRESHOLD
#define BATTERY_LOW_THRESHOLD 20
#endif

#ifndef BATTERY_NOT_CONNECTED_THRESHOLD
#define BATTERY_NOT_CONNECTED_THRESHOLD 1.0f
#endif

#ifndef BATTERY_LIPO_CELLS
#define BATTERY_LIPO_CELLS 1
#endif
