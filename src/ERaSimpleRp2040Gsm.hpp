#ifndef INC_ERA_SIMPLE_RP2040_GSM_HPP_
#define INC_ERA_SIMPLE_RP2040_GSM_HPP_

#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaGsmClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Zigbee/ERaZigbeeArduino.hpp>
#include <Utility/ERaFlashLittleFS.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
        xTaskCreate(this->modbusTask, "modbusTask", 1024 * 5, this,
                    configMAX_PRIORITIES - 3, (::TaskHandle_t*)(&this->_modbusTask));
        xTaskCreate(this->writeModbusTask, "writeModbusTask", 1024 * 5, this,
                    configMAX_PRIORITIES - 3, (::TaskHandle_t*)(&this->_writeModbusTask));
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
        if (args == NULL) {
            vTaskDelete(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(true);
        vTaskDelete(NULL);
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
        if (args == NULL) {
            vTaskDelete(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(false);
        vTaskDelete(NULL);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Api>
    void ERaZigbee<Api>::initZigbeeTask() {
        xTaskCreate(this->zigbeeTask, "zigbeeTask", 1024 * 12, this,
                    configMAX_PRIORITIES - 1, (::TaskHandle_t*)(&this->_zigbeeTask));
        xTaskCreate(this->controlZigbeeTask, "controlZigbeeTask", 1024 * 12, this,
                    configMAX_PRIORITIES - 1, (::TaskHandle_t*)(&this->_controlZigbeeTask));
        xTaskCreate(this->responseZigbeeTask, "responseZigbeeTask", 1024 * 12, this,
                    configMAX_PRIORITIES - 2, (::TaskHandle_t*)(&this->_responseZigbeeTask));
    }

    template <class Api>
    void ERaZigbee<Api>::zigbeeTask(void* args) {
        if (args == NULL) {
            vTaskDelete(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->run();
        vTaskDelete(NULL);
    }

    template <class Api>
    void ERaZigbee<Api>::controlZigbeeTask(void* args) {
        if (args == NULL) {
            vTaskDelete(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runControl();
        vTaskDelete(NULL);
    }

    template <class Api>
    void ERaZigbee<Api>::responseZigbeeTask(void* args) {
        if (args == NULL) {
            vTaskDelete(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runResponse();
        vTaskDelete(NULL);
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::initERaTask() {
#if defined(ERA_MODBUS)
	Base::Modbus::begin();
#endif
#if defined(ERA_ZIGBEE)
	Base::Zigbee::begin();
#endif
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::runERaTask() {
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

#endif /* INC_ERA_SIMPLE_RP2040_GSM_HPP_ */
