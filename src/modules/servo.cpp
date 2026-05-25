#include "modules/servo.h"

#include "modules_list.h"

static const uint8_t servoPin[]  = { SERVO_PINS };
static const uint8_t SERVO_COUNT = (sizeof(servoPin) / sizeof(servoPin[0]));

static const String ServoNames[] = {
#define X(name) #name,
    SERVO_LIST
#undef X
};

#include "poses_list.h"

void ServoModule::setup() {
    assert(SERVO_COUNT == SERVO_MAX);

#ifdef SERVO_POWER_EN_PIN
    pinMode(SERVO_POWER_EN_PIN, OUTPUT);
    digitalWrite(SERVO_POWER_EN_PIN, !SERVO_POWER_EN_ACTIVE);
#endif

    // Setup Timers
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    for(uint8_t i = 0; i < SERVO_COUNT; i++) {
        Servo * servo = &this->servos[i];
        servo->setPeriodHertz(50);
        servo->attach(servoPin[i], SERVO_PULSE_MIN, SERVO_PULSE_MAX);
    }

    this->enabled = false;
}

void ServoModule::loop() {
    for(uint8_t i = 0; i < SERVO_COUNT; i++) {
        if(anglesNeeds[i] == SERVO_NO_CHANGE || anglesNeeds[i] == anglesCurrent[i]) {
            continue;
        }

        Servo * servo = &this->servos[i];

#ifdef SERVO_POWER_EN_PIN
        digitalWrite(SERVO_POWER_EN_PIN, SERVO_POWER_EN_ACTIVE);
#endif

        if(!servo->attached()) {
            log_d("attaching servo %d to pin %d", i, servoPin[i]);
            servo->attach(servoPin[i], SERVO_PULSE_MIN, SERVO_PULSE_MAX);
        }
        log_e("writing angle %d to servo %d (%s)", anglesNeeds[i], i, ServoNames[i].c_str());
        servo->write(anglesNeeds[i]);
        this->anglesCurrent[i] = anglesNeeds[i];
        this->runIn(this->writeDelay);
        return;
    }

    // if no changes, detach servos to:
    // - prevent jitter
    // - save power
    // - motor burnout
    if(this->stopServosNextLoop) {
        this->stopServosNextLoop = false;
        this->stopServos();
        this->enabled  = false;
        this->canSleep = true;
        return;
    }

    this->stopServosNextLoop = true;
    this->runIn(10000);
}

void ServoModule::setServoAngle(ServoName channel, int angle) {
    if(angle == SERVO_NO_CHANGE || channel >= SERVO_MAX) {
        this->anglesNeeds[channel] = SERVO_NO_CHANGE;
        return;
    }
    anglesNeeds[channel]     = constrain(angle + this->servoSubtrim[channel], 0, 180);
    this->stopServosNextLoop = false;
    this->runNow();
}

void ServoModule::stopServos() {
    log_d("detaching servos");
    for(uint8_t i = 0; i < SERVO_COUNT; i++) {
        this->servos[i].detach();
    }
#ifdef SERVO_POWER_EN_PIN
    log_d("poweroff servos");
    digitalWrite(SERVO_POWER_EN_PIN, !SERVO_POWER_EN_ACTIVE);
#endif
}
