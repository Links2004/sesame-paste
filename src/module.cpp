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

void IRAM_ATTR Module::runOnce() {
    if(unlikely(this->inLoop)) {
        this->loopOnceNextFlag = true;
    } else {
        this->loopOnceFlag = true;
    }
    this->runNow();
}

void IRAM_ATTR Module::runOnceIn(unsigned long ms) {
    if(unlikely(this->inLoop)) {
        this->loopOnceNextFlag = true;
    } else {
        this->loopOnceFlag = true;
    }
    this->runIn(ms);
}

void IRAM_ATTR Module::runNow() {
    this->runIn(0);
}

void IRAM_ATTR Module::runIn(unsigned long ms) {
    this->enabled  = true;
    this->canSleep = false;
    this->setInterval(ms);
    this->runned();    // reset timer to count from now
    this->wakeLoop();
}

void Module::run() {
    if(!setupDone) {
        this->setup();
        setupDone = true;
        this->runned();
        return;
    }
    this->inLoop = true;
    this->loop();
    this->inLoop = false;
    this->runned();

    if(this->loopOnceFlag) {
        this->loopOnceFlag = false;
        this->enabled      = false;
        this->canSleep     = true;
    }
    if(this->loopOnceNextFlag) {
        this->loopOnceNextFlag = false;
        this->loopOnceFlag     = true;
        this->enabled          = true;
        this->canSleep         = false;
    }
};

void Module::registerModules(ThreadController * tControl) {
    for(Module * module : Modules) {
        module->init(tControl);
    }
}

void Module::debugPrintModules() {
    for(Module * module : Modules) {
        log_d("Module: %s (%X) enabled: %d canSleep: %d till: %ld", module->ThreadName.c_str(), module->ThreadID, module->enabled, module->canSleep, module->tillRun(millis()));
    }
}
