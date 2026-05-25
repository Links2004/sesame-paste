#include "modules/animation.h"
#include <functional>

#include "modules_list.h"
#include "poses_list.h"

void AnimationModule::setup() {
    this->playPose("Stand", ANIMATION_LOOP);
    // this->playPose("Wave", 10);
}

bool AnimationModule::playPose(const String & name, uint16_t count) {
    log_e("Playing pose: %s, count: %d", name.c_str(), count);
    Pose_t * pose = findPoseByName(name);
    if(pose == nullptr) {
        log_e("Pose not found: %s", name.c_str());
        return false;
    }
    if(pose->count == 0) {
        log_e("Pose has no steps: %s", name.c_str());
        return false;
    }
    if(pose->count == 1 && count > 1) {
        log_w("Pose %s has only one step, switching to ANIMATION_ONCE mode", name.c_str());
        count = ANIMATION_ONCE;
    }

    log_i("Playing pose: %s, count: %d", name.c_str(), count);
    this->currentPose           = pose;
    this->currentAnimationCount = count;
    this->currentAnimationStep  = 0;
    this->animationPlaying      = true;
    this->runNow();
    return true;
}

void AnimationModule::loop() {
    if(!animationPlaying || currentPose == nullptr) {
        this->enabled  = false;
        this->canSleep = true;
        return;
    }

    assert(currentAnimationStep < currentPose->count);

    const PoseStep_t & step = this->currentPose->poses[this->currentAnimationStep];

    g_faceModule.displayFace(step.face, step.face_mode);
    for(uint8_t i = 0; i < SERVO_MAX; i++) {
        g_servoModule.setServoAngle(i, step.angle[i]);
    }
    this->currentAnimationStep++;

    log_d("Playing pose step %d/%d: duration %ld ms", this->currentAnimationStep, this->currentPose->count, step.duration);

    if(this->currentAnimationStep >= this->currentPose->count) {
        if(this->currentAnimationCount == ANIMATION_LOOP || --this->currentAnimationCount > 0) {
            this->currentAnimationStep = this->currentPose->loop_start;
        } else {
            this->animationPlaying = false;
            this->enabled          = false;
            this->canSleep         = true;
            this->emitEvent(ANIMATION_EVENT_FINISHED, nullptr);
            return;
        }
    }
    this->runOnceIn(step.duration);
}
