#pragma once

//            angle     = R1, R2, L1, L2, R4, R3, L3, L4
// (optional) duration  = ms to delay after this step
// (optional) face      = override face to use during this step
// (optional) face_mode = FaceAnimMode

#define NA SERVO_NO_CHANGE

#define ANGLE_STAND { 135, 45, 45, 135, 0, 180, 0, 180 }
#define ANGLE_REST { 90, 90, 90, 90, 90, 90, 90, 90 }

#define POSES_LIST                                                                                                                         \
    X(Rest, 0, {                                                                                                                           \
                   { .angle = ANGLE_REST },                                                                                                \
               })                                                                                                                          \
    X(Stand, 0, {                                                                                                                          \
                    { .angle = ANGLE_STAND },                                                                                              \
                })                                                                                                                         \
    X(Wave, 2, {                                                                                                                           \
                   { .angle = ANGLE_STAND, .face = FACE_ID_wave, .face_mode = FACE_ANIM_ONCE, .duration = 200 },                           \
                   { .angle = { 100, NA, NA, 90, 80, NA, 180, NA }, .duration = 200 },                                                     \
                   { .angle = { NA, NA, NA, NA, NA, NA, 100, NA }, .duration = 300 },                                                      \
                   { .angle = { NA, NA, NA, NA, NA, NA, 180, NA }, .duration = 300 },                                                      \
               })                                                                                                                          \
    X(Dance, 1, {                                                                                                                          \
                    { .angle = { 90, 90, 90, 90, 160, 160, 10, 10 }, .face = FACE_ID_dance, .duration = 300 },                             \
                    { .angle = { NA, NA, NA, NA, 115, 115, 10, 10 }, .duration = 300 },                                                    \
                    { .angle = { NA, NA, NA, NA, 160, 160, 65, 65 }, .duration = 300 },                                                    \
                })                                                                                                                         \
    X(Swim, 1, {                                                                                                                           \
                   { .angle = ANGLE_REST, .face = FACE_ID_swim, .face_mode = FACE_ANIM_ONCE },                                             \
                   { .angle = { 135, 45, 45, 135, NA, NA, NA, NA }, .duration = 400 },                                                     \
                   { .angle = ANGLE_REST, .duration = 400 },                                                                               \
               })                                                                                                                          \
    X(Point, 1, {                                                                                                                          \
                    { .angle = ANGLE_STAND, .face = FACE_ID_point, .face_mode = FACE_ANIM_BOOMERANG },                                     \
                    { .angle = { 135, 100, 25, 90, 80, 170, 145, 180 }, .duration = 2000 },                                                \
                })                                                                                                                         \
    X(Pushup, 2, {                                                                                                                         \
                     { .angle = ANGLE_STAND, .face = FACE_ID_pushup, .face_mode = FACE_ANIM_ONCE, .duration = 200 },                       \
                     { .angle = { 180, NA, 0, NA, NA, 90, 90, NA }, .duration = 500 },                                                     \
                     { .angle = { NA, NA, NA, NA, NA, 0, 180, NA }, .duration = 600 },                                                     \
                     { .angle = { NA, NA, NA, NA, NA, 90, 90, NA }, .duration = 500 },                                                     \
                 })                                                                                                                        \
    X(Bow, 1, {                                                                                                                            \
                  { .angle = ANGLE_STAND, .face = FACE_ID_bow, .face_mode = FACE_ANIM_BOOMERANG, .duration = 200 },                        \
                  { .angle = { 180, 0, 0, 180, 0, 180, 0, 180 }, .duration = 600 },                                                        \
                  { .angle = { NA, NA, NA, NA, NA, 90, 90, NA }, .duration = 3000 },                                                       \
              })                                                                                                                           \
    X(Cute, 2, {                                                                                                                           \
                   { .angle = ANGLE_STAND, .face = FACE_ID_cute, .face_mode = FACE_ANIM_ONCE, .duration = 200 },                           \
                   { .angle = { 180, 20, 0, 160, 180, 0, 180, 0 }, .duration = 200 },                                                      \
                   { .angle = { NA, NA, NA, NA, 180, NA, NA, 45 }, .duration = 300 },                                                      \
                   { .angle = { NA, NA, NA, NA, 135, NA, NA, 0 }, .duration = 300 },                                                       \
               })                                                                                                                          \
    X(Freaky, 2, {                                                                                                                         \
                     { .angle = ANGLE_STAND, .face = FACE_ID_freaky, .face_mode = FACE_ANIM_ONCE, .duration = 200 },                       \
                     { .angle = { 180, 0, 0, 180, 90, 0, NA, NA }, .duration = 200 },                                                      \
                     { .angle = { NA, NA, NA, NA, NA, 25, NA, NA }, .duration = 400 },                                                     \
                     { .angle = { NA, NA, NA, NA, NA, 0, NA, NA }, .duration = 400 },                                                      \
                 })                                                                                                                        \
    X(Worm, 2, {                                                                                                                           \
                   { .angle = ANGLE_STAND, .face = FACE_ID_worm, .face_mode = FACE_ANIM_ONCE, .duration = 200 },                           \
                   { .angle = { 180, 0, 0, 180, 90, 90, 90, 90 }, .duration = 200 },                                                       \
                   { .angle = { NA, NA, NA, NA, 45, 45, 135, 135 }, .duration = 300 },                                                     \
                   { .angle = { NA, NA, NA, NA, 135, 135, 45, 45 }, .duration = 300 },                                                     \
               })                                                                                                                          \
    X(Shake, 2, {                                                                                                                          \
                    { .angle = ANGLE_STAND, .face = FACE_ID_shake, .face_mode = FACE_ANIM_ONCE, .duration = 200 },                         \
                    { .angle = { 135, 90, 45, 90, NA, 90, 90, NA }, .duration = 200 },                                                     \
                    { .angle = { NA, NA, NA, NA, 45, NA, NA, 135 }, .duration = 300 },                                                     \
                    { .angle = { NA, NA, NA, NA, 0, NA, NA, 180 }, .duration = 300 },                                                      \
                })                                                                                                                         \
    X(Shrug, 2, {                                                                                                                          \
                    { .angle = ANGLE_STAND, .face = FACE_ID_dead, .face_mode = FACE_ANIM_ONCE, .duration = 200 },                          \
                    { .angle = { NA, NA, NA, NA, 90, 90, 90, 90 }, .duration = 1000 },                                                     \
                    { .angle = { NA, NA, NA, NA, 180, 0, 180, 0 }, .face = FACE_ID_shrug, .face_mode = FACE_ANIM_ONCE, .duration = 1500 }, \
                })                                                                                                                         \
    X(Dead, 1, {                                                                                                                           \
                   { .angle = ANGLE_STAND, .face = FACE_ID_dead, .face_mode = FACE_ANIM_BOOMERANG, .duration = 200 },                      \
                   { .angle = { NA, NA, NA, NA, 90, 90, 90, 90 }, .duration = 0 },                                                         \
               })                                                                                                                          \
    X(Crab, 1, {                                                                                                                           \
                   { .angle = ANGLE_STAND, .face = FACE_ID_crab, .face_mode = FACE_ANIM_ONCE, .duration = 200 },                           \
                   { .angle = { 90, 90, 90, 90, 0, 180, 45, 135 }, .duration = 300 },                                                      \
                   { .angle = { NA, NA, NA, NA, 45, 135, 0, 180 }, .duration = 300 },                                                      \
               })

//{ .angle = { NA, NA, NA, NA, NA, NA, NA, NA }, .duration = 200 }, \
//
