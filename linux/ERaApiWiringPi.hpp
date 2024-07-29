#ifndef INC_ERA_API_WIRING_PI_HPP_
#define INC_ERA_API_WIRING_PI_HPP_

#include <ERa/ERaApi.hpp>
#include <ERa/ERaApiConfig.hpp>

#undef ERA_DECODE_PIN_NUMBER
#define ERA_DECODE_PIN_NUMBER(pin)  ((pin <= MAX_GPIO_WIRING_PI) ?              \
                                    pin : ERA_INVALID_PIN)

#undef ERA_DECODE_PIN_NAME
#define ERA_DECODE_PIN_NAME(pin)    (((pin[0] == 'a') || (pin[0] == 'A')) ?     \
                                    ERA_DECODE_PIN(atoi(pin + 1)) : ERA_DECODE_PIN_NUMBER(atoi(pin)))

inline
static int digitalReadPi(uint8_t pin) {
    return digitalRead(pin);
}

inline
static int analogReadPi(uint8_t pin) {
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
                pinModePi(pin.pin, INPUT);
                switch (pin.pinMode) {
                    case INPUT:
                        pullUpDnControl(pin.pin, PUD_OFF);
                        break;
                    case INPUT_PULLUP:
                        pullUpDnControl(pin.pin, PUD_UP);
                        break;
                    case INPUT_PULLDOWN:
                        pullUpDnControl(pin.pin, PUD_DOWN);
                        break;
                    default:
                        break;
                }
                ERA_SET_PIN_REPORT(pin.pinMode, digitalReadPi);
            }
            else if (ERaStrCmp(item->valuestring, "integer")) {
                pin.report = PinConfig_t::__ReportConfig_t(1000, 1000, 60000, 10.0f);
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
                ERA_SET_PIN_REPORT(ANALOG, analogReadPi);
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
                pinModePi(pin.pin, pin.pinMode);
                if (pin.pinMode == OUTPUT) {
                    ERA_SET_PIN_REPORT(pin.pinMode, digitalReadPi);
                }
            }
            else if (ERaStrCmp(item->valuestring, "integer")) {
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
                pwmModePi(pin.pin);
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
                ::pwmWritePi(pin, value);
                if (rp != nullptr) {
                    rp->updateReport(value, true);
                }
                break;
            case ANALOG:
                ::analogWrite(pin, value);
                break;
            case RAW_PIN:
                break;
            case VIRTUAL:
            case ERA_VIRTUAL:
                this->callERaWriteHandler(pin, param);
                break;
            default:
                pinModePi(pin, OUTPUT);
                if (value == TOGGLE) {
                    ::digitalWrite(pin, ((digitalReadPi(pin) == LOW) ? HIGH : LOW));
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
                    pinModePi(pin.pin, OUTPUT);
                }
                else if (ERaStrCmp(current->valuestring, "open_drain")) {
                    pinModePi(pin.pin, OUTPUT);
                }
                else if (ERaStrCmp(current->valuestring, "pwm")) {
                    pwmModePi(pin.pin);
                    ERA_SET_DEBUG_PIN_REPORT(PWM, nullptr);
                }
                else if (ERaStrCmp(current->valuestring, "input")) {
                    pinModePi(pin.pin, INPUT);
                    pullUpDnControl(pin.pin, PUD_OFF);
                    ERA_SET_DEBUG_PIN_REPORT(INPUT, digitalReadPi);
                }
                else if (ERaStrCmp(current->valuestring, "pullup")) {
                    pinModePi(pin.pin, INPUT);
                    pullUpDnControl(pin.pin, PUD_UP);
                    ERA_SET_DEBUG_PIN_REPORT(INPUT_PULLUP, digitalReadPi);
                }
                else if (ERaStrCmp(current->valuestring, "pulldown")) {
                    pinModePi(pin.pin, INPUT);
                    pullUpDnControl(pin.pin, PUD_DOWN);
                    ERA_SET_DEBUG_PIN_REPORT(INPUT_PULLDOWN, digitalReadPi);
                }
                else if (ERaStrCmp(current->valuestring, "analog")) {
                    ERA_SET_DEBUG_PIN_REPORT(ANALOG, analogReadPi);
                }
                else if (ERaStrCmp(current->valuestring, "remove")) {
                    this->ERaPinRp.deleteWithPin(pin.pin);
                }
                continue;
            }
            if (this->getGPIOPin(current, "digital_pin", pin.pin)) {
                ERA_CHECK_PIN(pin.pin);
                ERaParam param(current->valueint);
                pinModePi(pin.pin, OUTPUT);
                if (current->valueint == TOGGLE) {
                    ::digitalWrite(pin.pin, ((digitalReadPi(pin.pin) == LOW) ? HIGH : LOW));
                }
                else {
                    ::digitalWrite(pin.pin, current->valueint ? HIGH : LOW);
                }
                this->digitalWrite(pin.pin, digitalReadPi(pin.pin));
                this->callERaPinWriteHandler(pin.pin, param, param);
                continue;
            }
            if (this->getGPIOPin(current, "pwm_pin", pin.pin)) {
                ERA_CHECK_PIN(pin.pin);
                ::pwmWritePi(pin.pin, current->valueint);
                continue;
            }
        }

        cJSON_Delete(root);
        root = nullptr;
        ERA_FORCE_UNUSED(arrayTopic);
    }
#endif

#endif /* INC_ERA_API_WIRING_PI_HPP_ */
