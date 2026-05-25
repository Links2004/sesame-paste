#pragma once
#include "sesame_paste.h"
#include "module.h"
#include "helpers/poll2event.h"

enum CableEvent : uint16_t {
    CABLE_EVENT_USB_DATA_CONNECTED,
    CABLE_EVENT_USB_DATA_DISCONNECTED,
    CABLE_EVENT_USB_SERIAL_CONNECTED,
    CABLE_EVENT_USB_SERIAL_DISCONNECTED
};

class CableModule : public Module {
  public:
    CableModule() : Module() {
        ThreadName = "Cable";
    };

    inline bool isUSBConnected() {
        return this->stateUSBconnected.get();
    }

    inline bool isUSBSerialConnected() {
        return this->stateUSBserialConnected.get();
    }

#ifdef IS_USB_SERIAL
    void hwcdc_handler(void * arg, esp_event_base_t event_base, int32_t event_id, void * event_data);
#endif

#ifdef CABLE_CONNECTED_PIN
    void IRAM_ATTR cable_connected_interrupt_handler();
#endif

  protected:
    void setup() override;
    void loop() override;

    Poll2Event<bool> stateUSBconnected;
    Poll2Event<bool> stateUSBserialConnected;
};
