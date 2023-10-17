#ifndef INC_ERA_SIMPLE_LOGICROM_HPP_
#define INC_ERA_SIMPLE_LOGICROM_HPP_

#if !defined(ERA_ZIGBEE)
    #define ERA_NO_RTOS
#endif

#include <NetClient.h>
#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaLogicromClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Zigbee/ERaZigbeeArduino.hpp>
#include <Storage/ERaFlashLogicrom.hpp>
#include <Task/ERaTaskLogicrom.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static NetClient ERaNetClient;
    static ERaMqtt<NetClient, MQTTClient> mqtt(ERaNetClient);
    ERaGsm< ERaMqtt<NetClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaGsm< ERaMqtt<NetClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_LOGICROM_HPP_ */
