#pragma once
#include "sesame_paste.h"
#include "module.h"

#ifdef ENABLE_I2C_DISPLAY

#ifndef ENABLE_I2C
#error "ENABLE_I2C is not defined in config"
#endif

#ifndef DISPLAY_I2C_ADDR
#error "DISPLAY_I2C_ADDR is not defined in config"
#endif

#ifndef DISPLAY_RESET_PIN
#error "DISPLAY_RESET_PIN is not defined in config"
#endif

#ifndef DISPLAY_WIDTH
#error "DISPLAY_WIDTH is not defined in config"
#endif

#ifndef DISPLAY_HEIGHT
#error "DISPLAY_HEIGHT is not defined in config"
#endif

#include <U8g2lib.h>

class DisplayModule : public Module {
  public:
    DisplayModule() : Module() {
        ThreadName = "Display";
    };

  protected:
    void setup() override;
    void loop() override;

    DISPLAY_TYPE * display;

    const uint8_t * faceBitmap;

    void setCursorCentered(int16_t x, int16_t y, int16_t w, int16_t h, const char * text);

    inline void setCursorCenteredLowerColor(const char * text) {
        this->setCursorCentered(0, DISPLAY_COLOR_SHIFT_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - DISPLAY_COLOR_SHIFT_HEIGHT, text);
    }

    inline void printCenteredLowerColor(const char * text) {
        this->setCursorCenteredLowerColor(text);
        this->display->print(text);
    }
};

#endif
