#pragma once

#include "config.h"

#ifdef ENABLE_BUTTON
#include "modules/button.h"
static ButtonModule g_buttonModule;
#endif

#ifdef ENABLE_WEBSOCKETS
#include "modules/websocket.h"
static WebSocketModule g_webSocketModule;
#endif

#ifdef ENABLE_SSD1306
#include "modules/display_ssd1306.h"
static DisplaySSD1306Module g_displaySSD1306Module;
#endif

#ifdef ENABLE_NETWORK_MODE
#include "modules/network_handling.h"
static NetworkModule g_networkModule;
#endif
