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

class NetworkModule : public Module {
  public:
    NetworkModule() : Module() {
        ThreadName = "Network";
    };

  protected:
    String deviceHostname = "sesame-robot";
    void setup() override;
    void loop() override;
};

#endif
