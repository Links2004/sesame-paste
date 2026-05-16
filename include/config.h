#pragma once

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

#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR 0x3C
#endif

#ifndef SSD1306_RESET_PIN
#define SSD1306_RESET_PIN -1
#endif

#endif    // I2C_SDA && I2C_SCL
