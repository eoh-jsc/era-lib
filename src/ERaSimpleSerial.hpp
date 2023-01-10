#ifndef INC_ERA_SIMPLE_SERIAL_HPP_
#define INC_ERA_SIMPLE_SERIAL_HPP_

#define ERA_NO_RTOS

#include <Adapters/ERaSerialClient.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Utility/ERaFlashConfig.hpp>
#include <Task/ERaTaskArduino.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<ERaClient, MQTTClient> mqtt;
    ERaStream< ERaMqtt<ERaClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaStream< ERaMqtt<ERaClient, MQTTClient> > ERa;
#endif

#endif /* INC_ERA_SIMPLE_SERIAL_HPP_ */
