#ifndef INC_ERA_SIMPLE_RP2040_WIZ_HPP_
#define INC_ERA_SIMPLE_RP2040_WIZ_HPP_

#include <Ethernet.h>
#include <EthernetClient.h>
#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaEthernetClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Zigbee/ERaZigbeeArduino.hpp>
#include <Storage/ERaFlashLittleFS.hpp>
#include <Task/ERaTaskRp2040.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static EthernetClient ERaEthernetClient;
    static ERaMqtt<EthernetClient, MQTTClient> mqtt(ERaEthernetClient);
    ERaEthernet< ERaMqtt<EthernetClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaEthernet< ERaMqtt<EthernetClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_RP2040_WIZ_HPP_ */
