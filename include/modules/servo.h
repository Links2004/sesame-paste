#pragma once
#include "sesame_paste.h"
#include "module.h"

#include <ESP32Servo.h>

#define SERVO_LIST \
    X(R1)          \
    X(R2)          \
    X(L1)          \
    X(L2)          \
    X(R4)          \
    X(R3)          \
    X(L3)          \
    X(L4)

#define POSE_LIST \
    X(Rest)       \
    X(Stand)

enum ServoName : uint8_t {
#define X(name) name,
    SERVO_LIST
#undef X
        SERVO_MAX
};

enum PoseName : uint8_t {
#define X(name) POSE_##name,
    POSE_LIST
#undef X
        POSE_MAX
};

// Map 0-180 to approx 732-2929us
#define SERVO_PULSE_MIN 732
#define SERVO_PULSE_MAX 2929

#define SERVO_NO_CHANGE __INT_MAX__

class ServoModule : public Module {
  public:
    ServoModule() : Module() {
        ThreadName = "Servo";
        for(uint8_t i = 0; i < SERVO_MAX; i++) {
            this->anglesNeeds[i]   = SERVO_NO_CHANGE;
            this->anglesCurrent[i] = SERVO_NO_CHANGE;
        }
    };
    void interruptHandler();

    void runRestPose();
    void runStandPose(int face = 1);

    void setServoAngle(ServoName channel, int angle);
    void setServoAngle(uint8_t channel, int angle) {
        this->setServoAngle(static_cast<ServoName>(channel), angle);
    };

  protected:
    void setup() override;
    void loop() override;

    void stopServos();

    Servo servos[SERVO_MAX];
    int8_t servoSubtrim[SERVO_MAX] = { 0 };

    bool stopServosNextLoop  = false;
    unsigned long writeDelay = 20;
    int anglesNeeds[SERVO_MAX];
    int anglesCurrent[SERVO_MAX];
};
