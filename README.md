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

- [ ] OTA updates via PlatformIO
- [ ] Display
    - [ ] Support orginal Sesame faces
- [ ] Web UI
    - [ ] Websocket
    - [ ] Joystick control
- [ ] MQTT
    - [ ] Home Assistant integration
- [ ] Servo control
