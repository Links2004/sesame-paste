#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ThreadController.h>
#include <forward_list>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "sesame_paste.h"

#ifdef IS_USB_SERIAL
#include <HWCDC.h>
#endif

#ifdef ENABLE_I2C
#include <Wire.h>
#endif

#define MODULE_INSTANCES
#include "modules_list.h"

static ThreadController tControl            = ThreadController();
static SemaphoreHandle_t loopDelaySemaphore = nullptr;

static volatile bool loopRunNowFlag = false;

void setup() {
    loopDelaySemaphore = xSemaphoreCreateBinary();

    Serial.begin(115200);
#ifdef IS_USB_SERIAL
    // reduce USB CDC TX timeout to prevent blocking when Serial is not connected
    Serial.setTxTimeoutMs(0);
    Serial.setDebugOutput(HWCDC::isPlugged() && HWCDC::isConnected());
#else
    Serial.setDebugOutput(true);
#endif

#ifdef WAIT_FOR_SERIAL_STARTUP
    unsigned long start = millis();
    while(!Serial && (millis() - start < WAIT_FOR_SERIAL_STARTUP)) {
        delay(10);    // wait for serial port to connect. Needed for native USB
    }
#endif

#ifdef DELAY_STARTUP
    delay(DELAY_STARTUP);
#endif

    log_i("Hello, Sesame!");

#ifdef ENABLE_I2C
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(I2C_CLOCK_SPEED);
#endif

    log_d("Setting up Modules...");
    registerModules(&tControl);
    debugPrintModules();
    log_i("Setup complete, entering main loop...");
}

void loop() {
    long delayTime = tControl.runOrDelay();
    log_v("Main loop delay: %ld ms loopRunNowFlag: %d", delayTime, loopRunNowFlag);
    if(delayTime <= 0 || loopRunNowFlag) {
        loopRunNowFlag = false;
        return;
    }

    if(delayTime == __LONG_MAX__) {
        xSemaphoreTake(loopDelaySemaphore, UINT32_MAX);
        return;
    }
    if(delayTime > 10000) {
        // TODO: deep sleep
    }

    xSemaphoreTake(loopDelaySemaphore, pdMS_TO_TICKS(delayTime));
}

// Call from task context to wake the main loop early
void wakeMainLoop() {
    loopRunNowFlag = true;
    if(loopDelaySemaphore == nullptr) {
        return;
    }
    xSemaphoreGive(loopDelaySemaphore);
}

// Call from an ISR to wake the main loop early
void IRAM_ATTR wakeMainLoopFromISR() {
    loopRunNowFlag = true;
    if(loopDelaySemaphore == nullptr) {
        return;
    }

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(loopDelaySemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
