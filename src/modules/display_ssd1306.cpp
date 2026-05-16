#include "modules/display_ssd1306.h"

#include <Wire.h>

#ifdef ENABLE_SSD1306

void DisplaySSD1306Module::setup() {
    uint16_t errors = 0;

    Serial.println(F("Initializing SSD1306 display..."));
    this->display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SSD1306_RESET_PIN);

    // Test and init if display exists on I2C bus.
    while(1) {
        Wire.beginTransmission(SSD1306_I2C_ADDR);
        if(Wire.endTransmission() == 0) {
            if(this->display.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDR)) {
                break;
            } else {
                Serial.println(F("SSD1306 found but failed"));
            }
        }
        if(errors++ > 50) {
            this->enabled = false;
            Serial.println(F("SSD1306 failed, giving up."));
            return;
        }
        Serial.printf(F("SSD1306 (0x%02X) failed, %d retrying...\n"), SSD1306_I2C_ADDR, errors);
        delay(1000);
    }

    this->display.clearDisplay();
    this->display.setTextColor(SSD1306_WHITE);
    this->display.setTextSize(2);
    this->display.setCursor(0, 0);
    this->display.println(F("Booting..."));
    this->display.display();

    Serial.println(F("Initializing SSD1306 display... done"));
    this->displayFace(FACE_ID_idle_blink, FACE_ANIM_ONCE);
}

void DisplaySSD1306Module::loop() {
    // Serial.printf(F("DisplaySSD1306Module loop: faceId: %d, mode: %d, step: %d\n"), this->currentFaceId, this->currentFaceMode, this->currentFaceStep);
    const FaceEntry & entry = faceEntries[this->currentFaceId];
    if(entry.frames == nullptr || entry.frames[0] == nullptr) {
        this->enabled  = false;
        this->canSleep = true;
        return;
    }

    bitmap_t * bitmap = entry.frames[this->currentFaceStep];
    if(bitmap == nullptr) {
        this->currentFaceStep = 0;
        bitmap                = entry.frames[0];
    }
    this->drawBitmap(bitmap);

    if(this->currentFaceMode == FACE_ANIM_ONCE) {
        if(this->currentFaceStep >= entry.maxFrames - 1 || entry.frames[this->currentFaceStep + 1] == nullptr) {
            this->faceAnimFinished = true;
        } else {
            this->currentFaceStep++;
        }
    } else if(this->currentFaceMode == FACE_ANIM_LOOP) {
        this->currentFaceStep = (this->currentFaceStep + 1) % entry.maxFrames;
    } else if(this->currentFaceMode == FACE_ANIM_BOOMERANG) {
        if(faceAnimationDirectionReverse) {
            if(this->currentFaceStep == 0) {
                faceAnimationDirectionReverse = false;
                this->currentFaceStep++;
            } else {
                this->currentFaceStep--;
            }
        } else {
            if(this->currentFaceStep >= entry.maxFrames - 1 || entry.frames[this->currentFaceStep + 1] == nullptr) {
                faceAnimationDirectionReverse = true;
                this->currentFaceStep--;
            } else {
                this->currentFaceStep++;
            }
        }
    }

    if(entry.frames[this->currentFaceStep] == nullptr) {
        this->currentFaceStep = 0;
    }

    if(this->faceAnimFinished) {
        this->enabled  = false;
        this->canSleep = true;
    } else {
        this->enabled  = true;
        this->canSleep = false;
    }
}

void DisplaySSD1306Module::displayFace(FaceID faceId, FaceAnimMode mode) {
    if(faceId >= FACE_ID_MAX) {
        faceId = FACE_ID_defualt;
    }
    this->currentFaceId                 = faceId;
    this->currentFaceMode               = mode;
    this->currentFaceStep               = 0;
    this->faceAnimationDirectionReverse = false;
    this->faceAnimFinished              = false;

    // Calculate interval based on fps for the face
    unsigned long interval = 1000;    // 1 fps
    if(faceEntries[faceId].fps > 0) {
        interval = 1000 / faceEntries[faceId].fps;
    }

    const FaceEntry & entry = faceEntries[this->currentFaceId];

    Serial.printf(F("Displaying face: %s(%d) mode: %d interval: %lu ms\n"), entry.name, faceId, mode, interval);
    this->setInterval(interval);
    this->enabled  = true;
    this->canSleep = false;
};

void DisplaySSD1306Module::drawBitmap(bitmap_t * bitmap, uint16_t width, uint16_t height) {
    if(bitmap == nullptr) return;
    this->display.clearDisplay();
    this->display.drawBitmap(0, 0, bitmap, width, height, SSD1306_WHITE);
    this->display.display();
}

#endif
