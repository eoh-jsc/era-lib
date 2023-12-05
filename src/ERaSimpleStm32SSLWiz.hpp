#ifndef INC_ERA_SIMPLE_STM32_SSL_WIZ_HPP_
#define INC_ERA_SIMPLE_STM32_SSL_WIZ_HPP_

#if !defined(ERA_ZIGBEE)
    #define ERA_NO_RTOS
#endif

#define ERA_MQTT_SSL

#include <Ethernet.h>
#include <EthernetClient.h>
#include <ERa/ERaTask.hpp>
#include <ERa/ERaApiStm32Def.hpp>
#include <Adapters/ERaEthernetSSLClient.hpp>
#include <ERa/ERaApiStm32.hpp>
#include <Modbus/ERaModbusStm32.hpp>
#include <Zigbee/ERaZigbeeStm32.hpp>
#include <Storage/ERaFlashStm32.hpp>
#include <Task/ERaTaskStm32.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    #include <Time/ERaTimeC.hpp>

    static ERaFlash flash;
    static ERaMqtt<BearSSLClient, MQTTClient> mqtt;
    ERaEthernet< ERaMqtt<BearSSLClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaEthernet< ERaMqtt<BearSSLClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_STM32_SSL_WIZ_HPP_ */
