#ifndef INC_ERA_SIMPLE_STM32_GSM_HPP_
#define INC_ERA_SIMPLE_STM32_GSM_HPP_

#include <ERa/ERaTask.hpp>
#include <ERa/ERaApiStm32Def.hpp>
#include <Adapters/ERaGsmClient.hpp>
#include <ERa/ERaApiStm32.hpp>
#include <Modbus/ERaModbusStm32.hpp>
#include <Utility/ERaFlashStm32.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
    #if !defined(ERA_NO_RTOS)
        xTaskCreate(runERaLoopTask, "eraTask", 1024 * 8, NULL,
                    configMAX_PRIORITIES - 4, NULL);
        xTaskCreate(this->modbusTask, "modbusTask", 1024 * 5, this,
                    configMAX_PRIORITIES - 3, (::TaskHandle_t*)(&this->_modbusTask));
        xTaskCreate(this->writeModbusTask, "writeModbusTask", 1024 * 5, this,
                    configMAX_PRIORITIES - 3, (::TaskHandle_t*)(&this->_writeModbusTask));
    #endif
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            vTaskDelete(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(true);
        vTaskDelete(NULL);
    #endif
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            vTaskDelete(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(false);
        vTaskDelete(NULL);
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
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().ERA_AUTH);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, ERA_PROTO_TYPE);
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_PROTO_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddStringToObject(root, INFO_MAC, ERA_PROTO_TYPE);
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ERA_PROTO_TYPE);
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().transp.getPing());
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ERA_PROTO_TYPE);
}

static ERaMqtt<TinyGsmClient, MQTTClient> mqtt;
static ERaFlash flash;
ERaGsm ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_STM32_GSM_HPP_ */
