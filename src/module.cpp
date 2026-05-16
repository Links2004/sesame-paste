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
