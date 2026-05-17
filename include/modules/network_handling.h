#pragma once
#include "sesame_paste.h"

#if ENABLE_NETWORK_MODE
#if !defined(NETWORK_SSID) || !defined(NETWORK_PASS)
#error "NETWORK_SSID and NETWORK_PASS must be defined in user_config.h when ENABLE_NETWORK_MODE is true"
#endif

#include <WiFi.h>
#ifdef ENABLE_OTA
#include <ArduinoOTA.h>
#endif

enum NetworkEvent : uint16_t {
    NETWORK_EVENT_DISCONNECTED,
    NETWORK_EVENT_GOT_IP,
    NETWORK_EVENT_OTA_START,
    NETWORK_EVENT_OTA_PROGRESS,
    NETWORK_EVENT_OTA_END,
    NETWORK_EVENT_OTA_ERROR
};

class NetworkModule : public Module {
  public:
    NetworkModule() : Module() {
        ThreadName = "Network";
    };

  protected:
    String deviceHostname = "sesame-robot";
    void setup() override;
    void loop() override;
#ifdef ENABLE_OTA
    uint8_t lastOTAProgress = 0;
#endif
};

#endif
