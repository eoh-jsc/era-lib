#ifndef INC_ERA_API_MBED_HPP_
#define INC_ERA_API_MBED_HPP_

#include <mbed.h>
#include <ERa/ERaApi.hpp>
#include <ERa/ERaApiConfig.hpp>

using namespace mbed;

#if defined(ARDUINO)

    inline
    static int analogReadMbed(uint16_t pin) {
        return analogRead(pin);
    }

    inline
    static void pwmWrite(uint16_t pin, uint8_t value) {
#if !defined(ERA_IGNORE_ANALOG_WRITE)
        analogWrite(pin, value);
#else
        ERA_FORCE_UNUSED(pin);
        ERA_FORCE_UNUSED(value);
#endif
    }

    inline
    static int digitalReadMbed(uint16_t pin) {
        return digitalRead(pin);
    }

    inline
    static int digitalOutRead(uint16_t pin) {
        return digitalRead(pin);
    }

#else

    inline
    static int analogReadMbed(uint16_t pin) {
        AnalogIn p((PinName)pin);
        return int(p.read() * 4095);
    }

    inline
    static void pwmWrite(uint16_t pin, uint8_t value) {
        PwmOut p((PinName)pin);
        p.write(value / 100.0f);
    }

    inline
    static int digitalReadMbed(uint16_t pin) {
        DigitalIn p((PinName)pin);
        return int(p);
    }

    inline
    static int digitalOutRead(uint16_t pin) {
        DigitalOut p((PinName)pin);
        return int(p);
    }

    inline
    static void digitalWrite(uint16_t pin, int value) {
        DigitalOut p((PinName)pin);
        p = value;
    }

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
            if (this->skipPinReport) {
                this->ERaPinRp.setPinRaw(pin.pin, pin.configId);
            }
            else if (ERaStrCmp(item->valuestring, "boolean")) {
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
#if defined(ARDUINO)
                pinMode(pin.pin, pin.pinMode);
#endif
                ERA_SET_PIN_REPORT(pin.pinMode, digitalReadMbed);
            }
            else if (ERaStrCmp(item->valuestring, "integer")) {
                pin.report = PinConfig_t::__ReportConfig_t(ERA_SETTING_REPORT_PIN);
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
                ERA_SET_PIN_REPORT(ANALOG, analogReadMbed);
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
#if defined(ARDUINO)
                pinMode(pin.pin, pin.pinMode);
#endif
                ERA_SET_PIN_REPORT(pin.pinMode, digitalOutRead);
            }
            else if (ERaStrCmp(item->valuestring, "integer")) {
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
#if defined(ARDUINO)
                pinMode(pin.pin, OUTPUT);
#else
                if (pin.pinMode == PWM) {
                    PwmOut p((PinName)pin.pin);
                    p.period(1.0f / pin.pwm.frequency);
                    p.write(0);
                }
#endif
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
            case V_OUTPUT_OPEN_DRAIN:
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
                ::pwmWrite(pin, value);
                if (rp != nullptr) {
                    rp->updateReport(value, true);
                }
                break;
            case OUTPUT:
            case V_OUTPUT_OPEN_DRAIN:
                if (value == TOGGLE) {
                    ::digitalWrite(pin, ((digitalOutRead(pin) == LOW) ? HIGH : LOW));
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
    #if defined(ARDUINO)
                    pinMode(pin.pin, OUTPUT);
    #else
                    DigitalOut p((PinName)pin.pin);
    #endif
                }
                else if (ERaStrCmp(current->valuestring, "open_drain")) {
    #if defined(ARDUINO)
                    pinMode(pin.pin, OUTPUT_OPEN_DRAIN);
    #else
                    DigitalOut p((PinName)pin.pin);
    #endif
                }
                else if (ERaStrCmp(current->valuestring, "pwm")) {
    #if defined(ARDUINO)
                    pinMode(pin.pin, OUTPUT);
    #else
                    PwmOut p((PinName)pin.pin);
                    p.period(1.0f / pin.pwm.frequency);
                    p.write(0);
    #endif
                    ERA_SET_DEBUG_PIN_REPORT(PWM, nullptr);
                }
                else if (ERaStrCmp(current->valuestring, "input")) {
    #if defined(ARDUINO)
                    pinMode(pin.pin, INPUT);
    #endif
                    ERA_SET_DEBUG_PIN_REPORT(INPUT, digitalReadMbed);
                }
                else if (ERaStrCmp(current->valuestring, "pullup")) {
    #if defined(ARDUINO)
                    pinMode(pin.pin, INPUT_PULLUP);
    #endif
                    ERA_SET_DEBUG_PIN_REPORT(INPUT_PULLUP, digitalReadMbed);
                }
                else if (ERaStrCmp(current->valuestring, "pulldown")) {
    #if defined(ARDUINO)
                    pinMode(pin.pin, INPUT_PULLDOWN);
    #endif
                    ERA_SET_DEBUG_PIN_REPORT(INPUT_PULLDOWN, digitalReadMbed);
                }
                else if (ERaStrCmp(current->valuestring, "analog")) {
                    ERA_SET_DEBUG_PIN_REPORT(ANALOG, analogReadMbed);
                }
                else if (ERaStrCmp(current->valuestring, "remove")) {
                    this->ERaPinRp.deleteWithPin(pin.pin);
                }
                continue;
            }
            if (this->getGPIOPin(current, "digital_pin", pin.pin)) {
                ERA_CHECK_PIN(pin.pin);
                ERaParam param(current->valueint);
                if (current->valueint == TOGGLE) {
                    ::digitalWrite(pin.pin, ((digitalOutRead(pin.pin) == LOW) ? HIGH : LOW));
                }
                else {
                    ::digitalWrite(pin.pin, current->valueint ? HIGH : LOW);
                }
                this->digitalWrite(pin.pin, digitalOutRead(pin.pin));
                this->callERaPinWriteHandler(pin.pin, param, param);
                continue;
            }
            if (this->getGPIOPin(current, "pwm_pin", pin.pin)) {
                ERA_CHECK_PIN(pin.pin);
                ::pwmWrite(pin.pin, current->valueint);
            }
        }

        cJSON_Delete(root);
        root = nullptr;
        ERA_FORCE_UNUSED(arrayTopic);
    }
#endif

#endif /* INC_ERA_API_MBED_HPP_ */
