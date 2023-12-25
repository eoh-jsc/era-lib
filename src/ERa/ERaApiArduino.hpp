#ifndef INC_ERA_API_ARDUINO_HPP_
#define INC_ERA_API_ARDUINO_HPP_

#include <ERa/ERaApi.hpp>

#if defined(ARDUINO_ARCH_STM32)

    #include <string>

    #undef ERA_DECODE_PIN_NUMBER
    #if defined(NUM_DIGITAL_PINS)
        #define ERA_DECODE_PIN_NUMBER(pin)  ((pin < NUM_DIGITAL_PINS) ?                 \
                                            pinNametoDigitalPin((PinName)pin) : ERA_INVALID_PIN)
    #elif defined(STM_VALID_PINNAME)
        #define ERA_DECODE_PIN_NUMBER(pin)  (STM_VALID_PINNAME((PinName)pin) ?          \
                                            pinNametoDigitalPin((PinName)pin) : ERA_INVALID_PIN)
    #else
        #define ERA_DECODE_PIN_NUMBER(pin)  pinNametoDigitalPin((PinName)pin)
    #endif

    #undef ERA_DECODE_PIN_NAME
    #define ERA_DECODE_PIN_NAME(pin)        ((stringToPinName(pin) != PinName::NC) ?    \
                                            pinNametoDigitalPin(stringToPinName(pin)) : \
                                            (((pin[0] == 'a') || (pin[0] == 'A')) ?     \
                                            ERA_DECODE_PIN(atoi(pin + 1)) : ERA_DECODE_PIN_NUMBER(atoi(pin))))

    inline
    static PinName stringToPinName(std::string str) {
        if (str.size() < 3) {
            return PinName::NC;
        }

        PinName p = PinName::NC;
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
            return std::tolower(c);
        });

        if (str.at(0) != 'p' || !isdigit(str.at(2))) {
            return PinName::NC;
        }

        switch (str.at(1)) {
            case 'a':
                p = (PinName)(PortName::PortA + atoi(&str.at(2)));
                break;
            case 'b':
                p = (PinName)(PortName::PortB + atoi(&str.at(2)));
                break;
    #if defined(GPIOC_BASE)
            case 'c':
                p = (PinName)(PortName::PortC + atoi(&str.at(2)));
                break;
    #endif
    #if defined(GPIOD_BASE)
            case 'd':
                p = (PinName)(PortName::PortD + atoi(&str.at(2)));
                break;
    #endif
    #if defined(GPIOE_BASE)
            case 'e':
                p = (PinName)(PortName::PortE + atoi(&str.at(2)));
                break;
    #endif
    #if defined(GPIOF_BASE)
            case 'f':
                p = (PinName)(PortName::PortF + atoi(&str.at(2)));
                break;
    #endif
    #if defined(GPIOG_BASE)
            case 'g':
                p = (PinName)(PortName::PortG + atoi(&str.at(2)));
                break;
    #endif
    #if defined(GPIOH_BASE)
            case 'h':
                p = (PinName)(PortName::PortH + atoi(&str.at(2)));
                break;
    #endif
    #if defined(GPIOI_BASE)
            case 'i':
                p = (PinName)(PortName::PortI + atoi(&str.at(2)));
                break;
    #endif
    #if defined(GPIOJ_BASE)
            case 'j':
                p = (PinName)(PortName::PortJ + atoi(&str.at(2)));
                break;
    #endif
    #if defined(GPIOK_BASE)
            case 'k':
                p = (PinName)(PortName::PortK + atoi(&str.at(2)));
                break;
    #endif
    #if defined(GPIOZ_BASE)
            case 'z':
                p = (PinName)(PortName::PortZ + atoi(&str.at(2)));
                break;
    #endif
            default:
                return PinName::NC;
        }

        return p;
    }

#endif

inline
static int digitalReadArduino(uint8_t pin) {
    return digitalRead(pin);
}

