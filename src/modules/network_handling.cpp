#include "modules/network_handling.h"

#if ENABLE_NETWORK_MODE

void NetworkModule::setup() {
    log_i("Attempting to connect to network: %s", NETWORK_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(deviceHostname.c_str());
    WiFi.begin(NETWORK_SSID, NETWORK_PASS);

    WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info) {
        switch(event) {
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                log_w("Disconnected from WiFi network.");
                this->emitEvent(NETWORK_EVENT_DISCONNECTED, nullptr);
                break;
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                log_w("Connected to WiFi network! IP: %s", WiFi.localIP().toString().c_str());
                this->emitEvent(NETWORK_EVENT_GOT_IP, nullptr);
                break;
            default:
                break;
        }

        this->loopOnce();
        this->wakeLoop();
    });

#ifdef ENABLE_OTA
    ArduinoOTA.onStart([this]() {
        int cmd     = ArduinoOTA.getCommand();
        String type = (cmd == U_FLASH) ? "firmware" : "filesystem";
        log_i("Start updating %s", type.c_str());
        this->lastOTAProgress = 255;
        this->emitEvent(NETWORK_EVENT_OTA_START, &cmd);
    });
    ArduinoOTA.onEnd([this]() {
        log_i("\nEnd");
        this->emitEvent(NETWORK_EVENT_OTA_END, nullptr);
    });
    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        uint8_t percentage = ((int)((progress * 100) / total));
        // only update on change
        if(percentage != this->lastOTAProgress) {
            this->lastOTAProgress = percentage;
            log_d("Progress: %u%%\r", percentage);
            this->emitEvent(NETWORK_EVENT_OTA_PROGRESS, &percentage);
        }
    });
    ArduinoOTA.onError([this](ota_error_t error) {
        log_e("Error[%u]: ", error);
        if(error == OTA_AUTH_ERROR)
            log_e("Auth Failed");
        else if(error == OTA_BEGIN_ERROR)
            log_e("Begin Failed");
        else if(error == OTA_CONNECT_ERROR)
            log_e("Connect Failed");
        else if(error == OTA_RECEIVE_ERROR)
            log_e("Receive Failed");
        else if(error == OTA_END_ERROR)
            log_e("End Failed");
        this->emitEvent(NETWORK_EVENT_OTA_ERROR, &error);
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
