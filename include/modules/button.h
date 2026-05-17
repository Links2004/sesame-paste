#pragma once
#include "sesame_paste.h"
#include "module.h"

#ifdef ENABLE_BUTTON

#ifndef BUTTON_PINS
#error "BUTTON_PINS is not defined in config"
#endif

#ifndef BUTTON_DEBOUNCE_DELAY
#error "BUTTON_DEBOUNCE_DELAY is not defined in config"
#endif

enum ButtonEvent : uint16_t {
    BUTTON_EVENT_PRESSED,
    BUTTON_EVENT_RELEASED
};

class ButtonModule : public Module {
  public:
    ButtonModule() : Module() {
        ThreadName = "Button";
    };
    void interruptHandler();

  protected:
    void setup() override;
    void loop() override;
};

#endif
