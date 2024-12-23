#ifndef INC_ERA_API_HANDLER_HPP_
#define INC_ERA_API_HANDLER_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <ERa/ERaDefine.hpp>
#include <ERa/ERaConfig.hpp>
#include <ERa/ERaState.hpp>
#include <ERa/ERaDebug.hpp>
#include <ERa/ERaTimer.hpp>
#include <ERa/ERaCallbackSetter.hpp>
#include <Utility/ERacJSON.hpp>

class ERaApiHandler
    : public ERaCallbackSetter
{
    const char* TAG = "API";

public:
    ERaApiHandler()
        : ERaCallbackSetter(*this)
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
        ERA_LOG_WARNING(TAG, ERA_PSTR("specificDataWrite default."));
        ERA_FORCE_UNUSED(id);
        ERA_FORCE_UNUSED(value);
        ERA_FORCE_UNUSED(specific);
        ERA_FORCE_UNUSED(retained);
    }

    virtual void specificDataWrite(const char* id, const char* value,
                                   bool specific = false,
                                   bool retained = ERA_MQTT_PUBLISH_RETAINED) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("specificDataWrite default."));
        ERA_FORCE_UNUSED(id);
        ERA_FORCE_UNUSED(value);
        ERA_FORCE_UNUSED(specific);
        ERA_FORCE_UNUSED(retained);
    }
#endif

    virtual void sendNotify(ERaUInt_t automateId, ERaUInt_t notifyId) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("sendNotify default."));
        ERA_FORCE_UNUSED(automateId);
        ERA_FORCE_UNUSED(notifyId);
    }

    virtual void sendEmail(ERaUInt_t automateId, ERaUInt_t emailId) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("sendEmail default."));
        ERA_FORCE_UNUSED(automateId);
        ERA_FORCE_UNUSED(emailId);
    }

    virtual void beginFlash() {
        ERA_LOG_WARNING(TAG, ERA_PSTR("beginFlash default."));
    }

    virtual char* readFromFlash(const char* filename, bool force = false) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("readFromFlash default."));
        ERA_FORCE_UNUSED(filename);
        ERA_FORCE_UNUSED(force);
        return NULL;
    }

    virtual void writeToFlash(const char* filename, const char* buf,
                                                    bool force = false) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("writeToFlash default."));
        ERA_FORCE_UNUSED(filename);
        ERA_FORCE_UNUSED(buf);
        ERA_FORCE_UNUSED(force);
    }

    virtual size_t readBytesFromFlash(const char* key, void* buf, size_t maxLen,
                                                    bool force = false) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("readBytesFromFlash default."));
        ERA_FORCE_UNUSED(key);
        ERA_FORCE_UNUSED(buf);
        ERA_FORCE_UNUSED(maxLen);
        ERA_FORCE_UNUSED(force);
        return 0;
    }

    virtual void writeBytesToFlash(const char* key, const void* value, size_t len,
                                                    bool force = false) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("writeBytesToFlash default."));
        ERA_FORCE_UNUSED(key);
        ERA_FORCE_UNUSED(value);
        ERA_FORCE_UNUSED(len);
        ERA_FORCE_UNUSED(force);
    }

    virtual void callERaWriteHandler(uint8_t pin, const ERaParam& param) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("callERaWriteHandler default."));
        ERA_FORCE_UNUSED(pin);
        ERA_FORCE_UNUSED(param);
    }

    virtual void callERaPinReadHandler(uint8_t pin, const ERaParam& param, const ERaParam& raw) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("callERaPinReadHandler default."));
        ERA_FORCE_UNUSED(pin);
        ERA_FORCE_UNUSED(param);
        ERA_FORCE_UNUSED(raw);
    }

    virtual bool callERaPinWriteHandler(uint8_t pin, const ERaParam& param, const ERaParam& raw) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("callERaPinWriteHandler default."));
        ERA_FORCE_UNUSED(pin);
        ERA_FORCE_UNUSED(param);
        ERA_FORCE_UNUSED(raw);
        return false;
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

    void runTimer() {
        this->ERaTm.run();
    }

    virtual bool afterNetwork() {
        return (ERaState::is(StateT::STATE_RUNNING) ||
                ERaState::is(StateT::STATE_CONNECTED) ||
                ERaState::is(StateT::STATE_CONNECTING_CLOUD));
    }

    virtual bool connected() = 0;

protected:
    void run() {
        this->ERaTm.run();
    }

    virtual void requestListWiFi() {
        ERA_LOG_WARNING(TAG, ERA_PSTR("requestListWiFi default."));
    }

    virtual void responseListWiFi() {
        ERA_LOG_WARNING(TAG, ERA_PSTR("responseListWiFi default."));
    }

    virtual void connectNewWiFi(const char* ssid, const char* pass) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("connectNewWiFi default."));
        ERA_FORCE_UNUSED(ssid);
        ERA_FORCE_UNUSED(pass);
    }

    virtual void connectNewNetworkResult() {
        ERA_LOG_WARNING(TAG, ERA_PSTR("connectNewNetworkResult default."));
    }

    virtual void sendSMS(const char* to, const char* message) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("sendSMS default."));
        ERA_FORCE_UNUSED(to);
        ERA_FORCE_UNUSED(message);
    }

private:
    ERaTimer ERaTm;
};

#endif /* INC_ERA_API_HANDLER_HPP_ */
