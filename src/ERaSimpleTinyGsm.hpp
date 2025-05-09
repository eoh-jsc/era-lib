#ifndef INC_ERA_SIMPLE_TINY_GSM_HPP_
#define INC_ERA_SIMPLE_TINY_GSM_HPP_

#if !defined(ERA_ZIGBEE)
    #define ERA_NO_RTOS
#endif

#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaTinyGsmClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Zigbee/ERaZigbeeArduino.hpp>
#include <Storage/ERaFlashConfig.hpp>
#include <Task/ERaTaskConfig.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<Client, MQTTClient> mqtt;
    ERaGsm< ERaMqtt<Client, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaGsm< ERaMqtt<Client, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_TINY_GSM_HPP_ */
