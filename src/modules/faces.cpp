#include "modules/faces.h"

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

FaceEntry faceEntries[] = {
#define X(name, fps) { #name, face_##name##_frames, MAX_FACE_FRAMES, fps },
    FACE_LIST
#undef X
    { "default", face_rest_frames, MAX_FACE_FRAMES, 1 }
};

void FaceModule::setup() {
    this->calculateMaxFrames();
    this->displayFace(FACE_ID_rest, FACE_ANIM_BOOMERANG);
}

void FaceModule::loop() {
    const FaceEntry & entry = faceEntries[this->currentFaceId];
    log_v("loop: face: %s (%d), mode: %d, step: %d, max: %d", entry.name, this->currentFaceId, this->currentFaceMode, this->currentFaceStep, entry.maxFrames);
    if(entry.maxFrames <= 0) {
        this->enabled  = false;
        this->canSleep = true;
        return;
    }

    bitmap_t * bitmap = entry.frames[this->currentFaceStep];
    this->emitEvent(FACE_EVENT_NEW_BITMAP, (void *)bitmap);

    if(this->currentFaceMode == FACE_ANIM_ONCE) {
        this->currentFaceStep++;
        this->faceAnimFinished = this->currentFaceStep >= entry.maxFrames - 1;
    } else if(this->currentFaceMode == FACE_ANIM_LOOP) {
        this->currentFaceStep = (this->currentFaceStep + 1) % entry.maxFrames;
    } else if(this->currentFaceMode == FACE_ANIM_BOOMERANG) {
        if(this->faceAnimationDirectionReverse) {
            this->currentFaceStep--;
            if(this->currentFaceStep < 0) {
                this->currentFaceStep               = 1;
                this->faceAnimationDirectionReverse = false;
            }
        } else {
            this->currentFaceStep++;
            if(this->currentFaceStep >= entry.maxFrames) {
                this->currentFaceStep               = entry.maxFrames - 2;
                this->faceAnimationDirectionReverse = true;
            }
        }
    }

    if(this->faceAnimFinished) {
        this->enabled  = false;
        this->canSleep = true;
        this->emitEvent(FACE_EVENT_ANIMATION_FINISHED, nullptr);
        return;
    }

    assert(this->currentFaceStep >= 0);
    assert(this->currentFaceStep < entry.maxFrames);

    this->enabled  = true;
    this->canSleep = false;
}

void FaceModule::displayFace(FaceID faceId, FaceAnimMode mode) {
    if(faceId >= FACE_ID_MAX) {
        faceId = FACE_ID_rest;
    }
    this->currentFaceId                 = faceId;
    this->currentFaceMode               = mode;
    this->currentFaceStep               = 0;
    this->faceAnimationDirectionReverse = false;
    this->faceAnimFinished              = false;

    // Calculate interval based on fps for the face
    unsigned long interval = 1000;    // 1 fps
    if(faceEntries[faceId].fps > 0) {
        interval = 1000 / faceEntries[faceId].fps;
    }

    const FaceEntry & entry = faceEntries[this->currentFaceId];

    if(entry.maxFrames <= 0) {
        log_w("Face %s has no frames, cannot display.", entry.name);
        return;
    } else if(entry.maxFrames == 1) {
        // If there's only one frame, treat it as a static image and don't loop
        this->currentFaceMode = FACE_ANIM_ONCE;
    } else if(entry.maxFrames == 2 && mode == FACE_ANIM_BOOMERANG) {
        // there's only 2 frames, boomerang mode code not work
        // fallback to loop mode which is effectively the same as boomerang in this case
        this->currentFaceMode = FACE_ANIM_LOOP;
    }

    log_i("Displaying face: %s(%d) mode: %d interval: %lu ms", entry.name, faceId, mode, interval);
    this->setInterval(interval);
    this->enabled  = true;
    this->canSleep = false;
};

void FaceModule::calculateMaxFrames() {
    for(uint8_t i = 0; i < FACE_ID_MAX; i++) {
        FaceEntry & entry = faceEntries[i];
        // Ensure maxFrames is set to MAX_FACE_FRAMES
        // since this is how the define builds the struct
        // anything else would be a bug in the define or the struct initialization
        assert(entry.maxFrames == MAX_FACE_FRAMES);
        entry.maxFrames = 0;

        // Calculate maxFrames for each face entry based on the frames array
        for(uint8_t j = 0; j < MAX_FACE_FRAMES; j++) {
            if(entry.frames == nullptr || entry.frames[j] == nullptr) {
                break;
            }
            entry.maxFrames++;
        }

        if(entry.maxFrames == 0) {
            log_e("Face %s (index %d) has no frames defined.", entry.name ? entry.name : "(null)", i);
        }
    }
    log_i("Initialized %d face entries.", FACE_ID_MAX);
}
