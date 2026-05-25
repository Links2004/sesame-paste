#include "pose.h"
#include "poses_list.h"

// One static const array per pose — stored in flash (.rodata)
#define X(strname, loopStart, ...) static const PoseStep_t poseSteps_##strname[] = __VA_ARGS__;
POSES_LIST
#undef X

const Pose_t poseEntries[] = {
#define X(strname, loopStart, ...) { .name = #strname, .poses = poseSteps_##strname, .count = sizeof(poseSteps_##strname) / sizeof(PoseStep_t), .loop_start = loopStart },
    POSES_LIST
#undef X
};

Pose_t * findPoseByName(const String & name) {
    for(uint8_t i = 0; i < POSE_ID_MAX; i++) {
        const Pose_t & p = poseEntries[i];
        if(name == p.name) {
            return const_cast<Pose_t *>(&p);
        }
    }
    return nullptr;
}

void debugPrintPoses() {
    log_d("Available poses:");
    for(uint8_t i = 0; i < POSE_ID_MAX; i++) {
        log_d("- %s %d", poseEntries[i].name, poseEntries[i].count);
    }
}
