#ifndef INC_ERA_SIMPLE_ESP32_ETHERNET_HPP_
#define INC_ERA_SIMPLE_ESP32_ETHERNET_HPP_

#include <Adapters/ERaEthernetEsp32Client.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
        xTaskCreatePinnedToCore(this->modbusTask, "modbusTask", 1024 * 5, this, 17, &this->_modbusTask, ERA_MCU_CORE);
        xTaskCreatePinnedToCore(this->writeModbusTask, "writeModbusTask", 1024 * 5, this, 17, &this->_writeModbusTask, ERA_MCU_CORE);
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
        if (args == NULL) {
            vTaskDelete(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(true);
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
        if (args == NULL) {
            vTaskDelete(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(false);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Api>
    void ERaZigbee<Api>::initZigbeeTask() {
        xTaskCreatePinnedToCore(this->zigbeeTask, "zigbeeTask", 1024 * 10, this, 20, &this->_zigbeeTask, ERA_MCU_CORE);
        xTaskCreatePinnedToCore(this->controlZigbeeTask, "controlZigbeeTask", 1024 * 10, this, 20, &this->_controlZigbeeTask, ERA_MCU_CORE);
        xTaskCreatePinnedToCore(this->responseZigbeeTask, "responseZigbeeTask", 1024 * 10, this, 19, &this->_responseZigbeeTask, ERA_MCU_CORE);
    }

    template <class Api>
    void ERaZigbee<Api>::zigbeeTask(void* args) {
        if (args == NULL) {
            vTaskDelete(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->run();
    }

    template <class Api>
    void ERaZigbee<Api>::controlZigbeeTask(void* args) {
        if (args == NULL) {
            vTaskDelete(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runControl();
    }

    template <class Api>
    void ERaZigbee<Api>::responseZigbeeTask(void* args) {
        if (args == NULL) {
            vTaskDelete(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runResponse();
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

static WiFiClient eraWiFiClient;
static ERaMqtt<WiFiClient, MQTTClient> mqtt(eraWiFiClient);
static ERaFlash flash;
ERaEthernet ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_ESP32_ETHERNET_HPP_ */
