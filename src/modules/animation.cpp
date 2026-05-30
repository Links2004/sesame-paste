#include "modules/animation.h"
#include <functional>

#include "modules_list.h"
#include "poses_list.h"

void AnimationModule::setup() {
    this->playPose(POSE_ID_stand, ANIMATION_LOOP);
    // this->playPose("Wave", 10);

    registerJsonRpcMethod("pose", [](const JsonVariant & params, JsonObject & response) -> uint16_t {
        if(!params["name"].is<const char *>()) {
            response["error"] = "Invalid params: missing name";
            return 400;
        }
        const char * name = params["name"];
        uint16_t count    = ANIMATION_LOOP;
        if(params["count"].is<uint16_t>()) {
            count = params["count"];
        }
        Pose_t * pose = findPoseByName(name);
        if(pose == nullptr) {
            response["error"] = "Pose not found";
            return 404;
        }
        if(!g_animationModule.playPose(pose, count)) {
            response["error"] = "Failed to play animation";
            return 500;
        }
        response["result"] = "Animation started successfully";
        return 200;
    });

    registerJsonRpcMethod("emergencyStop", [this](const JsonVariant & params, JsonObject & response) -> uint16_t {
        this->emitEvent(ANIMATION_EVENT_EMERGENCY_STOP, nullptr);
        return 200;
    });

    registerJsonRpcMethod("stop", [this](const JsonVariant & params, JsonObject & response) -> uint16_t {
        this->currentAnimationCount = 1;    // will stop after current loop
        return 200;
    });

    this->registerEventCallback([this](uint16_t event, void * data) {
        switch(event) {
            case ANIMATION_EVENT_EMERGENCY_STOP:
                log_e("Animation emergency stop triggered!");
                this->animationPlaying = false;
                this->runNow();
                break;
            case ANIMATION_EVENT_FINISHED:
                log_i("Animation finished");
                sendJsonRpcEvent("animation.finished");
                break;
        }
    });
}

bool AnimationModule::playPose(const String & name, uint16_t count) {
    Pose_t * pose = findPoseByName(name);
    if(pose == nullptr) {
        log_e("Pose not found: %s", name.c_str());
        return false;
    }
    return playPose(pose, count);
}

bool AnimationModule::playPose(PoseID id, uint16_t count) {
    if(id >= POSE_ID_MAX) {
        return false;
    }
    Pose_t * pose = const_cast<Pose_t *>(&poseEntries[id]);
    return playPose(pose, count);
}

bool AnimationModule::playPose(Pose_t * pose, uint16_t count) {
    if(pose == nullptr) {
        return false;
    }
    log_v("Playing pose: %s, count: %d", pose->name, count);
    if(pose->count == 0) {
        log_e("Pose has no steps: %s", pose->name);
        return false;
    }
    if(pose->count == 1 && count > 1) {
        log_w("Pose %s has only one step, switching to ANIMATION_ONCE mode", pose->name);
        count = ANIMATION_ONCE;
    }

    log_i("Playing pose: %s, count: %d", pose->name, count);
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
