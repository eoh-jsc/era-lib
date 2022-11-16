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
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(true);
    }

    template <class Api>
    void* ERaModbus<Api>::writeModbusTask(void* args) {
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(false);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Api>
    void ERaZigbee<Api>::initZigbeeTask() {

    }

    template <class Api>
    void* ERaZigbee<Api>::zigbeeTask(void* args) {

    }

    template <class Api>
    void* ERaZigbee<Api>::controlZigbeeTask(void* args) {
        
    }

    template <class Api>
    void* ERaZigbee<Api>::responseZigbeeTask(void* args) {
        
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::initERaTask() {
#if defined(ERA_MODBUS)
	Base::ERaModbus::begin();
#endif
#if defined(ERA_ZIGBEE)
	Base::ERaZigbee::begin();
#endif
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::runERaTask() {
}

static ERaMqttLinux<MQTTLinuxClient> mqtt;
static ERaFlashLinux flash;
ERaLinux ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_WIRING_PI_HPP_ */
