#ifndef INC_ERA_SIMPLE_ESP32_ETHERNET_HPP_
#define INC_ERA_SIMPLE_ESP32_ETHERNET_HPP_

#include <Adapters/ERaEthernetEsp32Client.hpp>
#include <Task/ERaTaskEsp32.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiClient ERaWiFiClient;
    static ERaMqtt<Client, MQTTClient> mqtt(ERaWiFiClient);
    ERaEthernet< ERaMqtt<Client, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaEthernet< ERaMqtt<Client, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP32_ETHERNET_HPP_ */
