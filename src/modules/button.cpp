#include "modules/button.h"
#include <functional>

#ifdef ENABLE_BUTTON

#include "modules_list.h"

static const uint8_t buttonPin[]        = { BUTTON_PINS };
static const bool buttonLevel[]         = { BUTTON_LEVELS };
static const uint8_t BUTTON_COUNT       = (sizeof(buttonPin) / sizeof(buttonPin[0]));
static const uint8_t BUTTON_LEVEL_COUNT = (sizeof(buttonLevel) / sizeof(buttonLevel[0]));

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

    assert(BUTTON_LEVEL_COUNT == BUTTON_COUNT);
    g_buttonModuleInstance = this;

    for(uint8_t i = 0; i < BUTTON_COUNT; i++) {
        pinMode(buttonPin[i], INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(buttonPin[i]), buttonInterruptTrampoline, CHANGE);
        log_i("Button %d initialized on pin %d", i, buttonPin[i]);
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
        bool pressed = digitalRead(buttonPin[i]) == buttonLevel[i];
        anyButtonPressed |= pressed;
        if(pressed && !lastButtonState[i]) {
            lastButtonPressTime[i] = millis();
            log_i("Button %d (Pin %d) pressed!", i, buttonPin[i]);
            this->emitEvent(BUTTON_EVENT_PRESSED, nullptr);

            g_faceModule.displayFace(FACE_ID_love, FACE_ANIM_LOOP);
        } else if(!pressed && lastButtonState[i]) {
            unsigned long pressDuration = millis() - lastButtonPressTime[i];
            log_i("Button %d (Pin %d) released after %lu ms!", i, buttonPin[i], pressDuration);
            this->emitEvent(BUTTON_EVENT_RELEASED, &pressDuration);

            g_faceModule.displayFace(FACE_ID_happy, FACE_ANIM_LOOP);
        }
        lastButtonState[i] = pressed;
    }

    this->canSleep = !anyButtonPressed;    // stay awake when button is pressed
    this->enabled  = false;
}

#endif
