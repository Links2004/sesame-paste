#include "modules/websocket.h"

#ifdef ENABLE_WEBSOCKETS

WebSocketModule::~WebSocketModule() {
    this->webSocket.disconnect();
    this->webSocket.close();
}

void WebSocketModule::setup() {
    // TODO: handle cases where WiFi is not connected yet
    this->webSocket.begin();
    this->webSocket.onEvent([this](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
        if(type == WStype_TEXT) {
            Serial.printf("Received WebSocket message: %s\n", payload);
            // Echo the message back to the client
            this->webSocket.sendTXT(num, payload, length);
        }
    });
}

void WebSocketModule::loop() {
    this->webSocket.loop();

    int clients = webSocket.connectedClients(false);
    // keep running when clients are connected; otherwise back off
    this->setInterval(clients ? 0 : 200);
}

#endif    // ENABLE_WEBSOCKETS
