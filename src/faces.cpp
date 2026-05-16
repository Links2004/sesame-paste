#include "sesame_paste.h"
#include "faces.h"

FaceDisplay * display = nullptr;

static const uint8_t MAX_FACE_FRAMES = 6;

#define MAKE_FACE_FRAMES(name)                                              \
    const unsigned char * const face_##name##_frames[] = {                  \
        epd_bitmap_##name, epd_bitmap_##name##_1, epd_bitmap_##name##_2,    \
        epd_bitmap_##name##_3, epd_bitmap_##name##_4, epd_bitmap_##name##_5 \
    };

#define X(name, fps) MAKE_FACE_FRAMES(name)
FACE_LIST
#undef X
#undef MAKE_FACE_FRAMES

const FaceEntry faceEntries[] = {
#define X(name, fps) { #name, face_##name##_frames, MAX_FACE_FRAMES, fps },
    FACE_LIST
#undef X
    { "default", face_defualt_frames, MAX_FACE_FRAMES, 1 }
};
