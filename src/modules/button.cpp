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

void buttonInterruptTrampoline() {
    g_buttonModule.interruptHandler();
}

void ButtonModule::setup() {
    // allow sleeping when waiting for button press
    this->canSleep = true;
    this->enabled  = false;

    assert(BUTTON_LEVEL_COUNT == BUTTON_COUNT);
    for(uint8_t i = 0; i < BUTTON_COUNT; i++) {
        pinMode(buttonPin[i], INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(buttonPin[i]), buttonInterruptTrampoline, CHANGE);
        log_i("Button %d initialized on pin %d", i, buttonPin[i]);
    }

    this->registerEventCallback([this](uint16_t event, void * data) {
        ButtonEventData_t * eventData = (ButtonEventData_t *)data;
        JsonDocument json;
        JsonObject root  = json.to<JsonObject>();
        root["button"]   = eventData->buttonIndex;
        root["pressed"]  = eventData->pressed;
        root["duration"] = eventData->pressDuration;
        sendJsonRpcEvent("button", root);
    });
}

void IRAM_ATTR ButtonModule::interruptHandler() {
    this->enabled = true;
    this->runOnceIn(BUTTON_DEBOUNCE_DELAY);    // wake the loop to process button state
}

void ButtonModule::loop() {
    bool anyButtonPressed = false;
    ButtonEventData_t eventData;
    for(uint8_t i = 0; i < BUTTON_COUNT; i++) {
        bool pressed = digitalRead(buttonPin[i]) == buttonLevel[i];

        eventData.buttonIndex = i;
        eventData.pressed     = pressed;

        anyButtonPressed |= pressed;
        if(pressed && !lastButtonState[i]) {
            lastButtonPressTime[i] = millis();
            log_d("Button %d (Pin %d) pressed!", i, buttonPin[i]);
            this->emitEvent(BUTTON_EVENT_PRESSED, &eventData);
            g_faceModule.displayFace(FACE_ID_love, FACE_ANIM_LOOP);
        } else if(!pressed && lastButtonState[i]) {
            unsigned long pressDuration = millis() - lastButtonPressTime[i];
            log_d("Button %d (Pin %d) released after %lu ms!", i, buttonPin[i], pressDuration);

            eventData.pressDuration = pressDuration;
            this->emitEvent(BUTTON_EVENT_RELEASED, &eventData);

            g_faceModule.displayFace(FACE_ID_happy, FACE_ANIM_LOOP);
        }
        lastButtonState[i] = pressed;
    }

    this->canSleep = !anyButtonPressed;    // stay awake when button is pressed
    this->enabled  = false;
}

#endif
