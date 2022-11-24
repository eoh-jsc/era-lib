#ifndef INC_ERA_SIMPLE_STM32_WIFI_HPP_
#define INC_ERA_SIMPLE_STM32_WIFI_HPP_

#include <STM32FreeRTOS.h>
#include <Adapters/ERaWiFiClient.hpp>
#include <Modbus/ERaModbusStm32.hpp>
#include <Utility/ERaFlashStm32.hpp>

#define ERA_MODEL_TYPE                "ERa"
#define ERA_BOARD_TYPE                "STM32"

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
    #if !defined(ERA_NO_RTOS)
        xTaskCreate(this->modbusTask, "modbusTask", 1024 * 5, this, 17, (::TaskHandle_t*)(&this->_modbusTask));
        xTaskCreate(this->writeModbusTask, "writeModbusTask", 1024 * 5, this, 17, (::TaskHandle_t*)(&this->_writeModbusTask));
        vTaskStartScheduler();
    #endif
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(true);
    #endif
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(false);
    #endif
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::initERaTask() {
#if defined(ERA_MODBUS)
	Base::Modbus::begin();
#endif
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::runERaTask() {
#if defined(ERA_MODBUS) &&  \
    defined(ERA_NO_RTOS)
	Base::Modbus::runRead();
	Base::Modbus::runWrite();
#endif
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, static_cast<Proto*>(this)->ERA_AUTH);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, "WiFi");
    cJSON_AddStringToObject(root, INFO_BSSID, "WiFi");
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddStringToObject(root, INFO_MAC, "WiFi");
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, "WiFi");
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, "WiFi");
}

static ERaMqtt<TinyGsmClient, MQTTClient> mqtt;
static ERaFlash flash;
ERaWiFi ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_STM32_WIFI_HPP_ */
