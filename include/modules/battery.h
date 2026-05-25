#pragma once
#include "sesame_paste.h"
#include "module.h"
#include "helpers/poll2event.h"

#ifdef ENABLE_BATTERY

enum BatteryEvent : uint16_t {
    BATTERY_EVENT_LEVEL_CHANGED,
    BATTERY_EVENT_LOW,
    BATTERY_EVENT_CRITICAL,
};

class BatteryModule : public Module {
  public:
    BatteryModule() : Module() {
        ThreadName = "Battery";
    };
    void interruptHandler();

    float getBatteryVoltage() {
        return this->batteryVoltage;
    }

    uint8_t getBatteryPercentage() {
        return this->batteryPercentage.get();
    }

    bool isBatteryCritical() {
        return this->batteryPercentage.get() <= BATTERY_CRITICAL_THRESHOLD;
    }

    bool isBatteryLow() {
        return this->batteryPercentage.get() <= BATTERY_LOW_THRESHOLD;
    }

  protected:
    void setup() override;
    void loop() override;

    float batteryVoltage = 0.0f;
    Poll2Event<uint8_t> batteryPercentage;
    float readBatteryVoltage();
    uint8_t calcBatteryPercentage(float voltage);
};

#endif
