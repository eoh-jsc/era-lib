#ifndef INC_ERA_SIMPLE_MBED_WIFI_HPP_
#define INC_ERA_SIMPLE_MBED_WIFI_HPP_

#include <rtos.h>
#include <ERa/ERaApiMbedDef.hpp>
#include <Adapters/ERaWiFiClient.hpp>
#include <ERa/ERaApiMbed.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Zigbee/ERaZigbeeArduino.hpp>
#include <Utility/ERaFlashMbed.hpp>

using namespace rtos;

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
#if !defined(ERA_NO_RTOS)
        if (this->_modbusTask == NULL) {
            this->_modbusTask = new Thread(osPriority::osPriorityNormal, 5 * 1024);
        }
        ((Thread*)this->_modbusTask)->start(callback(this->modbusTask, this));
        if (this->_writeModbusTask == NULL) {
            this->_writeModbusTask = new Thread(osPriority::osPriorityNormal, 5 * 1024);
        }
        ((Thread*)this->_writeModbusTask)->start(callback(this->writeModbusTask, this));
#endif
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
#if !defined(ERA_NO_RTOS)
        ERaModbus* modbus = (ERaModbus*)args;
        if (modbus == NULL) {
            ((Thread*)modbus->_modbusTask)->terminate();
        }
        modbus->run(true);
        ((Thread*)modbus->_modbusTask)->terminate();
#endif
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
#if !defined(ERA_NO_RTOS)
        ERaModbus* modbus = (ERaModbus*)args;
        if (modbus == NULL) {
            ((Thread*)modbus->_writeModbusTask)->terminate();
        }
        modbus->run(false);
        ((Thread*)modbus->_writeModbusTask)->terminate();
#endif
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Api>
    void ERaZigbee<Api>::initZigbeeTask() {
#if !defined(ERA_NO_RTOS)
        if (this->_zigbeeTask == NULL) {
            this->_zigbeeTask = new Thread(osPriority::osPriorityHigh, 12 * 1024);
        }
        ((Thread*)this->_zigbeeTask)->start(callback(this->zigbeeTask, this));
        if (this->_controlZigbeeTask == NULL) {
            this->_controlZigbeeTask = new Thread(osPriority::osPriorityHigh, 12 * 1024);
        }
        ((Thread*)this->_controlZigbeeTask)->start(callback(this->controlZigbeeTask, this));
        if (this->_responseZigbeeTask == NULL) {
            this->_responseZigbeeTask = new Thread(osPriority::osPriorityHigh, 12 * 1024);
        }
        ((Thread*)this->_responseZigbeeTask)->start(callback(this->responseZigbeeTask, this));
#endif
    }

    template <class Api>
    void ERaZigbee<Api>::zigbeeTask(void* args) {
#if !defined(ERA_NO_RTOS)
        ERaZigbee* zigbee = (ERaZigbee*)args;
        if (zigbee == NULL) {
            ((Thread*)zigbee->_zigbeeTask)->terminate();
        }
        zigbee->run();
        ((Thread*)zigbee->_zigbeeTask)->terminate();
#endif
    }

    template <class Api>
    void ERaZigbee<Api>::controlZigbeeTask(void* args) {
#if !defined(ERA_NO_RTOS)
        ERaZigbee* zigbee = (ERaZigbee*)args;
        if (zigbee == NULL) {
            ((Thread*)zigbee->_controlZigbeeTask)->terminate();
        }
        zigbee->runControl();
        ((Thread*)zigbee->_controlZigbeeTask)->terminate();
#endif
    }

    template <class Api>
    void ERaZigbee<Api>::responseZigbeeTask(void* args) {
#if !defined(ERA_NO_RTOS)
        ERaZigbee* zigbee = (ERaZigbee*)args;
        if (zigbee == NULL) {
            ((Thread*)zigbee->_responseZigbeeTask)->terminate();
        }
        zigbee->runResponse();
        ((Thread*)zigbee->_responseZigbeeTask)->terminate();
#endif
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::initERaTask() {
#if defined(ERA_MODBUS)
	Base::Modbus::begin();
#endif
#if defined(ERA_ZIGBEE) &&  \
    !defined(ERA_NO_RTOS)
	Base::Zigbee::begin();
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

#endif /* INC_ERA_SIMPLE_MBED_WIFI_HPP_ */
