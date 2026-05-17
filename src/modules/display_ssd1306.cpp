#include "modules/display_ssd1306.h"

#include <Wire.h>

#ifdef ENABLE_SSD1306

void DisplaySSD1306Module::setup() {
    uint16_t errors = 0;

    ESP_LOGI(this->ThreadName.c_str(), "Initializing SSD1306 display...");
    this->display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SSD1306_RESET_PIN);

    // Test and init if display exists on I2C bus.
    while(1) {
        Wire.beginTransmission(SSD1306_I2C_ADDR);
        if(Wire.endTransmission() == 0) {
            if(this->display.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDR)) {
                break;
            } else {
                ESP_LOGW(this->ThreadName.c_str(), "SSD1306 found but failed");
            }
        }
        if(errors++ > 50) {
            this->enabled = false;
            ESP_LOGE(this->ThreadName.c_str(), "SSD1306 failed, giving up.");
            return;
        }
        ESP_LOGW(this->ThreadName.c_str(), "SSD1306 (0x%02X) failed, %d retrying...", SSD1306_I2C_ADDR, errors);
        delay(1000);
    }

    this->display.clearDisplay();
    this->display.setTextColor(SSD1306_WHITE);
    this->display.setTextSize(2);
    this->display.setCursor(0, 0);
    this->display.println(F("Booting..."));
    this->display.display();

    ESP_LOGI(this->ThreadName.c_str(), "Initializing SSD1306 display... done");
    // this->displayFace(FACE_ID_idle_blink, FACE_ANIM_ONCE);
    this->displayFace(FACE_ID_rest, FACE_ANIM_BOOMERANG);
}

void DisplaySSD1306Module::loop() {
    const FaceEntry & entry = faceEntries[this->currentFaceId];
    ESP_LOGV(this->ThreadName.c_str(), "loop: face: %s (%d), mode: %d, step: %d, max: %d", entry.name, this->currentFaceId, this->currentFaceMode, this->currentFaceStep, entry.maxFrames);
    if(entry.maxFrames <= 0) {
        this->enabled  = false;
        this->canSleep = true;
        return;
    }

    bitmap_t * bitmap = entry.frames[this->currentFaceStep];
    this->drawBitmap(bitmap);

    if(this->currentFaceMode == FACE_ANIM_ONCE) {
        this->currentFaceStep++;
        this->faceAnimFinished = this->currentFaceStep >= entry.maxFrames - 1;
    } else if(this->currentFaceMode == FACE_ANIM_LOOP) {
        this->currentFaceStep = (this->currentFaceStep + 1) % entry.maxFrames;
    } else if(this->currentFaceMode == FACE_ANIM_BOOMERANG) {
        if(this->faceAnimationDirectionReverse) {
            this->currentFaceStep--;
            if(this->currentFaceStep < 0) {
                this->currentFaceStep               = 1;
                this->faceAnimationDirectionReverse = false;
            }
        } else {
            this->currentFaceStep++;
            if(this->currentFaceStep >= entry.maxFrames) {
                this->currentFaceStep               = entry.maxFrames - 2;
                this->faceAnimationDirectionReverse = true;
            }
        }
    }

    if(this->faceAnimFinished) {
        this->enabled  = false;
        this->canSleep = true;
        this->animationFinishedCallback();
        return;
    }

    assert(this->currentFaceStep >= 0);
    assert(this->currentFaceStep < entry.maxFrames);

    this->enabled  = true;
    this->canSleep = false;
}

void DisplaySSD1306Module::displayFace(FaceID faceId, FaceAnimMode mode) {
    if(faceId >= FACE_ID_MAX) {
        faceId = FACE_ID_rest;
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

    if(entry.maxFrames <= 0) {
        ESP_LOGW(this->ThreadName.c_str(), "Face %s has no frames, cannot display.", entry.name);
        return;
    } else if(entry.maxFrames == 1) {
        // If there's only one frame, treat it as a static image and don't loop
        this->currentFaceMode = FACE_ANIM_ONCE;
    } else if(entry.maxFrames == 2 && mode == FACE_ANIM_BOOMERANG) {
        // there's only 2 frames, boomerang mode code not work
        // fallback to loop mode which is effectively the same as boomerang in this case
        this->currentFaceMode = FACE_ANIM_LOOP;
    }

    ESP_LOGI(this->ThreadName.c_str(), "Displaying face: %s(%d) mode: %d interval: %lu ms", entry.name, faceId, mode, interval);
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
