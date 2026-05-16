#include "modules/button.h"
#include <functional>

#ifdef ENABLE_BUTTON

static const uint8_t buttonPin[] = { BUTTON_PINS };
#define BUTTON_COUNT (sizeof(buttonPin) / sizeof(buttonPin[0]))

static bool lastButtonState[BUTTON_COUNT]              = { false };
static unsigned long lastButtonPressTime[BUTTON_COUNT] = { 0 };

namespace {
    static ButtonModule * g_buttonModuleInstance = nullptr;

    void buttonInterruptTrampoline() {
        if(g_buttonModuleInstance) {
            g_buttonModuleInstance->interruptHandler();
        }
    }
}    // namespace

void ButtonModule::setup() {
    // allow sleeping when waiting for button press
    this->canSleep = true;
    this->enabled  = false;

    g_buttonModuleInstance = this;

    for(uint8_t i = 0; i < BUTTON_COUNT; i++) {
        pinMode(buttonPin[i], INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(buttonPin[i]), buttonInterruptTrampoline, CHANGE);
    }
}

void IRAM_ATTR ButtonModule::interruptHandler() {
    this->enabled = true;
    this->setInterval(BUTTON_DEBOUNCE_DELAY);    // debounce delay, adjust as needed
    this->wakeLoopISR();                         // wake the loop to process button state
}

void ButtonModule::loop() {
    bool anyButtonPressed = false;
    for(uint8_t i = 0; i < BUTTON_COUNT; i++) {
        bool pressed = digitalRead(buttonPin[i]) == LOW;
        anyButtonPressed |= pressed;
        if(pressed && !lastButtonState[i]) {
            lastButtonPressTime[i] = millis();
            ESP_LOGI(this->ThreadName, "Button %d (%d) pressed!", buttonPin[i], i);
        } else if(!pressed && lastButtonState[i]) {
            unsigned long pressDuration = millis() - lastButtonPressTime[i];
            ESP_LOGI(this->ThreadName, "Button %d (%d) released after %lu ms!", buttonPin[i], i, pressDuration);
        }
        lastButtonState[i] = pressed;
    }

    this->canSleep = !anyButtonPressed;    // stay awake when button is pressed
    this->enabled  = false;
}

#endif
