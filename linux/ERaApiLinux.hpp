#ifndef INC_ERA_API_LINUX_HPP_
#define INC_ERA_API_LINUX_HPP_

#include <ERa/ERaApi.hpp>

#if defined(__has_include)
    #if __has_include("ERaInternalIO.hpp")
        #include <ERa/ERaApiConfig.hpp>
        #include "ERaInternalIO.hpp"
        #define ERA_HAS_INTERNAL_IO_H

        #undef LOW
        #define LOW                         0x00
        #undef HIGH
        #define HIGH                        0x01

        #undef ERA_DECODE_PIN_NAME
        #define ERA_DECODE_PIN_NAME(pin)    (((pin[0] == 'a') || (pin[0] == 'A')) ?     \
                                            ERA_DECODE_PIN(atoi(pin + 1)) : ERA_DECODE_PIN_NUMBER(atoi(pin)))
    #endif
#endif

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
#if defined(ERA_HAS_INTERNAL_IO_H)
            if (this->skipPinReport) {
                this->ERaPinRp.setPinRaw(pin.pin, pin.configId);
            }
            else if (ERaStrCmp(item->valuestring, "boolean")) {
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
                pinModeLinux(pin.pin, pin.pinMode);
                ERA_SET_PIN_REPORT(pin.pinMode, digitalReadLinux);
            }
            else if (ERaStrCmp(item->valuestring, "integer")) {
                pin.report = PinConfig_t::__ReportConfig_t(ERA_SETTING_REPORT_PIN);
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
                pinModeLinux(pin.pin, ANALOG);
                ERA_SET_PIN_REPORT(ANALOG, analogReadLinux);
            }
#else
            if (ERaStrCmp(item->valuestring, "boolean") ||
                ERaStrCmp(item->valuestring, "integer")) {
                this->ERaPinRp.setPinRaw(pin.pin, pin.configId);
            }
#endif
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
#if defined(ERA_HAS_INTERNAL_IO_H)
            if (this->skipPinReport) {
                this->ERaPinRp.setPinRaw(pin.pin, pin.configId);
            }
            else if (ERaStrCmp(item->valuestring, "boolean")) {
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
                pinModeLinux(pin.pin, pin.pinMode);
                ERA_SET_PIN_REPORT(pin.pinMode, digitalReadLinux);
            }
            else if (ERaStrCmp(item->valuestring, "integer")) {
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
                pinModeLinux(pin.pin, PWM);
                ERA_SET_PIN_REPORT(PWM, nullptr);
            }
#else
            if (ERaStrCmp(item->valuestring, "boolean") ||
                ERaStrCmp(item->valuestring, "integer")) {
                this->ERaPinRp.setPinRaw(pin.pin, pin.configId);
            }
#endif
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
#if defined(ERA_HAS_INTERNAL_IO_H)
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
                ::pwmWriteLinux(pin, value);
                if (rp != nullptr) {
                    rp->updateReport(value, true);
                }
                break;
            case ANALOG:
                ::analogWriteLinux(pin, value);
                break;
            case OUTPUT:
            case OUTPUT_OPEN_DRAIN:
                if (value == TOGGLE) {
                    ::digitalWriteLinux(pin, ((digitalReadLinux(pin) == LOW) ? HIGH : LOW));
                }
                else {
                    ::digitalWriteLinux(pin, value ? HIGH : LOW);
                }
                break;
            case RAW_PIN:
            default:
                break;
        }
#endif
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
                    pinModeLinux(pin.pin, OUTPUT);
                }
                else if (ERaStrCmp(current->valuestring, "open_drain")) {
                    pinModeLinux(pin.pin, OUTPUT);
                }
                else if (ERaStrCmp(current->valuestring, "pwm")) {
                    pwmModeLinux(pin.pin);
                    ERA_SET_DEBUG_PIN_REPORT(PWM, nullptr);
                }
                else if (ERaStrCmp(current->valuestring, "input")) {
                    pinModeLinux(pin.pin, INPUT);
                    ERA_SET_DEBUG_PIN_REPORT(INPUT, digitalReadLinux);
                }
                else if (ERaStrCmp(current->valuestring, "pullup")) {
                    pinModeLinux(pin.pin, INPUT_PULLUP);
                    ERA_SET_DEBUG_PIN_REPORT(INPUT_PULLUP, digitalReadLinux);
                }
                else if (ERaStrCmp(current->valuestring, "pulldown")) {
                    pinModeLinux(pin.pin, INPUT_PULLDOWN);
                    ERA_SET_DEBUG_PIN_REPORT(INPUT_PULLDOWN, digitalReadLinux);
                }
                else if (ERaStrCmp(current->valuestring, "analog")) {
                    ERA_SET_DEBUG_PIN_REPORT(ANALOG, analogReadLinux);
                }
                else if (ERaStrCmp(current->valuestring, "remove")) {
                    this->ERaPinRp.deleteWithPin(pin.pin);
                }
                continue;
            }
            if (this->getGPIOPin(current, "digital_pin", pin.pin)) {
                ERA_CHECK_PIN(pin.pin);
                ERaParam param(current->valueint);
                pinModeLinux(pin.pin, OUTPUT);
                if (current->valueint == TOGGLE) {
                    ::digitalWriteLinux(pin.pin, ((digitalReadLinux(pin.pin) == LOW) ? HIGH : LOW));
                }
                else {
                    ::digitalWriteLinux(pin.pin, current->valueint ? HIGH : LOW);
                }
                this->digitalWrite(pin.pin, digitalReadLinux(pin.pin));
                this->callERaPinWriteHandler(pin.pin, param, param);
                continue;
            }
            if (this->getGPIOPin(current, "pwm_pin", pin.pin)) {
                ERA_CHECK_PIN(pin.pin);
                ::pwmWriteLinux(pin.pin, current->valueint);
            }
        }

        cJSON_Delete(root);
        root = nullptr;
        ERA_FORCE_UNUSED(arrayTopic);
    }
#endif

#endif /* INC_ERA_API_LINUX_HPP_ */
