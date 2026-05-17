#pragma once
#include "sesame_paste.h"
#include "module.h"

#ifdef ENABLE_SSD1306

#ifndef ENABLE_I2C
#error "ENABLE_I2C is not defined in config"
#endif

#ifndef SSD1306_I2C_ADDR
#error "SSD1306_I2C_ADDR is not defined in config"
#endif

#ifndef SSD1306_RESET_PIN
#error "SSD1306_RESET_PIN is not defined in config"
#endif

#ifndef SCREEN_WIDTH
#error "SCREEN_WIDTH is not defined in config"
#endif

#ifndef SCREEN_HEIGHT
#error "SCREEN_HEIGHT is not defined in config"
#endif

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class DisplaySSD1306Module : public Module {
  public:
    DisplaySSD1306Module() : Module() {
        ThreadName = "Display";
    };

  protected:
    void setup() override;
    void loop() override;

    Adafruit_SSD1306 display;

    const uint8_t * faceBitmap;
};

#endif
