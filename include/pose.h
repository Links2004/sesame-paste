#pragma once

#include <WString.h>
#include "modules/faces.h"
#include "modules/servo.h"

typedef struct {
    int angle[SERVO_MAX]   = { 0 };
    FaceID face            = FACE_ID_NONE;
    FaceAnimMode face_mode = FACE_ANIM_LOOP;
    long duration          = 0;
} PoseStep_t;

typedef struct {
    const char * name;
    const PoseStep_t * poses;
    uint8_t count;
    uint8_t loop_start;
} Pose_t;

#include "poses_list.h"

// enum for pose names
enum PoseID : uint8_t {
#define X(name, loop_start, ...) POSE_ID_##name,
    POSES_LIST
#undef X
        POSE_ID_MAX,
    POSE_ID_NONE = UINT8_MAX
};

extern const Pose_t poseEntries[];

Pose_t * findPoseByName(const String & name);
void debugPrintPoses();
