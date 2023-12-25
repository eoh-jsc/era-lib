#ifndef INC_ERA_TASK_LOGICROM_HPP_
#define INC_ERA_TASK_LOGICROM_HPP_

#include <ERa/ERaApi.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
    #if !defined(ERA_NO_RTOS)
        this->_modbusTask = new int;
        this->_writeModbusTask = new int;
        *((int*)(this->_modbusTask)) = os_task_create(this->modbusTask, "modbusTask", this, FALSE);
        *((int*)(this->_writeModbusTask)) = os_task_create(this->writeModbusTask, "writeModbusTask", this, FALSE);
    #endif
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        ERaModbus* modbus = (ERaModbus*)args;
        if (modbus == NULL) {
            os_task_delete(*(int*)(modbus->_modbusTask));
        }
        modbus->runRead();
        os_task_delete(*(int*)(modbus->_modbusTask));
    #endif
        ERA_FORCE_UNUSED(args);
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        ERaModbus* modbus = (ERaModbus*)args;
        if (modbus == NULL) {
            os_task_delete(*(int*)(modbus->_writeModbusTask));
        }
        modbus->runWrite();
        os_task_delete(*(int*)(modbus->_writeModbusTask));
    #endif
        ERA_FORCE_UNUSED(args);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Api>
    void ERaZigbee<Api>::initZigbeeTask() {
    #if !defined(ERA_NO_RTOS)
        this->_zigbeeTask = new int;
        this->_responseZigbeeTask = new int;
        this->_controlZigbeeTask = new int;
        *((int*)(this->_zigbeeTask)) = os_task_create(this->zigbeeTask, "zigbeeTask", this, FALSE);
        *((int*)(this->_responseZigbeeTask)) = os_task_create(this->responseZigbeeTask, "responseZigbeeTask", this, FALSE);
        *((int*)(this->_controlZigbeeTask)) = os_task_create(this->controlZigbeeTask, "controlZigbeeTask", this, FALSE);
    #endif
    }

    template <class Api>
    void ERaZigbee<Api>::zigbeeTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        ERaZigbee* zigbee = (ERaZigbee*)args;
        if (zigbee == NULL) {
            os_task_delete(*(int*)(zigbee->_zigbeeTask));
        }
        zigbee->runEvent();
        os_task_delete(*(int*)(zigbee->_zigbeeTask));
    #endif
        ERA_FORCE_UNUSED(args);
    }

    template <class Api>
    void ERaZigbee<Api>::responseZigbeeTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        ERaZigbee* zigbee = (ERaZigbee*)args;
        if (zigbee == NULL) {
            os_task_delete(*(int*)(zigbee->_responseZigbeeTask));
        }
        zigbee->runResponse();
        os_task_delete(*(int*)(zigbee->_responseZigbeeTask));
    #endif
        ERA_FORCE_UNUSED(args);
    }

    template <class Api>
    void ERaZigbee<Api>::controlZigbeeTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        ERaZigbee* zigbee = (ERaZigbee*)args;
        if (zigbee == NULL) {
            os_task_delete(*(int*)(zigbee->_controlZigbeeTask));
        }
        zigbee->runControl();
        os_task_delete(*(int*)(zigbee->_controlZigbeeTask));
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
        os_task_delete(*(int*)(api->_apiTask));
    }
    api->runAPI();
    os_task_delete(*(int*)(api->_apiTask));
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
        this->_apiTask = new int;
        *((int*)(this->_apiTask)) = os_task_create(this->apiTask, "apiTask", this, FALSE);
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

#endif /* INC_ERA_TASK_LOGICROM_HPP_ */
