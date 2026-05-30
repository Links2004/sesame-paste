#include "modules/webserver.h"

#ifdef ENABLE_WEBSERVER

#include <ESPAsyncWebServer.h>

#include "webui.h"
#include "modules_list.h"
#include "jsonrpc.h"

static AsyncWebServer server(80);
static AsyncWebSocketMessageHandler wsHandler;
static AsyncWebSocket ws("/ws", wsHandler.eventHandler());

void WebServerModule::setup() {
    registerWebUiFiles(*this);
    server.addHandler(&ws);

    server.on("/img/faces/*.raw", HTTP_GET, [](AsyncWebServerRequest * request) {
        String name      = request->url().substring(11, request->url().length() - 4);
        FaceEntry * face = g_faceModule.getFaceByName(name.c_str());
        if(face == nullptr || face->frames[0] == nullptr) {
            request->send(404, "text/plain", "Face not found");
            return;
        }

        AsyncWebServerResponse * response = request->beginChunkedResponse(
            "application/octet-stream",
            [face](uint8_t * buffer, size_t maxLen, size_t index) -> size_t {
                size_t frameCounter = index / FACE_BYTES;
                size_t offset       = index - (FACE_BYTES * frameCounter);
                bitmap_t * bitmap   = face->frames[frameCounter];
                if(frameCounter >= face->maxFrames || bitmap == nullptr) {
                    return 0;
                }
                size_t left = FACE_BYTES - offset;
                memcpy(buffer, bitmap + offset, left);
                frameCounter++;
                return left;
            });

        // AsyncWebServerResponse * response = request->beginResponse(200, "application/octet-stream", bitmap, FACE_BYTES);
        response->addHeader("X-Face-Name", face->name);
        response->addHeader("X-Face-Frames", String(face->maxFrames));
        response->addHeader("X-Face-FPS", String(face->fps));
        response->addHeader("X-Face-Width", String(FACE_SIZE_WIDTH));
        response->addHeader("X-Face-Height", String(FACE_SIZE_HEIGHT));
        request->send(response);
    });

    server.on("/api/jsonrpc", HTTP_POST, [](AsyncWebServerRequest * request, JsonVariant & json) {
        AsyncJsonResponse * response = new AsyncJsonResponse();
        JsonObject root              = response->getRoot().to<JsonObject>();
        uint16_t status              = handleJsonRpcRequest(json, root);
        response->setCode(status);
        response->setLength();
        response->setContentType("application/json-rpc");
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
