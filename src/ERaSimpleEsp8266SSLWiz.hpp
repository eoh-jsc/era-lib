#ifndef INC_ERA_SIMPLE_ESP8266_SSL_WIZ_HPP_
#define INC_ERA_SIMPLE_ESP8266_SSL_WIZ_HPP_

#define ERA_NO_RTOS
#define ERA_MQTT_SSL

#include <Ethernet.h>
#include <EthernetClient.h>
#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaEthernetSSLClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Storage/ERaFlashEsp8266.hpp>
#include <Task/ERaTaskEsp8266.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<BearSSLClient, MQTTClient> mqtt;
    ERaEthernet< ERaMqtt<BearSSLClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaEthernet< ERaMqtt<BearSSLClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP8266_SSL_WIZ_HPP_ */
