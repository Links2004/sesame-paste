#include "sesame_paste.h"
#include "faces.h"
#include "esp_log.h"

static const char * TAG = "faces";

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

FaceEntry faceEntries[] = {
#define X(name, fps) { #name, face_##name##_frames, MAX_FACE_FRAMES, fps },
    FACE_LIST
#undef X
    { "default", face_rest_frames, MAX_FACE_FRAMES, 1 }
};

void initFaceEntries() {
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
            ESP_LOGE(TAG, "Face %s (index %d) has no frames defined.", entry.name ? entry.name : "(null)", i);
        }
    }
    ESP_LOGI(TAG, "Initialized %d face entries.", FACE_ID_MAX);
}
