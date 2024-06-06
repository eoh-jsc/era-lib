#ifndef INC_ERA_ABBREVIATION_ETHERNET_HPP_
#define INC_ERA_ABBREVIATION_ETHERNET_HPP_

#define ERA_ABBR
#define ERA_NO_RTOS

#include <Ethernet.h>
#include <EthernetClient.h>
#include <ERa/ERaApiDetectDef.hpp>
#include <Adapters/ERaEthernetClient.hpp>
#include <ERa/ERaApiDetect.hpp>
#include <Storage/ERaFlashDetect.hpp>
#include <Task/ERaTaskDetect.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static EthernetClient ERaEthernetClient;
    static ERaMqtt<EthernetClient, MQTTClient> mqtt(ERaEthernetClient);
    ERaEthernet< ERaMqtt<EthernetClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaEthernet< ERaMqtt<EthernetClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_ABBREVIATION_ETHERNET_HPP_ */
