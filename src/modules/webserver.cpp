#include "modules/webserver.h"

#ifdef ENABLE_WEBSERVER

#include "webui.h"
#include "modules_list.h"
#include "jsonrpc.h"

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

void WebServerModule::setup() {
    registerWebUiFiles(*this);
    server.addHandler(&ws);

    server.on("/api/jsonrpc", HTTP_POST, [](AsyncWebServerRequest * request, JsonVariant & json) {
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

void WebServerModule::registerStaticUiFiles(const char * uri, const uint8_t * data, const size_t size, const char * contentType, const char * hash) {
    server.on(uri, HTTP_GET, [data, size, contentType, hash](AsyncWebServerRequest * request) {
        if(request->header("If-None-Match") == hash) {
            request->send(304);
            return;
        }
        if(request->header("Accept-Encoding").indexOf("gzip") == -1) {
            request->send(400, "text/plain", "Client must support gzip encoding");
            return;
        }
        AsyncWebServerResponse * response = request->beginResponse(200, contentType, data, size);
        response->addHeader("Etag", hash);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });
}

#endif    // ENABLE_WEBSERVER
