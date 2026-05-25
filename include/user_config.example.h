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
