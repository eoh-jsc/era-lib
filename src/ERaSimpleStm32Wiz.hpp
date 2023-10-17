#ifndef INC_ERA_SIMPLE_STM32_WIZ_HPP_
#define INC_ERA_SIMPLE_STM32_WIZ_HPP_

#if !defined(ERA_ZIGBEE)
    #define ERA_NO_RTOS
#endif

#include <Ethernet.h>
#include <EthernetClient.h>
#include <ERa/ERaTask.hpp>
#include <ERa/ERaApiStm32Def.hpp>
#include <Adapters/ERaEthernetClient.hpp>
#include <ERa/ERaApiStm32.hpp>
#include <Modbus/ERaModbusStm32.hpp>
#include <Zigbee/ERaZigbeeStm32.hpp>
#include <Storage/ERaFlashStm32.hpp>
#include <Task/ERaTaskStm32.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static EthernetClient ERaEthernetClient;
    static ERaMqtt<EthernetClient, MQTTClient> mqtt(ERaEthernetClient);
    ERaEthernet< ERaMqtt<EthernetClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaEthernet< ERaMqtt<EthernetClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_STM32_WIZ_HPP_ */
