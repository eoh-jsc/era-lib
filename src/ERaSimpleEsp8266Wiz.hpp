#ifndef INC_ERA_SIMPLE_ESP8266_WIZ_HPP_
#define INC_ERA_SIMPLE_ESP8266_WIZ_HPP_

#define ERA_NO_RTOS

#include <Ethernet.h>
#include <EthernetClient.h>
#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaEthernetClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Storage/ERaFlashEsp8266.hpp>
#include <Task/ERaTaskEsp8266.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static EthernetClient ERaEthernetClient;
    static ERaMqtt<EthernetClient, MQTTClient> mqtt(ERaEthernetClient);
    ERaEthernet< ERaMqtt<EthernetClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaEthernet< ERaMqtt<EthernetClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP8266_WIZ_HPP_ */
