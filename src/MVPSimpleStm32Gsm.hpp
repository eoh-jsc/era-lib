#ifndef INC_MVP_SIMPLE_STM32_GSM_HPP_
#define INC_MVP_SIMPLE_STM32_GSM_HPP_

#include <Adapters/MVPGsmClient.hpp>
#include <STM32FreeRTOS.h>

#define MVP_MODEL_TYPE                "MVP"
#define MVP_BOARD_TYPE                "STM32"

#if defined(MVP_MODBUS)
    template <class Api>
    void MVPModbus<Api>::initModbusTask() {
    }

    template <class Api>
    void MVPModbus<Api>::modbusTask(void* args) {
    }

    template <class Api>
    void MVPModbus<Api>::writeModbusTask(void* args) {
    }
#endif

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::initMVPTask() {
#if defined(MVP_MODBUS)
	Base::MVPModbus::begin();
#endif
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::runMVPTask() {
#if defined(MVP_MODBUS)
	Base::MVPModbus::runRead();
	Base::MVPModbus::runWrite();
#endif
}

template <class Proto, class Flash>
void MVPApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, "board", MVP_BOARD_TYPE);
    cJSON_AddStringToObject(root, "model", MVP_MODEL_TYPE);
	cJSON_AddStringToObject(root, "auth_token", static_cast<Proto*>(this)->MVP_AUTH);
    cJSON_AddStringToObject(root, "firmware_version", MVP_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, "ssid", "GPRS");
    cJSON_AddStringToObject(root, "bssid", "GPRS");
    cJSON_AddNumberToObject(root, "rssi", 100);
    cJSON_AddStringToObject(root, "mac", "GPRS");
    cJSON_AddStringToObject(root, "ip", "GPRS");
}

template <class Proto, class Flash>
void MVPApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, "chip_temperature", 5000);
	cJSON_AddNumberToObject(root, "temperature", 0);
	cJSON_AddNumberToObject(root, "voltage", 999);
	cJSON_AddNumberToObject(root, "is_battery", 0);
	cJSON_AddNumberToObject(root, "rssi", 100);
	cJSON_AddStringToObject(root, "wifi_is_using", "GPRS");
}

static MVPMqtt<TinyGsmClient, MQTTClient> mqtt;
static MVPFlash flash;
MVPGsm mvp(mqtt, flash);

#endif /* INC_MVP_SIMPLE_STM32_GSM_HPP_ */