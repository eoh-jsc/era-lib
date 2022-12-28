#ifndef INC_ERA_SIMPLE_ESP32_SSL_ETHERNET_HPP_
#define INC_ERA_SIMPLE_ESP32_SSL_ETHERNET_HPP_

#define ERA_MQTT_SSL

#include <WiFiClientSecure.h>
#include <Adapters/ERaEthernetEsp32Client.hpp>
#include <MQTT/ERaMqttSecure.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
        this->_modbusTask = ERaOs::osThreadNew(this->modbusTask, "modbusTask",
                                            1024 * 5, this, configMAX_PRIORITIES - 9, ERA_MCU_CORE);
        this->_writeModbusTask = ERaOs::osThreadNew(this->writeModbusTask, "writeModbusTask",
                                            1024 * 5, this, configMAX_PRIORITIES - 9, ERA_MCU_CORE);
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
                                            1024 * 12, this, configMAX_PRIORITIES - 7, ERA_MCU_CORE);
        this->_controlZigbeeTask = ERaOs::osThreadNew(this->controlZigbeeTask, "controlZigbeeTask",
                                            1024 * 12, this, configMAX_PRIORITIES - 7, ERA_MCU_CORE);
        this->_responseZigbeeTask = ERaOs::osThreadNew(this->responseZigbeeTask, "responseZigbeeTask",
                                            1024 * 12, this, configMAX_PRIORITIES - 8, ERA_MCU_CORE);
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

static ERaFlash flash;
static WiFiClientSecure ERaWiFiClient;
static ERaMqttSecure<WiFiClientSecure, MQTTClient> mqtt(ERaWiFiClient);
ERaEthernet< ERaMqttSecure<WiFiClientSecure, MQTTClient> > ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_ESP32_SSL_ETHERNET_HPP_ */
