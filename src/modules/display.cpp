#include "modules/display.h"

#ifdef ENABLE_I2C_DISPLAY

#include <Wire.h>
#include <forward_list>

#include "modules_list.h"

void DisplayModule::setup() {
    uint16_t errors = 0;

    log_i("Initializing display...");
    this->display = new DISPLAY_TYPE(U8G2_R0, /* reset=*/DISPLAY_RESET_PIN);

    this->display->setI2CAddress(DISPLAY_I2C_ADDR << 1);
    this->display->setBusClock(I2C_CLOCK_SPEED);
    this->display->setFontPosTop();

    // Test and init if display exists on I2C bus.
    while(1) {
        Wire.beginTransmission(DISPLAY_I2C_ADDR);
        if(Wire.endTransmission() == 0) {
            if(this->display->begin()) {
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
        log_w("SSD1306 (0x%02X) failed, %d retrying...", DISPLAY_I2C_ADDR, errors);
        delay(1000);
    }

    this->display->clearBuffer();
    this->display->setFont(FONT_16);
    this->display->setCursor(0, 0);
    this->display->println(F("Booting..."));
    this->display->sendBuffer();

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
            case NETWORK_EVENT_DISCONNECTED:
            case NETWORK_EVENT_GOT_IP:
                this->loopOnce();
                this->wakeLoop();
                break;
#ifdef ENABLE_OTA
                // The normal loop is not running during OTA updates,
                // so we have to update the display directly from the event callback.

            case NETWORK_EVENT_OTA_START: {
                int cmd     = *((int *)data);
                String type = (cmd == U_FLASH) ? "firmware" : "filesystem";
                this->display->clearBuffer();
                this->display->drawFrame(0, 0, DISPLAY_WIDTH, DISPLAY_COLOR_SHIFT_HEIGHT);
                this->display->setFont(FONT_16);
                this->display->setCursor(0, DISPLAY_COLOR_SHIFT_HEIGHT + 2);
                this->display->printf("OTA\n%s", type.c_str());
                this->display->sendBuffer();
                break;
            }
            case NETWORK_EVENT_OTA_PROGRESS: {
                uint8_t progress = *((uint8_t *)data);
                int16_t barWidth = (DISPLAY_WIDTH - 4) * progress / 100;

                this->display->clearBuffer();
                this->display->drawFrame(0, 0, DISPLAY_WIDTH, DISPLAY_COLOR_SHIFT_HEIGHT);
                this->display->drawBox(2, 2, 2 + barWidth, DISPLAY_COLOR_SHIFT_HEIGHT - 4);

                char buf[8] = { 0 };
                snprintf(buf, sizeof(buf), "%u%%", progress);

                this->display->setFont(FONT_48);
                this->printCenteredLowerColor(buf);

                this->display->sendBuffer();
                break;
            }
            case NETWORK_EVENT_OTA_END:
            case NETWORK_EVENT_OTA_ERROR:
                this->display->clearBuffer();
                this->display->setFont(FONT_16);
                if(event == NETWORK_EVENT_OTA_ERROR) {
                    this->printCenteredLowerColor("OTA Error");
                } else {
                    this->printCenteredLowerColor("OTA Complete");
                }
                this->display->sendBuffer();
                break;
#endif
        }
    });
}

void DisplayModule::setCursorCentered(int16_t x, int16_t y, int16_t w, int16_t h, const char * text) {
    int16_t textWidth  = this->display->getStrWidth(text);
    int16_t textHeight = this->display->getMaxCharHeight();
    this->display->setCursor(
        x + (w - textWidth) / 2,
        y + (h - textHeight) / 2);
}

void DisplayModule::loop() {
    this->display->clearBuffer();

    // int16_t faceOffsetLines = DISPLAY_COLOR_SHIFT_HEIGHT;
    int16_t faceOffsetLines = 0;
    int16_t height          = DISPLAY_HEIGHT - faceOffsetLines;

    size_t faceOffsetBytes = (DISPLAY_WIDTH / 8) * faceOffsetLines;

    if(this->faceBitmap) {
        this->display->drawBitmap(0, faceOffsetLines, DISPLAY_WIDTH / 8, height, &this->faceBitmap[faceOffsetBytes]);
    }

    this->renderIcons();
    this->display->sendBuffer();
}

void DisplayModule::renderIcons() {
    std::forward_list<uint16_t> icons;

#ifdef ENABLE_NETWORK_MODE
    if(WiFi.status() != WL_CONNECTED) {
        icons.push_front(ICON_NO_WIFI);
    } else {
        // get WiFi signal strength and determine which icon to show
        int32_t rssi = WiFi.RSSI();
        if(rssi > -60) {
            icons.push_front(ICON_WIFI_3);
        } else if(rssi > -75) {
            icons.push_front(ICON_WIFI_2);
        } else {
            icons.push_front(ICON_WIFI_1);
        }
    }
#endif

    if(g_cableModule.isUSBSerialConnected()) {
        icons.push_front(ICON_USB_SERIAL_CONNECTED);
    }

    if(g_cableModule.isUSBConnected()) {
        icons.push_front(ICON_USB_CONNECTED);
    }

    // TODO: Battery level indicator

    this->renderIcons(icons);
}

void DisplayModule::renderIcons(std::forward_list<uint16_t> icons) {
    this->display->setFont(FONT_ICONS_16);
    const uint8_t iconWidth = this->display->getMaxCharWidth();
    uint8_t x               = 0;
    for(uint16_t icon : icons) {
        this->display->drawGlyph(x, 0, icon);
        x += iconWidth;
    }
}

#endif
