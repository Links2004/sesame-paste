#include "jsonrpc.h"
#include <map>
#include <cstring>

struct CStrCmp {
    bool operator()(const char * a, const char * b) const { return std::strcmp(a, b) < 0; }
};
static std::map<char *, jsonrpc_call_t, CStrCmp> methodMap;

void registerJsonRpcMethod(const char * method, jsonrpc_call_t callback) {
    methodMap[const_cast<char *>(method)] = callback;
}

void debugPrintJsonRpcMethods() {
    log_i("Registered JSON-RPC methods:");
    for(const auto & [key, callback] : methodMap) {
        log_i(" - %s", key);
    }
}

uint16_t handleJsonRpcRequest(const JsonVariant & request, JsonObject & response) {
    response["jsonrpc"] = "2.0";
    if(!request["id"].isNull()) {
        response["id"] = request["id"];
    }
    if(!request["method"].is<const char *>()) {
        response["error"] = "Invalid request: missing method";
        response["code"]  = -32600;
        return 400;
    }

    const char * method = request["method"];
    for(const auto & [key, callback] : methodMap) {
        if(strcmp(key, method) == 0) {
            return callback(request["params"], response);
        }
    }

    response["error"] = "Method not found";
    response["code"]  = -32601;
    return 404;
}
