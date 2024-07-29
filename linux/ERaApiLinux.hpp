#ifndef INC_ERA_API_LINUX_HPP_
#define INC_ERA_API_LINUX_HPP_

#include <ERa/ERaApi.hpp>

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
            if (ERaStrCmp(item->valuestring, "boolean") ||
                ERaStrCmp(item->valuestring, "integer")) {
                this->ERaPinRp.setPinRaw(pin.pin, pin.configId);
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
            if (ERaStrCmp(item->valuestring, "boolean") ||
                ERaStrCmp(item->valuestring, "integer")) {
                this->ERaPinRp.setPinRaw(pin.pin, pin.configId);
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
        param.add(item->valuedouble);
        int pMode = this->ERaPinRp.findPinMode(pin);
        switch (pMode) {
            case VIRTUAL:
            case ERA_VIRTUAL:
                this->callERaWriteHandler(pin, param);
                break;
            default:
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
            }
        }

        cJSON_Delete(root);
        root = nullptr;
        ERA_FORCE_UNUSED(arrayTopic);
    }
#endif

#endif /* INC_ERA_API_LINUX_HPP_ */
