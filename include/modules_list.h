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

#ifdef ENABLE_I2C_DISPLAY
#include "modules/display.h"
MODULE_LOAD_PREFIX DisplayModule g_DisplayModule;
#endif

#ifdef ENABLE_NETWORK_MODE
#include "modules/network_handling.h"
MODULE_LOAD_PREFIX NetworkModule g_networkModule;
#endif

#ifdef ENABLE_BATTERY
#include "modules/battery.h"
MODULE_LOAD_PREFIX BatteryModule g_batteryModule;
#endif

#include "modules/faces.h"
MODULE_LOAD_PREFIX FaceModule g_faceModule;

#include "modules/cable_handling.h"
MODULE_LOAD_PREFIX CableModule g_cableModule;

#include "modules/servo.h"
MODULE_LOAD_PREFIX ServoModule g_servoModule;

#include "modules/animation.h"
MODULE_LOAD_PREFIX AnimationModule g_animationModule;
