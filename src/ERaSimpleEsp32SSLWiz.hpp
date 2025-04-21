#ifndef INC_ERA_SIMPLE_ESP32_SSL_WIZ_HPP_
#define INC_ERA_SIMPLE_ESP32_SSL_WIZ_HPP_

#define ERA_MQTT_SSL

#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetUdp.h>
#include <Adapters/ERaEthernetSSLClient.hpp>
#include <ERa/ERaApiEsp32.hpp>
#include <Modbus/ERaModbusEsp32.hpp>
#include <Zigbee/ERaZigbeeEsp32.hpp>
#include <Storage/ERaFlashEsp32.hpp>
#include <Task/ERaTaskEsp32.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<Client, MQTTClient> mqtt;
    ERaEthernet< ERaMqtt<Client, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaEthernet< ERaMqtt<Client, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP32_SSL_WIZ_HPP_ */
