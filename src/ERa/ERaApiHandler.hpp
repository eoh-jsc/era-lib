#ifndef INC_ERA_API_HANDLER_HPP_
#define INC_ERA_API_HANDLER_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <ERa/ERaDefine.hpp>
#include <ERa/ERaDebug.hpp>
#include <ERa/ERaTimer.hpp>
#include <ERa/ERaCallbacksHelper.hpp>
#include <Utility/ERacJSON.hpp>

class ERaApiHandler
    : public ERaCallbacksHelper
{
    const char* TAG = "API";

public:
    ERaApiHandler()
    {}
    virtual ~ERaApiHandler()
    {}

#if defined(ERA_SPECIFIC)
    virtual void specificWrite(const char* id, cJSON* value) {
        return this->specificDataWrite(id, value, true, false);
    }

    virtual void specificWrite(const char* id, const char* value) {
        return this->specificDataWrite(id, value, true, false);
    }

    virtual void specificDataWrite(const char* id, cJSON* value,
                                   bool specific = false,
                                   bool retained = ERA_MQTT_PUBLISH_RETAINED) {
        ERA_LOG(TAG, ERA_PSTR("specificDataWrite default."));
        ERA_FORCE_UNUSED(id);
        ERA_FORCE_UNUSED(value);
        ERA_FORCE_UNUSED(specific);
        ERA_FORCE_UNUSED(retained);
    }

    virtual void specificDataWrite(const char* id, const char* value,
                                   bool specific = false,
                                   bool retained = ERA_MQTT_PUBLISH_RETAINED) {
        ERA_LOG(TAG, ERA_PSTR("specificDataWrite default."));
        ERA_FORCE_UNUSED(id);
        ERA_FORCE_UNUSED(value);
        ERA_FORCE_UNUSED(specific);
        ERA_FORCE_UNUSED(retained);
    }
#endif

    virtual char* readFromFlash(const char* filename, bool force = false) {
        ERA_LOG(TAG, ERA_PSTR("readFromFlash default."));
        ERA_FORCE_UNUSED(filename);
        ERA_FORCE_UNUSED(force);
        return NULL;
    }

    virtual void writeToFlash(const char* filename, const char* buf,
                                                    bool force = false) {
        ERA_LOG(TAG, ERA_PSTR("writeToFlash default."));
        ERA_FORCE_UNUSED(filename);
        ERA_FORCE_UNUSED(buf);
        ERA_FORCE_UNUSED(force);
    }

    template <typename... Args>
    ERaTimer::iterator addInterval(unsigned long interval, Args... tail) {
        return this->ERaTm.setInterval(interval, tail...);
    }

    template <typename... Args>
    ERaTimer::iterator addTimeout(unsigned long interval, Args... tail) {
        return this->ERaTm.setTimeout(interval, tail...);
    }

    template <typename... Args>
    ERaTimer::iterator addTimer(unsigned long interval, Args... tail) {
        return this->ERaTm.setTimer(interval, tail...);
    }

    virtual bool connected() = 0;

protected:
    void run() {
        this->ERaTm.run();
    }

    virtual void connectNewWiFi(const char* ssid, const char* pass) {
        ERA_LOG(TAG, ERA_PSTR("connectNewWiFi default."));
        ERA_FORCE_UNUSED(ssid);
        ERA_FORCE_UNUSED(pass);
    }

private:
    ERaTimer ERaTm;
};

#endif /* INC_ERA_API_HANDLER_HPP_ */
