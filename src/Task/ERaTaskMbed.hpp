#ifndef INC_ERA_TASK_MBED_HPP_
#define INC_ERA_TASK_MBED_HPP_

#include <mbed.h>
#include <ERa/ERaApi.hpp>

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
        modbus->runRead();
        ((Thread*)modbus->_modbusTask)->terminate();
    #endif
        ERA_FORCE_UNUSED(args);
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        ERaModbus* modbus = (ERaModbus*)args;
        if (modbus == NULL) {
            ((Thread*)modbus->_writeModbusTask)->terminate();
        }
        modbus->runWrite();
        ((Thread*)modbus->_writeModbusTask)->terminate();
    #endif
        ERA_FORCE_UNUSED(args);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Api>
    void ERaZigbee<Api>::initZigbeeTask() {
    #if !defined(ERA_NO_RTOS)
        if (this->_zigbeeTask == NULL) {
            this->_zigbeeTask = new Thread(osPriority::osPriorityHigh1, 12 * 1024);
        }
        ((Thread*)this->_zigbeeTask)->start(callback(this->zigbeeTask, this));
        if (this->_responseZigbeeTask == NULL) {
            this->_responseZigbeeTask = new Thread(osPriority::osPriorityHigh1, 12 * 1024);
        }
        ((Thread*)this->_responseZigbeeTask)->start(callback(this->responseZigbeeTask, this));
        if (this->_controlZigbeeTask == NULL) {
            this->_controlZigbeeTask = new Thread(osPriority::osPriorityHigh, 12 * 1024);
        }
        ((Thread*)this->_controlZigbeeTask)->start(callback(this->controlZigbeeTask, this));
    #endif
    }

    template <class Api>
    void ERaZigbee<Api>::zigbeeTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        ERaZigbee* zigbee = (ERaZigbee*)args;
        if (zigbee == NULL) {
            ((Thread*)zigbee->_zigbeeTask)->terminate();
        }
        zigbee->runEvent();
        ((Thread*)zigbee->_zigbeeTask)->terminate();
    #endif
        ERA_FORCE_UNUSED(args);
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
        ERA_FORCE_UNUSED(args);
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
        ERA_FORCE_UNUSED(args);
    }
#endif

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::apiTask(void* args) {
#if !defined(ERA_NO_RTOS)
    ERaApi* api = (ERaApi*)args;
    if (api == NULL) {
        ((Thread*)api->_apiTask)->terminate();
    }
    api->runAPI();
    ((Thread*)api->_apiTask)->terminate();
    ((Thread*)api->_apiTask)->~Thread();
    delete api->_apiTask;
    api->_apiTask = NULL;
#endif
    ERA_FORCE_UNUSED(args);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::initApiTask() {
#if !defined(ERA_NO_RTOS)
    if ((this->taskSize > 0) && (this->_apiTask == NULL)) {
        if (this->_apiTask == NULL) {
            this->_apiTask = new Thread(osPriority::osPriorityNormal, this->taskSize);
        }
        ((Thread*)this->_apiTask)->start(callback(this->apiTask, this));
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

#endif /* INC_ERA_TASK_MBED_HPP_ */