inline
static int analogReadArduino(uint8_t pin) {
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
                pinMode(pin.pin, pin.pinMode);
                this->ERaPinRp.setPinReport(pin.pin, pin.pinMode, digitalReadArduino,
                                            pin.report.interval, pin.report.minInterval,
                                            pin.report.maxInterval, pin.report.reportableChange,
                                            this->reportPinConfigCb, pin.configId);
            }
            else if (ERaStrCmp(item->valuestring, "integer")) {
                pin.report = PinConfig_t::__ReportConfig_t(1000, 1000, 60000, 10.0f);
                this->getPinConfig(current, pin);
                this->getScaleConfig(current, pin);
#if defined(ESP32)
                pinMode(pin.pin, ANALOG);
#elif defined(ARDUINO_ARCH_STM32)
                if (!digitalpinIsAnalogInput(pin.pin)) {
                    continue;
                }
                pin.pin = digitalPinToAnalogInput(pin.pin) + PNUM_ANALOG_BASE;
                pinMode(pin.pin, ANALOG);
#endif
                this->ERaPinRp.setPinReport(pin.pin, ANALOG, analogReadArduino,
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
                pinMode(pin.pin, pin.pinMode);
                this->ERaPinRp.setPinReport(pin.pin, pin.pinMode, digitalReadArduino,
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
#if !defined(ERA_IGNORE_ANALOG_WRITE)
                ::analogWrite(pin, value);
#endif
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
                    ::digitalWrite(pin, ((digitalReadArduino(pin) == LOW) ? HIGH : LOW));
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
                this->ERaPinRp.setPinReport(pin.pin, INPUT, digitalReadArduino,
                                            pin.report.interval, pin.report.minInterval,
                                            pin.report.maxInterval, pin.report.reportableChange,
                                            this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "pullup")) {
                pinMode(pin.pin, INPUT_PULLUP);
                this->ERaPinRp.setPinReport(pin.pin, INPUT_PULLUP, digitalReadArduino,
                                            pin.report.interval, pin.report.minInterval,
                                            pin.report.maxInterval, pin.report.reportableChange,
                                            this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "pulldown")) {
                pinMode(pin.pin, INPUT_PULLDOWN);
                this->ERaPinRp.setPinReport(pin.pin, INPUT_PULLDOWN, digitalReadArduino,
                                            pin.report.interval, pin.report.minInterval,
                                            pin.report.maxInterval, pin.report.reportableChange,
                                            this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "analog")) {
#if defined(ESP32)
                pinMode(pin.pin, ANALOG);
#elif defined(ARDUINO_ARCH_STM32)
                if (!digitalpinIsAnalogInput(pin.pin)) {
                    continue;
                }
                pin.pin = digitalPinToAnalogInput(pin.pin) + PNUM_ANALOG_BASE;
                pinMode(pin.pin, ANALOG);
#endif
                this->ERaPinRp.setPinReport(pin.pin, ANALOG, analogReadArduino,
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
                ::digitalWrite(pin.pin, ((digitalReadArduino(pin.pin) == LOW) ? HIGH : LOW));
            }
            else {
                ::digitalWrite(pin.pin, current->valueint ? HIGH : LOW);
            }
            this->digitalWrite(pin.pin, digitalReadArduino(pin.pin));
            this->callERaPinWriteHandler(pin.pin, param, param);
            continue;
        }
        if (this->getGPIOPin(current, "pwm_pin", pin.pin)) {
            ERA_CHECK_PIN(pin.pin);
#if !defined(ERA_IGNORE_ANALOG_WRITE)
            ::analogWrite(pin.pin, current->valueint);
#endif
            continue;
        }
    }

    cJSON_Delete(root);
    root = nullptr;
    ERA_FORCE_UNUSED(arrayTopic);
}

#endif /* INC_ERA_API_ARDUINO_HPP_ */
