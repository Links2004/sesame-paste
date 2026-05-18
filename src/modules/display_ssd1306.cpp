#include "modules/display_ssd1306.h"

#ifdef ENABLE_SSD1306

#include <Wire.h>

#include "modules_list.h"

void DisplaySSD1306Module::setup() {
    uint16_t errors = 0;

    log_i("Initializing SSD1306 display...");
    this->display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SSD1306_RESET_PIN);

    // Test and init if display exists on I2C bus.
    while(1) {
        Wire.beginTransmission(SSD1306_I2C_ADDR);
        if(Wire.endTransmission() == 0) {
            if(this->display.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDR)) {
                break;
            } else {
                log_w("SSD1306 found but failed");
            }
        }
        if(errors++ > 50) {
            this->enabled = false;
            log_e("SSD1306 failed, giving up.");
            return;
        }
        log_w("SSD1306 (0x%02X) failed, %d retrying...", SSD1306_I2C_ADDR, errors);
        delay(1000);
    }

    this->display.clearDisplay();
    this->display.setTextColor(SSD1306_WHITE);
    this->display.setTextSize(2);
    this->display.setCursor(0, 0);
    this->display.println(F("Booting..."));
    this->display.display();

    log_i("Initializing SSD1306 display... done");

    g_faceModule.registerEventCallback([this](uint16_t event, void * data) {
        if(event == FACE_EVENT_NEW_BITMAP) {
            this->faceBitmap = (bitmap_t *)data;
            this->loopOnce();
            this->wakeLoop();
        } else if(event == FACE_EVENT_ANIMATION_FINISHED) {
            // You can add code here to handle when a face animation finishes if desired
        }
    });

    g_cableModule.registerEventCallback([this](uint16_t event, void * data) {
        this->loopOnce();
        this->wakeLoop();
    });

    g_networkModule.registerEventCallback([this](uint16_t event, void * data) {
        switch(event) {
#ifdef ENABLE_OTA
                // The normal loop is not running during OTA updates,
                // so we have to update the display directly from the event callback.

            case NETWORK_EVENT_OTA_START: {
                int cmd     = *((int *)data);
                String type = (cmd == U_FLASH) ? "firmware" : "filesystem";
                this->display.clearDisplay();
                this->display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_COLOR_SHIFT_HEIGHT, SSD1306_WHITE);
                this->display.setTextSize(2);
                this->display.setCursor(0, SCREEN_COLOR_SHIFT_HEIGHT + 2);
                this->display.printf("OTA\n%s", type.c_str());
                this->display.display();
                break;
            }
            case NETWORK_EVENT_OTA_PROGRESS: {
                uint8_t progress = *((uint8_t *)data);
                int16_t barWidth = (SCREEN_WIDTH - 4) * progress / 100;
                this->display.fillRect(
                    2,
                    2,
                    2 + barWidth,
                    SCREEN_COLOR_SHIFT_HEIGHT - 4,
                    SSD1306_WHITE);

                this->display.fillRect(
                    0,
                    SCREEN_COLOR_SHIFT_HEIGHT,
                    SCREEN_WIDTH,
                    SCREEN_HEIGHT - SCREEN_COLOR_SHIFT_HEIGHT,
                    SSD1306_BLACK);

                this->display.setTextSize(4);
                {
                    // Each char at textSize=4 is 6*4=24px wide, 8*4=32px tall
                    char buf[8] = { 0 };
                    snprintf(buf, sizeof(buf), "%u%%", progress);
                    int16_t textWidth = strlen(buf) * 24;
                    this->display.setCursor(
                        (SCREEN_WIDTH - textWidth) / 2,
                        SCREEN_COLOR_SHIFT_HEIGHT + ((SCREEN_HEIGHT - SCREEN_COLOR_SHIFT_HEIGHT) / 2) - 16);
                    this->display.print(buf);
                }
                this->display.display();
                break;
            }
            case NETWORK_EVENT_OTA_END:
            case NETWORK_EVENT_OTA_ERROR:
                this->display.clearDisplay();
                this->display.setTextSize(2);
                this->display.setCursor(0, SCREEN_COLOR_SHIFT_HEIGHT);
                this->display.println(F("OTA Update"));
                if(event == NETWORK_EVENT_OTA_ERROR) {
                    this->display.println(F("Error!"));
                } else {
                    this->display.println(F("Complete!"));
                }
                this->display.display();
                break;
#endif
        }
    });
}

void DisplaySSD1306Module::loop() {
    this->display.clearDisplay();

    // int16_t faceOffsetLines = SCREEN_COLOR_SHIFT_HEIGHT;
    int16_t faceOffsetLines = 0;
    int16_t height          = SCREEN_HEIGHT - faceOffsetLines;

    size_t faceOffsetBytes = (SCREEN_WIDTH / 8) * faceOffsetLines;

    if(this->faceBitmap) {
        this->display.drawBitmap(0, faceOffsetLines, &this->faceBitmap[faceOffsetBytes], SCREEN_WIDTH, height, SSD1306_WHITE);
    }

    this->display.fillRect(
        0,
        0,
        16,
        SCREEN_COLOR_SHIFT_HEIGHT,
        g_cableModule.isUSBConnected() ? SSD1306_WHITE : SSD1306_BLACK);

    this->display.fillRect(
        0 + 50,
        0,
        16,
        SCREEN_COLOR_SHIFT_HEIGHT,
        g_cableModule.isUSBSerialConnected() ? SSD1306_WHITE : SSD1306_BLACK);

    this->display.display();
}

#endif
