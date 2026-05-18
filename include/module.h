#pragma once

#include "sesame_paste.h"

#include <Thread.h>
#include <ThreadController.h>
#include <forward_list>

void registerModules(ThreadController * tControl);
void debugPrintModules();

extern void wakeMainLoop();
extern void wakeMainLoopFromISR();

typedef std::function<void(uint16_t event, void * data)> EventCallback;

class Module : public Thread {
  public:
    Module();
    ~Module();

    virtual void init(ThreadController * tControl) {
        this->tControl = tControl;
        tControl->add(this);
        log_i("%s Module registered", this->ThreadName.c_str());
    };

    inline void registerEventCallback(EventCallback callback) {
        this->eventCallbacks.push_front(callback);
    }

  protected:
    bool setupDone    = false;
    bool loopOnceFlag = false;

    std::forward_list<EventCallback> eventCallbacks;

    ThreadController * tControl;

    void emitEvent(uint16_t event, void * data) {
        for(EventCallback & callback : eventCallbacks) {
            callback(event, data);
        }
    }

    void run() override;
    void loopOnce();

    virtual String getName() {
        return ThreadName;
    };

    inline void wakeLoop() {
        log_v("wakeMainLoop %s", this->ThreadName.c_str());
        wakeMainLoop();
    }

    inline void wakeLoopISR() {
        log_v("wakeMainLoopFromISR %s", this->ThreadName.c_str());
        wakeMainLoopFromISR();
    }

    virtual void setup() {};
    virtual void loop() {
        this->enabled = false;    // default to disable loop if not implemented
    };
};
