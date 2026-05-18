#pragma once

#include "config.h"

#ifndef MODULE_INSTANCES
#define MODULE_LOAD_PREFIX extern
#else
#define MODULE_LOAD_PREFIX
#endif

#ifdef ENABLE_BUTTON
#include "modules/button.h"
MODULE_LOAD_PREFIX ButtonModule g_buttonModule;
#endif

#ifdef ENABLE_WEBSOCKETS
#include "modules/websocket.h"
MODULE_LOAD_PREFIX WebSocketModule g_webSocketModule;
#endif

#ifdef ENABLE_SSD1306
#include "modules/display_ssd1306.h"
MODULE_LOAD_PREFIX DisplaySSD1306Module g_displaySSD1306Module;
#endif

#ifdef ENABLE_NETWORK_MODE
#include "modules/network_handling.h"
MODULE_LOAD_PREFIX NetworkModule g_networkModule;
#endif

#include "modules/faces.h"
MODULE_LOAD_PREFIX FaceModule g_faceModule;

#include "modules/cable_handling.h"
MODULE_LOAD_PREFIX CableModule g_cableModule;
