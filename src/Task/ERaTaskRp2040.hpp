#ifndef INC_ERA_TASK_RP2040_HPP_
#define INC_ERA_TASK_RP2040_HPP_

#include <ERa/ERaApi.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
    #if !defined(ERA_NO_RTOS)
        this->_modbusTask = ERaOs::osThreadNew(this->modbusTask, "modbusTask",
                                            1024 * 5, this, configMAX_PRIORITIES - 3);
        this->_writeModbusTask = ERaOs::osThreadNew(this->writeModbusTask, "writeModbusTask",
                                            1024 * 5, this, configMAX_PRIORITIES - 3);
    #endif
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->runRead();
        ERaOs::osThreadDelete(NULL);
    #endif
        ERA_FORCE_UNUSED(args);
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->runWrite();
        ERaOs::osThreadDelete(NULL);
    #endif
        ERA_FORCE_UNUSED(args);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Api>
    void ERaZigbee<Api>::initZigbeeTask() {
    #if !defined(ERA_NO_RTOS)
        this->_zigbeeTask = ERaOs::osThreadNew(this->zigbeeTask, "zigbeeTask",
                                            1024 * 12, this, configMAX_PRIORITIES - 1);
        this->_responseZigbeeTask = ERaOs::osThreadNew(this->responseZigbeeTask, "responseZigbeeTask",
                                            1024 * 12, this, configMAX_PRIORITIES - 1);
        this->_controlZigbeeTask = ERaOs::osThreadNew(this->controlZigbeeTask, "controlZigbeeTask",
                                            1024 * 12, this, configMAX_PRIORITIES - 2);
    #endif
    }

    template <class Api>
    void ERaZigbee<Api>::zigbeeTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runEvent();
        ERaOs::osThreadDelete(NULL);
    #endif
        ERA_FORCE_UNUSED(args);
    }

    template <class Api>
    void ERaZigbee<Api>::responseZigbeeTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runResponse();
        ERaOs::osThreadDelete(NULL);
    #endif
        ERA_FORCE_UNUSED(args);
    }

    template <class Api>
    void ERaZigbee<Api>::controlZigbeeTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaZigbee* zigbee = (ERaZigbee*)args;
        zigbee->runControl();
        ERaOs::osThreadDelete(NULL);
    #endif
        ERA_FORCE_UNUSED(args);
    }
#endif

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::apiTask(void* args) {
#if !defined(ERA_NO_RTOS)
    if (args == NULL) {
        ERaOs::osThreadDelete(NULL);
    }
    ERaApi* api = (ERaApi*)args;
    api->runAPI();
    api->_apiTask = NULL;
    ERaOs::osThreadDelete(NULL);
#endif
    ERA_FORCE_UNUSED(args);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::initApiTask() {
#if !defined(ERA_NO_RTOS)
    if ((this->taskSize > 0) && (this->_apiTask == NULL)) {
        this->_apiTask = ERaOs::osThreadNew(this->apiTask, "apiTask", this->taskSize,
                                            this, ERA_API_TASK_PRIORITY);
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

#endif /* INC_ERA_TASK_RP2040_HPP_ */
