# Sesame Paste

This is a Firmware for the [Sesame Robot](https://github.com/dorianborian/sesame-robot) from Dorian.
The main goal is to allow smoother animations and mixing of control inputs.

This project is here to play around and learn about robot control and kinematics, and to have fun with the Sesame Robot.
May even explore reverse kinematics in the future.

Sub goals are easy integration in the Home Assistant to allow control of the robot from there.

## The Name

The name "Sesame Paste" is a play on words, combining "Sesame" from the Sesame Robot and "Paste" as a nod to the idea of "pasting" together different control inputs and animations for the robot. And the fact that paste can be spread smoothly.

## Notes

This repo is using code from the original firmware e.g. the face bitmaps.
All credits for the original code and assets go to original creators.

This project uses PlatformIO as the build system, and is based on the Arduino framework for ESP32.

## Features

- [x] Cooperative multitasking
- [x] Module system for different functionalities
- [x] Button input handling with debounce
- [ ] Display
    - [x] SSD1306 OLED display support
    - [ ] OTA progress display
    - [ ] Support orginal Sesame faces
- [x] OTA updates via PlatformIO
- [ ] Global state management
- [ ] event system
    - [ ] Button events
    - [ ] OTA events
    - [ ] WiFi events
- [ ] Remote logging
- [ ] Servo control
- [ ] Web UI
    - [ ] Websocket
    - [ ] Joystick control
- [ ] WiFi Improv configuration
- [ ] MQTT
    - [ ] Home Assistant integration
