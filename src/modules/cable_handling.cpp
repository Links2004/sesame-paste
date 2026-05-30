#include "modules/cable_handling.h"
#include "modules_list.h"
#include <HWCDC.h>

#ifdef IS_USB_SERIAL
void hwcdc_handler_helper(void * arg, esp_event_base_t event_base, int32_t event_id, void * event_data) {
    g_cableModule.hwcdc_handler(arg, event_base, event_id, event_data);
}

void CableModule::hwcdc_handler(void * arg, esp_event_base_t event_base, int32_t event_id, void * event_data) {
    arduino_hw_cdc_event_data_t * data = (arduino_hw_cdc_event_data_t *)event_data;
    switch(event_id) {
        case ARDUINO_HW_CDC_BUS_RESET_EVENT:
        case ARDUINO_HW_CDC_CONNECTED_EVENT:
            this->runOnce();
            break;
        case ARDUINO_HW_CDC_RX_EVENT:
            if(!stateUSBconnected.get() || !stateUSBserialConnected.get()) {
                log_i("USB event (%d)", data->rx.len);
                this->runOnce();
            }
            break;
        default:
            break;
    }
}
#endif

#ifdef CABLE_CONNECTED_PIN
void IRAM_ATTR cable_connected_helper() {
    g_cableModule.cable_connected_interrupt_handler();
}
#endif

void IRAM_ATTR CableModule::cable_connected_interrupt_handler() {
    this->runOnce();
}

void CableModule::setup() {
#ifdef IS_USB_SERIAL
    Serial.onEvent(hwcdc_handler_helper);
    log_d("Registered HWCDC event handler");
#endif

#ifdef CABLE_CONNECTED_PIN
    pinMode(CABLE_CONNECTED_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(CABLE_CONNECTED_PIN), cable_connected_helper, CHANGE);
    log_d("Initialized cable connected pin: %d", CABLE_CONNECTED_PIN);
#endif

    stateUSBconnected.registerEventCallback([this](bool connected) {
        if(connected) {
            log_i("USB Data Connected");
            this->emitEvent(CABLE_EVENT_USB_DATA_CONNECTED, nullptr);
            return;
        }
        log_i("USB Data Disconnected");
        this->emitEvent(CABLE_EVENT_USB_DATA_DISCONNECTED, nullptr);
    });

    stateUSBserialConnected.registerEventCallback([this](bool connected) {
        if(connected) {
            Serial.setDebugOutput(true);
            log_i("USB Serial Connected");
            this->emitEvent(CABLE_EVENT_USB_SERIAL_CONNECTED, nullptr);
            return;
        }
        log_i("USB Serial Disconnected");
        this->emitEvent(CABLE_EVENT_USB_SERIAL_DISCONNECTED, nullptr);
        Serial.setDebugOutput(false);
    });

#ifdef SERIAL_JSON_RPC
    registerJsonRpcEventSender([this](const JsonDocument & doc) {
        if(!stateUSBserialConnected.get()) {
            return;
        }
        serializeJson(doc, Serial);
        Serial.println();
    });
#endif

    this->runOnce();
}

void CableModule::loop() {
    // Check USB Data and Serial connection status
    bool power = false;
#ifdef IS_USB_SERIAL
    power |= HWCDC::isPlugged();
#endif
#ifdef CABLE_CONNECTED_PIN
    power |= digitalRead(CABLE_CONNECTED_PIN) == CABLE_CONNECTED_ACTIVE;
#endif
    this->stateUSBconnected.set(power);

    bool serial = false;
#ifdef IS_USB_SERIAL
    serial |= HWCDC::isConnected();
#endif
    this->stateUSBserialConnected.set(serial);

    this->canSleep = true;

    // there are no events from HWCDC when USB is unplugged,
    // so we need to resort to polling to detect USB disconnection.
    this->enabled = power || serial;
    if(this->enabled) {
        this->setInterval(1000);    // check every second when connected
        this->loopOnceFlag = false;
        this->enabled      = true;
    }
}
