#ifndef INC_ERA_SIMPLE_MULTI_HPP_
#define INC_ERA_SIMPLE_MULTI_HPP_

#define ERA_NO_RTOS

#include <Client.h>
#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaMultiClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Utility/ERaFlashConfig.hpp>
#include <Task/ERaTaskConfig.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<Client, MQTTClient> mqtt;
    ERaMulti< ERaMqtt<Client, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaMulti< ERaMqtt<Client, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_MULTI_HPP_ */
