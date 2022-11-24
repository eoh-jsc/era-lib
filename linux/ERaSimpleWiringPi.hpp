#ifndef INC_ERA_SIMPLE_WIRING_PI_HPP_
#define INC_ERA_SIMPLE_WIRING_PI_HPP_

#include <ERaLinuxClient.hpp>
#include <ModbusPi/ERaModbusWiringPi.hpp>
#include <ZigbeePi/ERaZigbeeWiringPi.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
        this->_modbusTask = new pthread_t;
        this->_writeModbusTask = new pthread_t;
        pthread_create((pthread_t*)(this->_modbusTask), NULL, this->modbusTask, this);
        pthread_create((pthread_t*)(this->_writeModbusTask), NULL, this->writeModbusTask, this);
    }

    template <class Api>
    void* ERaModbus<Api>::modbusTask(void* args) {
        if (args == NULL) {
            pthread_exit(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(true);
    }

    template <class Api>
    void* ERaModbus<Api>::writeModbusTask(void* args) {
        if (args == NULL) {
            pthread_exit(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(false);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Api>
    void ERaZigbee<Api>::initZigbeeTask() {
        this->_zigbeeTask = new pthread_t;
        this->_controlZigbeeTask = new pthread_t;
        this->_responseZigbeeTask = new pthread_t;
        pthread_create((pthread_t*)(this->_zigbeeTask), NULL, this->zigbeeTask, this);
        pthread_create((pthread_t*)(this->_controlZigbeeTask), NULL, this->controlZigbeeTask, this);
        pthread_create((pthread_t*)(this->_responseZigbeeTask), NULL, this->responseZigbeeTask, this);
    }

    template <class Api>
    void* ERaZigbee<Api>::zigbeeTask(void* args) {
        if (args == NULL) {
            pthread_exit(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->run();
    }

    template <class Api>
    void* ERaZigbee<Api>::controlZigbeeTask(void* args) {
        if (args == NULL) {
            pthread_exit(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runControl();
    }

    template <class Api>
    void* ERaZigbee<Api>::responseZigbeeTask(void* args) {
        if (args == NULL) {
            pthread_exit(NULL);
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

static ERaMqttLinux<MQTTLinuxClient> mqtt;
static ERaFlashLinux flash;
ERaLinux ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_WIRING_PI_HPP_ */
