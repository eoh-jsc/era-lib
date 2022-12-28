#ifndef INC_ERA_SIMPLE_LINUX_HPP_
#define INC_ERA_SIMPLE_LINUX_HPP_

#if defined(LINUX) && defined(RASPBERRY)
    #include <wiringPi.h>
    #include <ERaApiWiringPiDef.hpp>
#elif defined(LINUX)
    #include <ERaApiLinuxDef.hpp>
#endif

#include <UtilityLinux/ERaSocketLinux.hpp>
#include <UtilityLinux/ERaSerialLinux.hpp>
#include <ERaLinuxClient.hpp>
#include <ModbusLinux/ERaModbusLinux.hpp>
#include <ZigbeeLinux/ERaZigbeeLinux.hpp>

#if defined(LINUX) && defined(RASPBERRY)
    #include <ERaApiWiringPi.hpp>
#elif defined(LINUX)
    #include <ERaApiLinux.hpp>
#endif

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
        pthread_exit(NULL);
    }

    template <class Api>
    void* ERaModbus<Api>::writeModbusTask(void* args) {
        if (args == NULL) {
            pthread_exit(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(false);
        pthread_exit(NULL);
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
        pthread_exit(NULL);
    }

    template <class Api>
    void* ERaZigbee<Api>::controlZigbeeTask(void* args) {
        if (args == NULL) {
            pthread_exit(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runControl();
        pthread_exit(NULL);
    }

    template <class Api>
    void* ERaZigbee<Api>::responseZigbeeTask(void* args) {
        if (args == NULL) {
            pthread_exit(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runResponse();
        pthread_exit(NULL);
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

static ERaFlashLinux flash;
static ERaMqttLinux<MQTTLinuxClient> mqtt;
ERaLinux< ERaMqttLinux<MQTTLinuxClient> > ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_LINUX_HPP_ */
