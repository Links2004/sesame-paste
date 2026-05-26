#pragma once
#include "sesame_paste.h"
#include "module.h"

#include "pose.h"

#define ANIMATION_ONCE 1
#define ANIMATION_LOOP UINT16_MAX

enum AnimationEvent : uint16_t {
    ANIMATION_EVENT_FINISHED,
    ANIMATION_EVENT_EMERGENCY_STOP,
};

class AnimationModule : public Module {
  public:
    AnimationModule() : Module() {
        ThreadName = "Animation";
    };

    bool playPose(const String & name, uint16_t count = ANIMATION_LOOP);
    bool playPose(PoseID id, uint16_t count = ANIMATION_LOOP);
    bool playPose(Pose_t * pose, uint16_t count = ANIMATION_LOOP);

  protected:
    void setup() override;
    void loop() override;

    bool animationPlaying          = false;
    uint16_t currentAnimationCount = 0;
    uint8_t currentAnimationStep   = 0;
    Pose_t * currentPose           = nullptr;
};
