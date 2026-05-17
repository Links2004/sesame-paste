#pragma once

#include "sesame_paste.h"

#include <Thread.h>
#include <ThreadController.h>
#include <forward_list>

void registerModules(ThreadController * tControl);
void debugPrintModules();

extern void wakeMainLoop();
extern void wakeMainLoopFromISR();

class Module : public Thread {
  public:
    Module();
    ~Module();

    virtual void init(ThreadController * tControl) {
        this->tControl = tControl;
        tControl->add(this);
    };

  protected:
    bool setupDone    = false;
    bool loopOnceFlag = false;

    ThreadController * tControl;

    void run() override;
    void loopOnce();

    virtual String getName() {
        return ThreadName;
    };

    void inline wakeLoop() {
        wakeMainLoop();
    }

    void inline wakeLoopISR() {
        wakeMainLoopFromISR();
    }

    virtual void setup() {};
    virtual void loop() {
        this->enabled = false;    // default to disable loop if not implemented
    };
};
