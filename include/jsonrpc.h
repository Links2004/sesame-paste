#pragma once

#include "sesame_paste.h"

#include <ArduinoJson.h>

typedef std::function<void(const JsonDocument & json)> jsonrpc_event_send_t;
void registerJsonRpcEventSender(jsonrpc_event_send_t sender);
void sendJsonRpcEvent(const char * event);
void sendJsonRpcEvent(const char * event, const JsonVariant & params);

typedef std::function<uint16_t(const JsonVariant & params, JsonObject & response)> jsonrpc_call_t;
void registerJsonRpcMethod(const char * method, jsonrpc_call_t callback);
void debugPrintJsonRpcMethods();

uint16_t handleJsonRpcRequest(const JsonVariant & request, JsonObject & response);
