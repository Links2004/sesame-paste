#pragma once

#include "sesame_paste.h"

#ifdef ENABLE_WEBSERVER

#include <ESPAsyncWebServer.h>

class WebServerModule : public Module {
  public:
    WebServerModule() : Module() {
        ThreadName = "WebServer";
    };

  protected:
    void setup() override;
    void loop() override;
};
#endif
