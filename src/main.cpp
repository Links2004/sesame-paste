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

#define POSE_INSTANCES
#include "poses_list.h"

static ThreadController tControl            = ThreadController();
static SemaphoreHandle_t loopDelaySemaphore = nullptr;

static volatile bool loopRunNowFlag = false;

const char * resetReasonName(esp_reset_reason_t r) {
    switch(r) {
        case ESP_RST_UNKNOWN:
            return "Unknown";
        case ESP_RST_POWERON:
            return "PowerOn";    // Power on or RST pin toggled
        case ESP_RST_EXT:
            return "ExtPin";    // External pin - not applicable for ESP32
        case ESP_RST_SW:
            return "Reboot";    // esp_restart()
        case ESP_RST_PANIC:
            return "Crash";    // Exception/panic
        case ESP_RST_INT_WDT:
            return "WDT_Int";    // Interrupt watchdog (software or hardware)
        case ESP_RST_TASK_WDT:
            return "WDT_Task";    // Task watchdog
        case ESP_RST_WDT:
            return "WDT_Other";    // Other watchdog
        case ESP_RST_DEEPSLEEP:
            return "Sleep";    // Reset after exiting deep sleep mode
        case ESP_RST_BROWNOUT:
            return "BrownOut";    // Brownout reset (software or hardware)
        case ESP_RST_SDIO:
            return "SDIO";    // Reset over SDIO
        case ESP_RST_USB:
            return "USB";    // Reset by USB peripheral
        case ESP_RST_JTAG:
            return "JTAG";    // Reset by JTAG
        case ESP_RST_EFUSE:
            return "eFuse";    // Reset due to eFuse error
        case ESP_RST_PWR_GLITCH:
            return "PwrGlitch";    // Reset due to power glitch detected
        case ESP_RST_CPU_LOCKUP:
            return "CPULockup";    // Reset due to CPU lock up (double exception
        default:
            return "";
    }
}

#if defined(IS_USB_SERIAL) && !defined(WAIT_FOR_SERIAL_STARTUP)
__attribute__((constructor)) void disable_early_cdc_blocking() {
    // disable USB CDC TX blocking on boot
    // this needs to be done as early as possible to prevent blocking
    // during startup before setup() is called
    Serial.setTxTimeoutMs(0);
    Serial.setTxBufferSize(2048);
}
#endif

void setup() {
    loopDelaySemaphore = xSemaphoreCreateBinary();

    Serial.begin(115200);
#if defined(IS_USB_SERIAL) && !defined(WAIT_FOR_SERIAL_STARTUP)
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

    esp_reset_reason_t resetReason = esp_reset_reason();
    log_i("Reset reason: %s (%d)", resetReasonName(resetReason), resetReason);

#ifdef ENABLE_I2C
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(I2C_CLOCK_SPEED);
#endif

    log_d("Setting up Modules...");
    Module::registerModules(&tControl);
    Module::debugPrintModules();
    log_i("Setup complete, entering main loop...");

    debugPrintPoses();
    debugPrintJsonRpcMethods();
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
void IRAM_ATTR wakeMainLoop() {
    loopRunNowFlag = true;
    if(loopDelaySemaphore == nullptr) {
        return;
    }
    if(unlikely(xPortInIsrContext())) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(loopDelaySemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        return;
    }
    xSemaphoreGive(loopDelaySemaphore);
}
