#pragma once

template<typename T = bool>
class Poll2Event {
    typedef std::function<void(T value)> cb_t;

  public:
    Poll2Event() {};

    bool set(T value) {
        if(!initialized || value != lastValue) {
            initialized = true;
            lastValue   = value;
            for(cb_t & callback : eventCallbacks) {
                callback(value);
            }
            return true;
        }
        return false;
    }

    inline T get() {
        return lastValue;
    }

    inline void registerEventCallback(cb_t callback) {
        this->eventCallbacks.push_front(callback);
    }

  protected:
    bool initialized = false;
    T lastValue      = {};

    std::forward_list<cb_t> eventCallbacks;
};
