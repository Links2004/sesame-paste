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
        this->runned();
        return;
    }
    this->loop();
    this->runned();
    if(this->loopOnceFlag) {
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

void debugPrintModules() {
    for(Module * module : Modules) {
        ;
        log_d("Module: %s (%X) enabled: %d canSleep: %d till: %ld", module->ThreadName.c_str(), module->ThreadID, module->enabled, module->canSleep, module->tillRun(millis()));
    }
}
