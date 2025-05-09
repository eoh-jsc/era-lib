#ifndef INC_ERA_API_ESP32_HPP_
#define INC_ERA_API_ESP32_HPP_

#include <ERa/ERaApi.hpp>
#include <ERa/ERaApiConfig.hpp>

#ifndef SOC_ADC_MAX_CHANNEL_NUM
    #define SOC_ADC_MAX_CHANNEL_NUM         (10)
#endif

inline
static int digitalReadEsp32(uint16_t pin) {
    return digitalRead(pin);
}

inline
static int analogReadEsp32(uint16_t pin) {
    return analogRead(pin);
}

#if (ESP_IDF_VERSION_MAJOR > 4)
    inline
    static int ledcReadEsp32(uint16_t pin) {
        return ledcRead(pin);
    }
#else
    inline
    static int ledcReadEsp32(uint8_t channel) {
        return ledcRead(channel);
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
                pinMode(pin.pin, pin.pinMode);
                ERA_SET_PIN_REPORT(pin.pinMode, digitalReadEsp32);
            }
            else if (ERaStrCmp(item->valuestring, "integer")) {
                pin.report = PinConfig_t::__ReportConfig_t(ERA_SETTING_REPORT_PIN);
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
                uint8_t adcUnit = (digitalPinToAnalogChannel(pin.pin) / SOC_ADC_MAX_CHANNEL_NUM);
                if (adcUnit == 0) {
                    pinMode(pin.pin, ANALOG);
                    ERA_SET_PIN_REPORT(ANALOG, analogReadEsp32);
                }
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
                pinMode(pin.pin, pin.pinMode);
                ERA_SET_PIN_REPORT(pin.pinMode, digitalReadEsp32);
            }
            else if (ERaStrCmp(item->valuestring, "integer")) {
                pin.pwm.channel = this->ERaPinRp.findChannelFree();
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
                if (pin.pwm.channel >= 0) {
#if (ESP_IDF_VERSION_MAJOR > 4)
                    ledcDetach(pin.pin);
                    ledcAttach(pin.pin, pin.pwm.frequency, pin.pwm.resolution);
                    ERA_SET_PIN_REPORT(PWM, ledcReadEsp32);
#else
                    ledcDetachPin(pin.pin);
                    ledcAttachPin(pin.pin, pin.pwm.channel);
                    ledcSetup(pin.pwm.channel, pin.pwm.frequency, pin.pwm.resolution);
                    ERA_SET_PWM_REPORT(PWM, ledcReadEsp32);
#endif
                }
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
        int8_t channel {0};
        param.add(item->valuedouble);
        float value = item->valuedouble;
        int pMode = this->ERaPinRp.findPinMode(pin);
        const ERaReport::ScaleData_t* scale = this->ERaPinRp.findScale(pin);
        if ((scale != nullptr) && scale->enable) {
            value = ERaMapNumberRange((float)item->valuedouble, scale->min, scale->max, scale->rawMin, scale->rawMax);
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
#if (ESP_IDF_VERSION_MAJOR > 4)
                ledcWrite(pin, value);
                ERA_FORCE_UNUSED(channel);
#else
                channel = this->ERaPinRp.findChannelPWM(pin);
                if (channel >= 0) {
                    ledcWrite(channel, value);
                }
#endif
                break;
            case OUTPUT:
            case OUTPUT_OPEN_DRAIN:
                if (value == TOGGLE) {
                    ::digitalWrite(pin, ((digitalReadEsp32(pin) == LOW) ? HIGH : LOW));
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
    #if (ESP_IDF_VERSION_MAJOR > 4)
                    ledcDetach(pin.pin);
                    ledcAttach(pin.pin, pin.pwm.frequency, pin.pwm.resolution);
                    ERA_SET_DEBUG_PIN_REPORT(PWM, ledcReadEsp32);
    #else
                    ledcDetachPin(pin.pin);
                    ledcAttachPin(pin.pin, pin.pwm.channel);
                    ledcSetup(pin.pwm.channel, pin.pwm.frequency, pin.pwm.resolution);
                    ERA_SET_DEBUG_PWM_REPORT(PWM, ledcReadEsp32);
    #endif
                }
                else if (ERaStrCmp(current->valuestring, "input")) {
                    pinMode(pin.pin, INPUT);
                    ERA_SET_DEBUG_PIN_REPORT(INPUT, digitalReadEsp32);
                }
                else if (ERaStrCmp(current->valuestring, "pullup")) {
                    pinMode(pin.pin, INPUT_PULLUP);
                    ERA_SET_DEBUG_PIN_REPORT(INPUT_PULLUP, digitalReadEsp32);
                }
                else if (ERaStrCmp(current->valuestring, "pulldown")) {
                    pinMode(pin.pin, INPUT_PULLDOWN);
                    ERA_SET_DEBUG_PIN_REPORT(INPUT_PULLDOWN, digitalReadEsp32);
                }
                else if (ERaStrCmp(current->valuestring, "analog")) {
                    uint8_t adcUnit = (digitalPinToAnalogChannel(pin.pin) / SOC_ADC_MAX_CHANNEL_NUM);
                    if (adcUnit == 0) {
                        pinMode(pin.pin, ANALOG);
                        ERA_SET_DEBUG_PIN_REPORT(ANALOG, analogReadEsp32);
                    }
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
                    ::digitalWrite(pin.pin, ((digitalReadEsp32(pin.pin) == LOW) ? HIGH : LOW));
                }
                else {
                    ::digitalWrite(pin.pin, current->valueint ? HIGH : LOW);
                }
                this->digitalWrite(pin.pin, digitalReadEsp32(pin.pin));
                this->callERaPinWriteHandler(pin.pin, param, param);
                continue;
            }
            if (this->getGPIOPin(current, "pwm_pin", pin.pin)) {
                ERA_CHECK_PIN(pin.pin);
    #if (ESP_IDF_VERSION_MAJOR > 4)
                ledcWrite(pin.pin, current->valueint);
    #else
                pin.pwm.channel = this->ERaPinRp.findChannelPWM(pin.pin);
                if (pin.pwm.channel >= 0) {
                    ledcWrite(pin.pwm.channel, current->valueint);
                }
    #endif
            }
        }

        cJSON_Delete(root);
        root = nullptr;
        ERA_FORCE_UNUSED(arrayTopic);
    }
#endif

#endif /* INC_ERA_API_ESP32_HPP_ */
