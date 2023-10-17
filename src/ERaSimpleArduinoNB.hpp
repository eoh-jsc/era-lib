#ifndef INC_ERA_SIMPLE_ARDUINO_NB_HPP_
#define INC_ERA_SIMPLE_ARDUINO_NB_HPP_

#define ERA_NO_RTOS

#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaArduinoNBClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Storage/ERaFlashConfig.hpp>
#include <Task/ERaTaskConfig.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<NBClient, MQTTClient> mqtt;
    ERaNB< ERaMqtt<NBClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaNB< ERaMqtt<NBClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ARDUINO_NB_HPP_ */
