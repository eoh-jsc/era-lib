#ifndef INC_ERA_SIMPLE_ESP32_MULTI_HPP_
#define INC_ERA_SIMPLE_ESP32_MULTI_HPP_

#include <Client.hpp>
#include <Adapters/ERaMultiClient.hpp>
#include <ERa/ERaApiEsp32.hpp>
#include <Modbus/ERaModbusEsp32.hpp>
#include <Zigbee/ERaZigbeeEsp32.hpp>
#include <Storage/ERaFlashEsp32.hpp>
#include <Task/ERaTaskEsp32.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<Client, MQTTClient> mqtt;
    ERaMulti< ERaMqtt<Client, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaMulti< ERaMqtt<Client, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP32_MULTI_HPP_ */
