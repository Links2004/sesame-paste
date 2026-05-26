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

    g_animationModule.registerEventCallback([this](uint16_t event, void * data) {
        if(event == ANIMATION_EVENT_EMERGENCY_STOP) {
            log_e("Emergency stop activated! Stopping all servos immediately.");
            this->detach();
            this->runOnceIn(SERVO_SHUTDOWN_POWEROFF_TIMEOUT_MS);
        }
    });

    this->enabled = false;
}

void ServoModule::loop() {
    // if no changes, detach servos to:
    // - prevent jitter
    // - save power
    // - motor burnout
    switch(this->servosShutdownSteps) {
        default:
        case SERVO_SHUTDOWN_NONE:
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
                this->runOnceIn(this->writeDelay);
                this->servosShutdownSteps = SERVO_SHUTDOWN_NONE;
                return;
            }
            this->runOnceIn(SERVO_SHUTDOWN_TIMEOUT_MS);
            break;
        case SERVO_SHUTDOWN_DETACH:
            this->detach();
            this->runOnceIn(SERVO_SHUTDOWN_POWEROFF_TIMEOUT_MS);
            break;
#ifdef SERVO_POWER_EN_PIN
        case SERVO_SHUTDOWN_POWER_OFF:
            log_d("poweroff servos");
            digitalWrite(SERVO_POWER_EN_PIN, !SERVO_POWER_EN_ACTIVE);
#endif
        case SERVO_SHUTDOWN_DONE:
            this->enabled  = false;
            this->canSleep = true;
            return;
    }
    this->servosShutdownSteps = (servo_shutdown_steps_t)(this->servosShutdownSteps + 1);
}

void ServoModule::setServoAngle(ServoName channel, int angle) {
    if(angle == SERVO_NO_CHANGE || channel >= SERVO_MAX) {
        this->anglesNeeds[channel] = SERVO_NO_CHANGE;
        return;
    }
    anglesNeeds[channel]      = constrain(angle + this->servoSubtrim[channel], 0, 180);
    this->servosShutdownSteps = SERVO_SHUTDOWN_NONE;
    this->runNow();
}

void ServoModule::detach() {
    log_d("detaching servos");
    this->servosShutdownSteps = SERVO_SHUTDOWN_DETACH;
    for(uint8_t i = 0; i < SERVO_COUNT; i++) {
        this->anglesNeeds[i]   = SERVO_NO_CHANGE;
        this->anglesCurrent[i] = SERVO_NO_CHANGE;
        this->servos[i].detach();
        pinMode(servoPin[i], INPUT);
    }
}
