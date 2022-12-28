#ifndef INC_ERA_SIMPLE_STM32_GSM_HPP_
#define INC_ERA_SIMPLE_STM32_GSM_HPP_

#include <ERa/ERaTask.hpp>
#include <ERa/ERaApiStm32Def.hpp>
#include <Adapters/ERaGsmClient.hpp>
#include <ERa/ERaApiStm32.hpp>
#include <Modbus/ERaModbusStm32.hpp>
#include <Utility/ERaFlashStm32.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
    #if !defined(ERA_NO_RTOS)
        ERaOs::osThreadNew(runERaLoopTask, "eraTask",
                            1024 * 8, NULL, configMAX_PRIORITIES - 4);
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
        modbus->run(true);
        ERaOs::osThreadDelete(NULL);
    #endif
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
    #if !defined(ERA_NO_RTOS)
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(false);
        ERaOs::osThreadDelete(NULL);
    #endif
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::initERaTask() {
#if defined(ERA_MODBUS)
	Base::Modbus::begin();
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

static ERaFlash flash;
static ERaMqtt<TinyGsmClient, MQTTClient> mqtt;
ERaGsm< ERaMqtt<TinyGsmClient, MQTTClient> > ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_STM32_GSM_HPP_ */
