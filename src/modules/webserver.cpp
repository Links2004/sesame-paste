#include "modules/webserver.h"

#ifdef ENABLE_WEBSERVER

#include "webui.h"
#include "modules_list.h"
#include "jsonrpc.h"

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

void WebServerModule::setup() {
    registerStaticUiFiles(server);
    server.addHandler(&ws);

    server.on("/api/jsonrpc", HTTP_POST, [](AsyncWebServerRequest * request, JsonVariant & json) {
        serializeJson(json, Serial);
        Serial.println();
        AsyncJsonResponse * response = new AsyncJsonResponse();
        JsonObject root              = response->getRoot().to<JsonObject>();
        uint16_t status              = handleJsonRpcRequest(json, root);
        response->setCode(status);
        response->setLength();
        request->send(response);
    });

    // start server when network is ready
    // starting server with out network will result in a crash
    g_networkModule.registerEventCallback([this](uint16_t event, void * data) {
        if(event == NETWORK_EVENT_GOT_IP) {
            server.begin();
        }
    });
}

void WebServerModule::loop() {
    this->enabled  = false;
    this->canSleep = true;
}

#endif    // ENABLE_WEBSERVER
