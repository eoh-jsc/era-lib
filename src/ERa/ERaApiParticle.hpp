#ifndef INC_ERA_API_PARTICLE_HPP_
#define INC_ERA_API_PARTICLE_HPP_

#include "application.h"
#include <ERa/ERaApi.hpp>
#include <ERa/ERaApiConfig.hpp>

inline
static int digitalReadParticle(uint16_t pin) {
    return digitalRead(pin);
}

inline
static int analogReadParticle(uint16_t pin) {
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
                this->getScaleConfig(current, pin);
                pinMode(pin.pin, (PinMode)pin.pinMode);
                ERA_SET_PIN_REPORT(pin.pinMode, digitalReadParticle);
            }
            else if (ERaStrCmp(item->valuestring, "integer")) {
                pin.report = PinConfig_t::__ReportConfig_t(ERA_SETTING_REPORT_PIN);
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
                pinMode(pin.pin, ANALOG);
                ERA_SET_PIN_REPORT(ANALOG, analogReadParticle);
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
                this->getScaleConfig(current, pin);
                pinMode(pin.pin, (PinMode)pin.pinMode);
                ERA_SET_PIN_REPORT(pin.pinMode, digitalReadParticle);
            }
            else if (ERaStrCmp(item->valuestring, "integer")) {
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
                pinMode(pin.pin, OUTPUT);
                ERA_SET_PIN_REPORT(PWM, nullptr);
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
    uint16_t pin = ERA_DECODE_PIN_NAME(str);
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
            case PWM:
            case OUTPUT:
            case OUTPUT_OPEN_DRAIN:
                raw = value;
                if (this->callERaPinWriteHandler(pin, param, raw) ||
                    this->skipPinWrite) {
                    pMode = RAW_PIN;
                }
                break;
            default:
                break;
        }
        switch (pMode) {
            case PWM:
                ::analogWrite(pin, value);
                if (rp != nullptr) {
                    rp->updateReport(value, true);
                }
                break;
            case OUTPUT:
            case OUTPUT_OPEN_DRAIN:
                if (value == TOGGLE) {
                    ::digitalWrite(pin, ((digitalReadParticle(pin) == LOW) ? HIGH : LOW));
                }
                else {
                    ::digitalWrite(pin, value ? HIGH : LOW);
                }
                break;
            case RAW_PIN:
            default:
                break;
        }
    }

    root = nullptr;
    item = nullptr;
}

#if defined(ERA_PIN_DEBUG)
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
                    ERA_SET_DEBUG_PIN_REPORT(PWM, nullptr);
                }
                else if (ERaStrCmp(current->valuestring, "input")) {
                    pinMode(pin.pin, INPUT);
                    ERA_SET_DEBUG_PIN_REPORT(INPUT, digitalReadParticle);
                }
                else if (ERaStrCmp(current->valuestring, "pullup")) {
                    pinMode(pin.pin, INPUT_PULLUP);
                    ERA_SET_DEBUG_PIN_REPORT(INPUT_PULLUP, digitalReadParticle);
                }
                else if (ERaStrCmp(current->valuestring, "pulldown")) {
                    pinMode(pin.pin, INPUT_PULLDOWN);
                    ERA_SET_DEBUG_PIN_REPORT(INPUT_PULLDOWN, digitalReadParticle);
                }
                else if (ERaStrCmp(current->valuestring, "analog")) {
                    pinMode(pin.pin, ANALOG);
                    ERA_SET_DEBUG_PIN_REPORT(ANALOG, analogReadParticle);
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
            }
        }

        cJSON_Delete(root);
        root = nullptr;
        ERA_FORCE_UNUSED(arrayTopic);
    }
#endif

#endif /* INC_ERA_API_PARTICLE_HPP_ */
