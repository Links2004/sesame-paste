#pragma once

#include "sesame_paste.h"

#ifdef ENABLE_WEBSOCKETS

#include <WebSocketsServer.h>

class WebSocketModule : public Module {
  public:
    WebSocketModule() : Module() {};
    ~WebSocketModule();

  protected:
    WebSocketsServer webSocket = WebSocketsServer(81);
    void setup() override;
    void loop() override;
};
#endif
