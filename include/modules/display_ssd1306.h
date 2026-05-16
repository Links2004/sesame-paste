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

class DisplaySSD1306Module : public Module, public FaceDisplay {
  public:
    DisplaySSD1306Module() : Module() {
        ThreadName = "Display";
    };

    void displayFace(FaceID faceId, FaceAnimMode mode = FACE_ANIM_LOOP) override;
    bool isAnimationFinished() override {
        return this->faceAnimFinished;
    }
    FaceID getCurrentFaceID() override {
        return this->currentFaceId;
    }

  protected:
    void setup() override;
    void loop() override;
    void drawBitmap(bitmap_t * bitmap, uint16_t width = SCREEN_WIDTH, uint16_t height = SCREEN_HEIGHT);

    FaceID currentFaceId;
    FaceAnimMode currentFaceMode;

    int8_t currentFaceStep;
    bool faceAnimationDirectionReverse;
    bool faceAnimFinished;

    Adafruit_SSD1306 display;
};

#endif
