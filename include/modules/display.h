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

#define ICON_USB_CONNECTED 0xE20E
#define ICON_USB_SERIAL_CONNECTED 0xE00C

#define ICON_POWER_ERROR 0xE20F
#define ICON_CRITICAL_SW_ERROR 0xE077

// Battery 0-9 levels
#define ICON_BATTERY_EMPTY 0xE211
#define ICON_BATTERY_0 0xE242
#define ICON_BATTERY_1 0xE243
#define ICON_BATTERY_2 0xE244
#define ICON_BATTERY_3 0xE245
#define ICON_BATTERY_4 0xE246
#define ICON_BATTERY_5 0xE247
#define ICON_BATTERY_6 0xE248
#define ICON_BATTERY_7 0xE249
#define ICON_BATTERY_8 0xE24A
#define ICON_BATTERY_9 0xE24B

#define ICON_NO_WIFI 0xE217
#define ICON_WIFI_1 0xE218
#define ICON_WIFI_2 0xE219
#define ICON_WIFI_3 0xE21A

#include <U8g2lib.h>
#include <initializer_list>

class DisplayModule : public Module {
  public:
    DisplayModule() : Module() {
        ThreadName = "Display";
    };

  protected:
    void setup() override;
    void loop() override;

    DISPLAY_TYPE * display;

    bool showClock = false;
    bool showIcons = true;

    const uint8_t * faceBitmap;

    void setCursorCentered(int16_t x, int16_t y, int16_t w, int16_t h, const char * text);

    inline void setCursorCenteredLowerColor(const char * text) {
        this->setCursorCentered(0, DISPLAY_COLOR_SHIFT_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - DISPLAY_COLOR_SHIFT_HEIGHT, text);
    }

    inline void printCenteredLowerColor(const char * text) {
        this->setCursorCenteredLowerColor(text);
        this->display->print(text);
    }

    void renderIcons();
    void renderIcons(std::forward_list<uint16_t> icons);
};

#endif
