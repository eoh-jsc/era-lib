#ifndef INC_ERA_SIMPLE_LOGICROM_SSL_HPP_
#define INC_ERA_SIMPLE_LOGICROM_SSL_HPP_

#if !defined(ERA_ZIGBEE)
    #define ERA_NO_RTOS
#endif

#define ERA_MQTT_SSL

#include <NetClientSecure.h>
#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaLogicromClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Zigbee/ERaZigbeeArduino.hpp>
#include <Storage/ERaFlashLogicrom.hpp>
#include <MQTT/ERaMqttSecure.hpp>
#include <Task/ERaTaskLogicrom.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static NetClientSecure ERaNetClient;
    static ERaMqttSecure<NetClientSecure, MQTTClient> mqtt(ERaNetClient);
    ERaGsm< ERaMqttSecure<NetClientSecure, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaGsm< ERaMqttSecure<NetClientSecure, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_LOGICROM_SSL_HPP_ */
