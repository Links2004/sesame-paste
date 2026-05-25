#pragma once

#define NETWORK_SSID "your_wifi_ssid"
#define NETWORK_PASS "your_wifi_password"

// Useful to give the serial monitor time to connect before the device starts
// PlatformIO / VScode does not connect fast enough for the initial logs to be seen
// #define DELAY_STARTUP 5000

// Keep in sync with platformio.user.ota.ini
#define OTA_PASSWORD "sesameadmin"

// Servo power control pin
// #define SERVO_POWER_EN_PIN 9
// #define SERVO_POWER_EN_ACTIVE HIGH

// Cable connected pin
// #define CABLE_CONNECTED_PIN 11
// #define CABLE_CONNECTED_ACTIVE HIGH

// Battery Voltage pin
// with Voltage divider e.g. 15k/47k
// #define BATTERY_ADC_PIN 10
// #define BATTERY_VOLTAGE_DIVIDER_TOP 15000
// #define BATTERY_VOLTAGE_DIVIDER_BOTTOM 47000
// #define BATTERY_VOLTAGE_DIVIDER_RATIO ((BATTERY_VOLTAGE_DIVIDER_TOP + BATTERY_VOLTAGE_DIVIDER_BOTTOM) / (float)BATTERY_VOLTAGE_DIVIDER_BOTTOM)
// #define BATTERY_LIPO_CELLS 1
