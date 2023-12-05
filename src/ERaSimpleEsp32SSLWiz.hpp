#ifndef INC_ERA_SIMPLE_ESP32_SSL_WIZ_HPP_
#define INC_ERA_SIMPLE_ESP32_SSL_WIZ_HPP_

#define ERA_MQTT_SSL

#include <Ethernet.h>
#include <EthernetClient.h>
#include <Adapters/ERaEthernetSSLClient.hpp>
#include <ERa/ERaApiEsp32.hpp>
#include <Modbus/ERaModbusEsp32.hpp>
#include <Zigbee/ERaZigbeeEsp32.hpp>
#include <Storage/ERaFlashEsp32.hpp>
#include <Task/ERaTaskEsp32.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<BearSSLClient, MQTTClient> mqtt;
    ERaEthernet< ERaMqtt<BearSSLClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaEthernet< ERaMqtt<BearSSLClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP32_SSL_WIZ_HPP_ */
