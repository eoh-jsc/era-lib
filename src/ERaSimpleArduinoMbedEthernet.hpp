#ifndef INC_ERA_SIMPLE_ARDUINO_MBED_ETHERNET_HPP_
#define INC_ERA_SIMPLE_ARDUINO_MBED_ETHERNET_HPP_

#if !defined(ERA_ZIGBEE)
    #define ERA_NO_RTOS
#endif

#include <Ethernet.h>
#include <EthernetClient.h>
#include <ERa/ERaApiMbedDef.hpp>
#include <Adapters/ERaEthernetClient.hpp>
#include <ERa/ERaApiMbed.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Zigbee/ERaZigbeeArduino.hpp>
#include <Storage/ERaFlashMbed.hpp>
#include <Task/ERaTaskMbed.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static EthernetClient ERaEthernetClient;
    static ERaMqtt<EthernetClient, MQTTClient> mqtt(ERaEthernetClient);
    ERaEthernet< ERaMqtt<EthernetClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaEthernet< ERaMqtt<EthernetClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ARDUINO_MBED_ETHERNET_HPP_ */
