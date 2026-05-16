#pragma once

#include "face-bitmaps.h"

// enum for face names
enum FaceID : uint8_t {
#define X(name, fps) FACE_ID_##name,
    FACE_LIST
#undef X
        FACE_ID_MAX
};

enum FaceAnimMode : uint8_t {
    FACE_ANIM_LOOP      = 0,
    FACE_ANIM_ONCE      = 1,
    FACE_ANIM_BOOMERANG = 2
};

struct FaceEntry {
    const char * name;
    const unsigned char * const * frames;
    uint8_t maxFrames;
    uint8_t fps;
};

class FaceDisplay;
extern FaceDisplay * display;

class FaceDisplay {
  public:
    FaceDisplay() {
        display = this;
    }
    virtual void displayFace(FaceID faceId, FaceAnimMode mode = FACE_ANIM_LOOP) = 0;
    virtual bool animationFinished()                                            = 0;
};

extern const FaceEntry faceEntries[];

typedef const unsigned char bitmap_t;
