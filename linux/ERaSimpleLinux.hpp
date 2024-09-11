#ifndef INC_ERA_SIMPLE_LINUX_HPP_
#define INC_ERA_SIMPLE_LINUX_HPP_

#include <pthread.h>

#if defined(WIRING_PI)
    #include <wiringPi.h>
    #include <Utility/ERaPwmWiringPi.hpp>
    #include <Utility/ERaPulseWiringPi.hpp>
    #include <ERaApiWiringPiDef.hpp>
#elif defined(LINUX)
    #include <ERaApiLinuxDef.hpp>
#endif

#include <Network/ERaUDPLinux.hpp>
#include <Network/ERaNetworkInfo.hpp>
#include <Network/ERaSocketLinux.hpp>
#include <Utility/ERaSerialLinux.hpp>
#include <ERaLinuxClient.hpp>
#include <Utility/ERaStreamLinux.hpp>
#include <Modbus/ERaModbusLinux.hpp>
#include <Zigbee/ERaZigbeeLinux.hpp>

#if defined(WIRING_PI)
    #include <Utility/ERaI2CWiringPi.hpp>
    #include <ERaApiWiringPi.hpp>
#elif defined(LINUX)
    #include <ERaApiLinux.hpp>
#endif

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
    #if !defined(ERA_NO_RTOS)
        this->_modbusTask = new pthread_t;
        this->_writeModbusTask = new pthread_t;
        pthread_create((pthread_t*)(this->_modbusTask), NULL, this->modbusTask, this);
        pthread_create((pthread_t*)(this->_writeModbusTask), NULL, this->writeModbusTask, this);
    #endif
    }

    template <class Api>
    void* ERaModbus<Api>::modbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            pthread_exit(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->runRead();
        pthread_exit(NULL);
    #endif
        ERA_FORCE_UNUSED(args);
    }

    template <class Api>
    void* ERaModbus<Api>::writeModbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            pthread_exit(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->runWrite();
        pthread_exit(NULL);
    #endif
        ERA_FORCE_UNUSED(args);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Api>
    void ERaZigbee<Api>::initZigbeeTask() {
    #if !defined(ERA_NO_RTOS)
        this->_zigbeeTask = new pthread_t;
        this->_responseZigbeeTask = new pthread_t;
        this->_controlZigbeeTask = new pthread_t;
        pthread_create((pthread_t*)(this->_zigbeeTask), NULL, this->zigbeeTask, this);
        pthread_create((pthread_t*)(this->_responseZigbeeTask), NULL, this->responseZigbeeTask, this);
        pthread_create((pthread_t*)(this->_controlZigbeeTask), NULL, this->controlZigbeeTask, this);
    #endif
    }

    template <class Api>
    void* ERaZigbee<Api>::zigbeeTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            pthread_exit(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runEvent();
        pthread_exit(NULL);
    #endif
        ERA_FORCE_UNUSED(args);
    }

    template <class Api>
    void* ERaZigbee<Api>::responseZigbeeTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            pthread_exit(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runResponse();
        pthread_exit(NULL);
    #endif
        ERA_FORCE_UNUSED(args);
    }

    template <class Api>
    void* ERaZigbee<Api>::controlZigbeeTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            pthread_exit(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runControl();
        pthread_exit(NULL);
    #endif
        ERA_FORCE_UNUSED(args);
    }
#endif

template <class Proto, class Flash>
inline
void* ERaApi<Proto, Flash>::apiTask(void* args) {
#if !defined(ERA_NO_RTOS)
    if (args == NULL) {
        pthread_exit(NULL);
    }
    ERaApi* api = (ERaApi*)args;
    api->runAPI();
    delete api->_apiTask;
    api->_apiTask = NULL;
    pthread_exit(NULL);
#endif
    ERA_FORCE_UNUSED(args);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::initApiTask() {
#if !defined(ERA_NO_RTOS)
    if ((this->taskSize > 0) && (this->_apiTask == NULL)) {
        this->_apiTask = new pthread_t;
        pthread_create((pthread_t*)(this->_apiTask), NULL, this->apiTask, this);
    }
#endif
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::initERaApiTask() {
    this->initApiTask();
#if defined(ERA_MODBUS)
    Modbus::begin();
#endif
#if defined(ERA_ZIGBEE)
    Zigbee::begin();
#endif
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::runERaApiTask() {
#if defined(ERA_MODBUS) &&  \
    defined(ERA_NO_RTOS)
    Modbus::run();
#endif
#if defined(ERA_ZIGBEE) &&  \
    defined(ERA_NO_RTOS)
    Zigbee::run();
#endif
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlashLinux flash;
    static ERaMqttLinux<MQTTLinuxClient> mqtt;
    ERaLinux< ERaMqttLinux<MQTTLinuxClient> > ERa(mqtt, flash);
#else
    extern ERaLinux< ERaMqttLinux<MQTTLinuxClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_LINUX_HPP_ */
