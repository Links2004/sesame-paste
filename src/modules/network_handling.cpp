#include "modules/network_handling.h"

#include "WiFiMulti.h"

#include <esp_wifi.h>

#if ENABLE_NETWORK_MODE

void NetworkModule::setup() {
    log_i("Attempting to connect to network: %s", NETWORK_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.setMinSecurity(WIFI_AUTH_WPA3_PSK);
    WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL);
    WiFi.setHostname(deviceHostname.c_str());

    // Enable Advanced Wi-Fi features for better performance and reliability
    wifi_config_t current_config;
    esp_err_t err = esp_wifi_get_config(WIFI_IF_STA, &current_config);
    if(err == ESP_OK) {
        current_config.sta.bssid_set   = false;
        current_config.sta.rm_enabled  = 1;    // Enable RRM (802.11k Radio Resource Management)
        current_config.sta.btm_enabled = 1;    // Enable BTM (802.11v BSS Transition Management)
        current_config.sta.mbo_enabled = 1;    // Enable MBO (802.11v Multi-AP Operation)
        current_config.sta.ft_enabled  = 1;    // Enable FT (802.11r Fast Transition)
        esp_wifi_set_config(WIFI_IF_STA, &current_config);
        log_i("RRM (802.11k) and BTM (802.11v) successfully enabled!");
    } else {
        log_e("Failed to retrieve native Wi-Fi config.");
    }

    // Enable all Wi-Fi protocols (n/ac/ax)
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11A | WIFI_PROTOCOL_11AC | WIFI_PROTOCOL_11AX);

    WiFi.setSleep(WIFI_PS_MIN_MODEM);
    WiFi.begin(NETWORK_SSID, NETWORK_PASS);
    WiFi.setAutoReconnect(true);
    WiFi.setTxPower(WIFI_POWER_11dBm);

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
                log_w("WiFi event: %s", WiFi.eventName(event));
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
        log_i("OTA End");
        this->emitEvent(NETWORK_EVENT_OTA_END, nullptr);
    });
    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        uint8_t percentage = ((int)((progress * 100) / total));
        // only update on change
        if(percentage != this->lastOTAProgress) {
            this->lastOTAProgress = percentage;
            log_d("Progress: %u%%", percentage);
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
    if(WiFi.status() != WL_CONNECTED) {
        this->enabled  = false;
        this->canSleep = true;
        return;
    }

    // keep checking when connected to WiFi
    this->loopOnceFlag = false;
    this->canSleep     = false;
    this->setInterval(1000);

#ifdef ENABLE_OTA
    ArduinoOTA.handle();
#endif
}

#endif
