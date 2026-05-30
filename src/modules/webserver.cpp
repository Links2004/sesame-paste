#include "modules/webserver.h"

#ifdef ENABLE_WEBSERVER

#include "webui.h"
#include "modules_list.h"
#include "jsonrpc.h"

static AsyncWebServer server(80);
static AsyncWebSocketMessageHandler wsHandler;
static AsyncWebSocket ws("/ws", wsHandler.eventHandler());

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

    wsHandler.onConnect([this](AsyncWebSocket * server, AsyncWebSocketClient * client) {
        log_i("Client %" PRIu32 " connected\n", client->id());
        this->runIn(10000);
    });

    wsHandler.onDisconnect([](AsyncWebSocket * server, uint32_t clientId) {
        log_i("Client %" PRIu32 " disconnected\n", clientId);
    });

    wsHandler.onError([](AsyncWebSocket * server, AsyncWebSocketClient * client, uint16_t errorCode, const char * reason, size_t len) {
        log_e("Client %" PRIu32 " error: %" PRIu16 ": %s\n", client->id(), errorCode, reason);
    });

    wsHandler.onMessage([](AsyncWebSocket * server, AsyncWebSocketClient * client, const uint8_t * data, size_t len) {
        JsonDocument json;
        DeserializationError error = deserializeJson(json, data);
        if(error) {
            log_e("Failed to parse JSON: %s\n", error.c_str());
            log_e("data(%d): %.*s\n", len, (int)len, data);
            json.clear();
            return;
        }
        // create response
        JsonDocument response;
        JsonObject root = response.add<JsonObject>();
        uint16_t status = handleJsonRpcRequest(json, root);

        // client didn't provide an id, so we can't send a response
        if(root["id"].isNull()) {
            response.clear();
            return;
        }

        // send response
        size_t respSize    = measureJson(root);
        uint8_t * respData = new uint8_t[respSize];
        assert(respData != nullptr);

        serializeJson(root, respData, respSize);
        client->text((const char *)respData, respSize);
        response.clear();
        delete[] respData;
    });

    // start server when network is ready
    // starting server with out network will result in a crash
    g_networkModule.registerEventCallback([this](uint16_t event, void * data) {
        if(event == NETWORK_EVENT_GOT_IP) {
            server.begin();
        }
    });

    registerJsonRpcEventSender([this](const JsonDocument & json) {
        if(ws.count() <= 0) {
            return;
        }

        size_t respSize    = measureJson(json);
        uint8_t * respData = new uint8_t[respSize];
        assert(respData != nullptr);

        serializeJson(json, respData, respSize);
        ws.textAll(respData, respSize);
        delete[] respData;
    });
}

void WebServerModule::loop() {
    if(ws.count() == 0) {
        this->enabled  = false;
        this->canSleep = true;
    }
    ws.pingAll();
    this->runIn(10000);
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
