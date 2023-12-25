#ifndef INC_ERA_API_PARTICLE_HPP_
#define INC_ERA_API_PARTICLE_HPP_

#include "application.h"
#include <ERa/ERaApi.hpp>

inline
static int digitalReadParticle(uint8_t pin) {
    return digitalRead(pin);
}

inline
static int analogReadParticle(uint8_t pin) {
    return analogRead(pin);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::handleReadPin(cJSON* root) {
    if (!cJSON_IsArray(root)) {
        return;
    }

    PinConfig_t pin {};
    cJSON* current = nullptr;

    for (current = root->child; current != nullptr; current = current->next) {
        if (!cJSON_IsObject(current)) {
            continue;
        }
        pin = PinConfig_t();
        cJSON* item = cJSON_GetObjectItem(current, "config_id");
        if (cJSON_IsNumber(item)) {
            pin.configId = item->valueint;
        }
        item = cJSON_GetObjectItem(current, "pin_number");
        if (cJSON_IsNumber(item)) {
            pin.pin = ERA_DECODE_PIN_NUMBER(item->valueint);
        }
        else if (cJSON_IsString(item)) {
            pin.pin = ERA_DECODE_PIN_NAME(item->valuestring);
        }
        ERA_CHECK_PIN(pin.pin);
        pin.pinMode = this->getPinMode(current);
        if (pin.pinMode == VIRTUAL) {
            this->ERaPinRp.setPinVirtual(pin.pin, pin.configId);
            continue;
        }
        if (!this->isReadPinMode(pin.pinMode)) {
            continue;
        }
        item = cJSON_GetObjectItem(current, "value_type");
        if (cJSON_IsString(item)) {
            if (this->skipPinReport) {
                this->ERaPinRp.setPinRaw(pin.pin, pin.configId);
            }
            else if (ERaStrCmp(item->valuestring, "boolean")) {
                this->getPinConfig(current, pin);
                pinMode(pin.pin, (PinMode)pin.pinMode);
                this->ERaPinRp.setPinReport(pin.pin, pin.pinMode, digitalReadParticle,
                                            pin.report.interval, pin.report.minInterval,
                                            pin.report.maxInterval, pin.report.reportableChange,
                                            this->reportPinConfigCb, pin.configId);
            }
            else if (ERaStrCmp(item->valuestring, "integer")) {
                pin.report = PinConfig_t::__ReportConfig_t(1000, 1000, 60000, 10.0f);
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
                pinMode(pin.pin, ANALOG);
                this->ERaPinRp.setPinReport(pin.pin, ANALOG, analogReadParticle,
                                            pin.report.interval, pin.report.minInterval,
                                            pin.report.maxInterval, pin.report.reportableChange,
                                            this->reportPinConfigCb, pin.configId).setScale(pin.scale.min,
                                            pin.scale.max, pin.scale.rawMin, pin.scale.rawMax);
            }
            else if (ERaStrCmp(item->valuestring, "virtual")) {
                this->ERaPinRp.setPinVirtual(pin.pin, pin.configId);
            }
        }
    }
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::handleWritePin(cJSON* root) {
    if (!cJSON_IsArray(root)) {
        return;
    }

    PinConfig_t pin {};
    cJSON* current = nullptr;

    for (current = root->child; current != nullptr; current = current->next) {
        if (!cJSON_IsObject(current)) {
            continue;
        }
        pin = PinConfig_t();
        cJSON* item = cJSON_GetObjectItem(current, "config_id");
        if (cJSON_IsNumber(item)) {
            pin.configId = item->valueint;
        }
        item = cJSON_GetObjectItem(current, "pin_number");
        if (cJSON_IsNumber(item)) {
            pin.pin = ERA_DECODE_PIN_NUMBER(item->valueint);
        }
        else if (cJSON_IsString(item)) {
            pin.pin = ERA_DECODE_PIN_NAME(item->valuestring);
        }
        ERA_CHECK_PIN(pin.pin);
        pin.pinMode = this->getPinMode(current);
        if (pin.pinMode == VIRTUAL) {
            this->ERaPinRp.setPinVirtual(pin.pin, pin.configId);
            continue;
        }
        if (this->isReadPinMode(pin.pinMode)) {
            continue;
        }
        item = cJSON_GetObjectItem(current, "value_type");
        if (cJSON_IsString(item)) {
            if (this->skipPinReport) {
                this->ERaPinRp.setPinRaw(pin.pin, pin.configId);
            }
            else if (ERaStrCmp(item->valuestring, "boolean")) {
                this->getPinConfig(current, pin);
                pinMode(pin.pin, (PinMode)pin.pinMode);
                this->ERaPinRp.setPinReport(pin.pin, pin.pinMode, digitalReadParticle,
                                            pin.report.interval, pin.report.minInterval,
                                            pin.report.maxInterval, pin.report.reportableChange,
                                            this->reportPinConfigCb, pin.configId);
            }
            else if (ERaStrCmp(item->valuestring, "integer")) {
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
                pinMode(pin.pin, OUTPUT);
                this->ERaPinRp.setPinReport(pin.pin, PWM, nullptr,
                                            pin.report.interval, pin.report.minInterval,
                                            pin.report.maxInterval, pin.report.reportableChange,
                                            this->reportPinConfigCb, pin.configId).setScale(pin.scale.min,
                                            pin.scale.max, pin.scale.rawMin, pin.scale.rawMax);
            }
            else if (ERaStrCmp(item->valuestring, "virtual")) {
                this->ERaPinRp.setPinVirtual(pin.pin, pin.configId);
            }
        }
    }
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::processArduinoPinRequest(const ERaDataBuff& arrayTopic, const char* payload) {
    if (arrayTopic.size() != 3) {
        return;
    }
    const char* str = arrayTopic.at(2).getString();
    if (str == nullptr) {
        return;
    }
    cJSON* root = cJSON_Parse(payload);
    if (!cJSON_IsObject(root)) {
        cJSON_Delete(root);
        root = nullptr;
        return;
    }
    ERaDataJson data(root);
    ERaParam param(data);
    uint8_t pin = ERA_DECODE_PIN_NAME(str);
    ERA_CHECK_PIN_RETURN(pin);
    cJSON* item = cJSON_GetObjectItem(root, "value");
    if (cJSON_IsNumber(item) ||
        cJSON_IsBool(item)) {
        ERaParam raw;
        param.add(item->valuedouble);
        float value = item->valuedouble;
        int pMode = this->ERaPinRp.findPinMode(pin);
        const ERaReport::iterator* rp = this->ERaPinRp.getReport(pin);
        if (rp != nullptr) {
            const ERaReport::ScaleData_t* scale = rp->getScale();
            if ((scale != nullptr) && scale->enable) {
                value = ERaMapNumberRange((float)item->valuedouble, scale->min, scale->max, scale->rawMin, scale->rawMax);
            }
        }
        switch (pMode) {
            case RAW_PIN:
            case VIRTUAL:
            case ERA_VIRTUAL:
                break;
            default:
                raw = value;
                if (this->callERaPinWriteHandler(pin, param, raw) ||
                    this->skipPinWrite) {
                    pMode = RAW_PIN;
                }
                break;
        }
        switch (pMode) {
            case PWM:
            case ANALOG:
                ::analogWrite(pin, value);
                if (rp != nullptr) {
                    rp->updateReport(value, true);
                }
                break;
            case RAW_PIN:
                break;
            case VIRTUAL:
            case ERA_VIRTUAL:
                this->callERaWriteHandler(pin, param);
                break;
            default:
                pinMode(pin, OUTPUT);
                if (value == TOGGLE) {
                    ::digitalWrite(pin, ((digitalReadParticle(pin) == LOW) ? HIGH : LOW));
                }
                else {
                    ::digitalWrite(pin, value ? HIGH : LOW);
                }
                break;
        }
    }
    else if (cJSON_IsString(item)) {
        param.add_static(item->valuestring);
        this->callERaWriteHandler(pin, param);
    }

    root = nullptr;
    item = nullptr;
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::handlePinRequest(const ERaDataBuff& arrayTopic, const char* payload) {
    cJSON* root = cJSON_Parse(payload);
    if (!cJSON_IsObject(root)) {
        cJSON_Delete(root);
        root = nullptr;
        return;
    }

    ERaParam param;
    PinConfig_t pin {};
    cJSON* current = nullptr;

    for (current = root->child; current != nullptr && current->string != nullptr; current = current->next) {
        if (this->getGPIOPin(current, "virtual_pin", pin.pin)) {
            if (cJSON_IsNumber(current)) {
                param = current->valuedouble;
            }
            else if (cJSON_IsString(current)) {
                param.add_static(current->valuestring);
            }
            this->callERaWriteHandler(pin.pin, param);
            continue;
        }
        if (this->getGPIOPin(current, "pin_mode", pin.pin)) {
            if (!cJSON_IsString(current)) {
                continue;
            }
            ERA_CHECK_PIN(pin.pin);

            this->getReportConfig(root, pin);

            if (ERaStrCmp(current->valuestring, "output")) {
                pinMode(pin.pin, OUTPUT);
            }
            else if (ERaStrCmp(current->valuestring, "open_drain")) {
                pinMode(pin.pin, OUTPUT_OPEN_DRAIN);
            }
            else if (ERaStrCmp(current->valuestring, "pwm")) {
                pinMode(pin.pin, OUTPUT);
                this->ERaPinRp.setPinReport(pin.pin, PWM, nullptr,
                                            pin.report.interval, pin.report.minInterval,
                                            pin.report.maxInterval, pin.report.reportableChange,
                                            this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "input")) {
                pinMode(pin.pin, INPUT);
                this->ERaPinRp.setPinReport(pin.pin, INPUT, digitalReadParticle,
                                            pin.report.interval, pin.report.minInterval,
                                            pin.report.maxInterval, pin.report.reportableChange,
                                            this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "pullup")) {
                pinMode(pin.pin, INPUT_PULLUP);
                this->ERaPinRp.setPinReport(pin.pin, INPUT_PULLUP, digitalReadParticle,
                                            pin.report.interval, pin.report.minInterval,
                                            pin.report.maxInterval, pin.report.reportableChange,
                                            this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "pulldown")) {
                pinMode(pin.pin, INPUT_PULLDOWN);
                this->ERaPinRp.setPinReport(pin.pin, INPUT_PULLDOWN, digitalReadParticle,
                                            pin.report.interval, pin.report.minInterval,
                                            pin.report.maxInterval, pin.report.reportableChange,
                                            this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "analog")) {
                pinMode(pin.pin, ANALOG);
                this->ERaPinRp.setPinReport(pin.pin, ANALOG, analogReadParticle,
                                            pin.report.interval, pin.report.minInterval,
                                            pin.report.maxInterval, pin.report.reportableChange,
                                            this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "remove")) {
                this->ERaPinRp.deleteWithPin(pin.pin);
            }
            continue;
        }
        if (this->getGPIOPin(current, "digital_pin", pin.pin)) {
            ERA_CHECK_PIN(pin.pin);
            ERaParam param(current->valueint);
            pinMode(pin.pin, OUTPUT);
            if (current->valueint == TOGGLE) {
                ::digitalWrite(pin.pin, ((digitalReadParticle(pin.pin) == LOW) ? HIGH : LOW));
            }
            else {
                ::digitalWrite(pin.pin, current->valueint ? HIGH : LOW);
            }
            this->digitalWrite(pin.pin, digitalReadParticle(pin.pin));
            this->callERaPinWriteHandler(pin.pin, param, param);
            continue;
        }
        if (this->getGPIOPin(current, "pwm_pin", pin.pin)) {
            ERA_CHECK_PIN(pin.pin);
            ::analogWrite(pin.pin, current->valueint);
            continue;
        }
    }

    cJSON_Delete(root);
    root = nullptr;
    ERA_FORCE_UNUSED(arrayTopic);
}

#endif /* INC_ERA_API_PARTICLE_HPP_ */
