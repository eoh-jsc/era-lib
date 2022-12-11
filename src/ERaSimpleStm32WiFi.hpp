#ifndef INC_ERA_SIMPLE_STM32_WIFI_HPP_
#define INC_ERA_SIMPLE_STM32_WIFI_HPP_

#include <ERa/ERaTask.hpp>
#include <ERa/ERaApiStm32Def.hpp>
#include <Adapters/ERaWiFiClient.hpp>
#include <ERa/ERaApiStm32.hpp>
#include <Modbus/ERaModbusStm32.hpp>
#include <Utility/ERaFlashStm32.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
    #if !defined(ERA_NO_RTOS)
        ERaOs::osThreadNew(runERaLoopTask, "eraTask",
                            1024 * 8, NULL, configMAX_PRIORITIES - 4);
        this->_modbusTask = ERaOs::osThreadNew(this->modbusTask, "modbusTask",
                            1024 * 5, this, configMAX_PRIORITIES - 3);
        this->_writeModbusTask = ERaOs::osThreadNew(this->writeModbusTask, "writeModbusTask",
                            1024 * 5, this, configMAX_PRIORITIES - 3);
    #endif
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(true);
        ERaOs::osThreadDelete(NULL);
    #endif
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(false);
        ERaOs::osThreadDelete(NULL);
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
    cJSON_AddStringToObject(root, INFO_SSID, ((this->thisProto().transp.getSSID() == nullptr) ?
                                            ERA_PROTO_TYPE : this->thisProto().transp.getSSID()));
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
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ((this->thisProto().transp.getSSID() == nullptr) ?
                                                    ERA_PROTO_TYPE : this->thisProto().transp.getSSID()));
}

static ERaFlash flash;
static ERaMqtt<TinyGsmClient, MQTTClient> mqtt;
ERaWiFi< ERaMqtt<TinyGsmClient, MQTTClient> > ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_STM32_WIFI_HPP_ */
