#include "modules/battery.h"

#ifdef ENABLE_BATTERY

void BatteryModule::setup() {
    analogReadResolution(12);
    pinMode(BATTERY_ADC_PIN, INPUT);
    analogReadMilliVolts(BATTERY_ADC_PIN);
    batteryPercentage.registerEventCallback([this](uint8_t percentage) {
        if(percentage <= BATTERY_CRITICAL_THRESHOLD) {
            log_w("Battery critical: %d%%", percentage);
            this->emitEvent(BATTERY_EVENT_CRITICAL, &percentage);
        } else if(percentage <= BATTERY_LOW_THRESHOLD) {
            log_w("Battery low: %d%%", percentage);
            this->emitEvent(BATTERY_EVENT_LOW, &percentage);
        } else {
            log_v("Battery updated: %d%%", percentage);
            this->emitEvent(BATTERY_EVENT_LEVEL_CHANGED, &percentage);
        }
    });

    this->runNow();
}

void BatteryModule::loop() {
    this->batteryVoltage = this->readBatteryVoltage();
    uint8_t percentage   = this->calcBatteryPercentage(this->batteryVoltage);
    this->batteryPercentage.set(percentage);
    log_d("Battery: %.3fV, %ld%%", this->batteryVoltage, percentage);

    // update every 10 seconds
    this->runIn(10000);
}

float BatteryModule::readBatteryVoltage() {
    uint32_t raw  = analogReadMilliVolts(BATTERY_ADC_PIN);
    float voltage = (raw * BATTERY_VOLTAGE_DIVIDER_RATIO) / 1000.0f;
    return voltage;
}

uint8_t BatteryModule::calcBatteryPercentage(float voltage) {
    // assume balanced cells
    voltage /= BATTERY_LIPO_CELLS;

    if(voltage >= 4.15) return 100;
    if(voltage <= 3.40) return 0;

    int v = (int)(voltage * 1000.0);
    if(v >= 4050) return map(v, 4050, 4150, 90, 100);
    if(v >= 3960) return map(v, 3960, 4050, 80, 90);
    if(v >= 3900) return map(v, 3900, 3960, 70, 80);
    if(v >= 3840) return map(v, 3840, 3900, 60, 70);
    if(v >= 3800) return map(v, 3800, 3840, 50, 60);
    if(v >= 3760) return map(v, 3760, 3800, 40, 50);
    if(v >= 3730) return map(v, 3730, 3760, 30, 40);
    if(v >= 3680) return map(v, 3680, 3730, 20, 30);
    if(v >= 3600) return map(v, 3600, 3680, 10, 20);
    return map(v, 3400, 3600, 0, 10);
}

#endif
