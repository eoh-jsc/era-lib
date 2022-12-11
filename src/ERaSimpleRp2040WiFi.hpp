#ifndef INC_ERA_SIMPLE_RP2040_WIFI_HPP_
#define INC_ERA_SIMPLE_RP2040_WIFI_HPP_

#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaWiFiClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Zigbee/ERaZigbeeArduino.hpp>
#include <Utility/ERaFlashLittleFS.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
        this->_modbusTask = ERaOs::osThreadNew(this->modbusTask, "modbusTask",
                                            1024 * 5, this, configMAX_PRIORITIES - 3);
        this->_writeModbusTask = ERaOs::osThreadNew(this->writeModbusTask, "writeModbusTask",
                                            1024 * 5, this, configMAX_PRIORITIES - 3);
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(true);
        ERaOs::osThreadDelete(NULL);
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(false);
        ERaOs::osThreadDelete(NULL);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Api>
    void ERaZigbee<Api>::initZigbeeTask() {
        this->_zigbeeTask = ERaOs::osThreadNew(this->zigbeeTask, "zigbeeTask",
                                            1024 * 12, this, configMAX_PRIORITIES - 1);
        this->_controlZigbeeTask = ERaOs::osThreadNew(this->controlZigbeeTask, "controlZigbeeTask",
                                            1024 * 12, this, configMAX_PRIORITIES - 1);
        this->_responseZigbeeTask = ERaOs::osThreadNew(this->responseZigbeeTask, "responseZigbeeTask",
                                            1024 * 12, this, configMAX_PRIORITIES - 2);
    }

    template <class Api>
    void ERaZigbee<Api>::zigbeeTask(void* args) {
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->run();
        ERaOs::osThreadDelete(NULL);
    }

    template <class Api>
    void ERaZigbee<Api>::controlZigbeeTask(void* args) {
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runControl();
        ERaOs::osThreadDelete(NULL);
    }

    template <class Api>
    void ERaZigbee<Api>::responseZigbeeTask(void* args) {
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runResponse();
        ERaOs::osThreadDelete(NULL);
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

#endif /* INC_ERA_SIMPLE_RP2040_WIFI_HPP_ */
