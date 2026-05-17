#include "modules/network_handling.h"

#if ENABLE_NETWORK_MODE

void NetworkModule::setup() {
    Serial.printf("Attempting to connect to network: %s\n", NETWORK_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(deviceHostname.c_str());
    WiFi.begin(NETWORK_SSID, NETWORK_PASS);

    WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info) {
        if(event == ARDUINO_EVENT_WIFI_STA_GOT_IP) {
            log_w("Connected to WiFi network! IP: %s", WiFi.localIP().toString().c_str());
        } else if(event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
            log_w("Disconnected from WiFi network.");
        }
        this->loopOnce();
        this->wakeLoop();
    });

#ifdef ENABLE_OTA
    ArduinoOTA.onStart([]() {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if(error == OTA_AUTH_ERROR)
            Serial.println("Auth Failed");
        else if(error == OTA_BEGIN_ERROR)
            Serial.println("Begin Failed");
        else if(error == OTA_CONNECT_ERROR)
            Serial.println("Connect Failed");
        else if(error == OTA_RECEIVE_ERROR)
            Serial.println("Receive Failed");
        else if(error == OTA_END_ERROR)
            Serial.println("End Failed");
    });

    ArduinoOTA.setHostname(deviceHostname.c_str());
    ArduinoOTA.setMdnsEnabled(false);
    ArduinoOTA.setPort(3232);

#ifdef OTA_PASSWORD
    ArduinoOTA.setPassword(OTA_PASSWORD);
#endif
    ArduinoOTA.begin();
#endif    // ENABLE_OTA
    this->loopOnce();
}

void NetworkModule::loop() {
#ifdef ENABLE_OTA
    if(WiFi.status() == WL_CONNECTED) {
        ArduinoOTA.handle();
        this->loopOnceFlag = false;    // keep looping
        this->setInterval(0);          // run every loop
    }
#endif
}

#endif
