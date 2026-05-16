#include "module.h"

static std::forward_list<Module *> Modules;

Module::Module() : Thread() {
    Modules.push_front(this);
};

Module::~Module() {
    Modules.remove(this);
    this->enabled = false;
    if(this->tControl) {
        this->tControl->remove(this);
    }
};

void Module::loopOnce() {
    this->loopOnceFlag = true;
    this->enabled      = true;
    this->canSleep     = false;
    this->setInterval(0);    // run as soon as possible
}

void Module::run() {
    if(!setupDone) {
        this->setup();
        setupDone = true;
        return;
    }
    this->loop();
    this->runned();
    if(this->loopOnceFlag) {
        this->setInterval(__LONG_MAX__);    // effectively disable until next wakeLoop
        this->enabled      = false;
        this->canSleep     = true;
        this->loopOnceFlag = false;
    }
};

void registerModules(ThreadController * tControl) {
    for(Module * module : Modules) {
        module->init(tControl);
    }
}

void debugPrintModules(Print * printer) {
    for(Module * module : Modules) {
        printer->printf(F("Module: %p, enabled: %d, canSleep: %d\n"), module, module->enabled, module->canSleep);
    }
}
