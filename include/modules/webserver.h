#pragma once

#include "sesame_paste.h"

#ifdef ENABLE_WEBSERVER

#include <ESPAsyncWebServer.h>

class WebServerModule : public Module {
  public:
    WebServerModule() : Module() {
        ThreadName = "WebServer";
    };

    void registerStaticUiFiles(const char * uri, const uint8_t * data, const size_t size, const char * contentType, const char * hash);

  protected:
    void setup() override;
    void loop() override;
};
#endif
