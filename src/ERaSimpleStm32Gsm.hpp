#ifndef INC_ERA_SIMPLE_STM32_GSM_HPP_
#define INC_ERA_SIMPLE_STM32_GSM_HPP_

#include <STM32FreeRTOS.h>
#include <Adapters/ERaGsmClient.hpp>
#include <Modbus/ERaModbusStm32.hpp>
#include <Utility/ERaFlashStm32.hpp>

#define ERA_MODEL_TYPE                "ERa"
#define ERA_BOARD_TYPE                "STM32"

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
        xTaskCreate(this->modbusTask, "modbusTask", 1024 * 5, this, 17, (::TaskHandle_t*)(&this->_modbusTask));
        xTaskCreate(this->writeModbusTask, "writeModbusTask", 1024 * 5, this, 17, (::TaskHandle_t*)(&this->_writeModbusTask));
        vTaskStartScheduler();
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(true);
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(false);
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::initERaTask() {
#if defined(ERA_MODBUS)
	Base::ERaModbus::begin();
#endif
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::runERaTask() {
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, static_cast<Proto*>(this)->ERA_AUTH);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, "GPRS");
    cJSON_AddStringToObject(root, INFO_BSSID, "GPRS");
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddStringToObject(root, INFO_MAC, "GPRS");
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, "GPRS");
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, "GPRS");
}

static ERaMqtt<TinyGsmClient, MQTTClient> mqtt;
static ERaFlash flash;
ERaGsm ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_STM32_GSM_HPP_ */
