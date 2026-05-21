#pragma once
#include "sesame_paste.h"
#include "face-bitmaps.h"

// enum for face names
enum FaceID : uint8_t {
#define X(name, fps) FACE_ID_##name,
    FACE_LIST
#undef X
        FACE_ID_MAX,
    FACE_ID_NONE = UINT8_MAX
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

enum FaceEvent : uint16_t {
    FACE_EVENT_NEW_BITMAP,
    FACE_EVENT_ANIMATION_FINISHED
};

class FaceModule : public Module {
  public:
    FaceModule() {
        ThreadName = "Face";
    }

    void displayFace(FaceID faceId, FaceAnimMode mode = FACE_ANIM_LOOP);

    bool isAnimationFinished() {
        return this->faceAnimFinished;
    }

    FaceID getCurrentFaceID() {
        return this->currentFaceId;
    }

  protected:
    void setup() override;
    void loop() override;

    void calculateMaxFrames();

    FaceID currentFaceId;
    FaceAnimMode currentFaceMode;

    int8_t currentFaceStep;
    bool faceAnimationDirectionReverse;
    bool faceAnimFinished;
};

extern FaceEntry faceEntries[];

typedef const unsigned char bitmap_t;
