#ifndef INC_ERA_SIMPLE_RP2040_WIFI_HPP_
#define INC_ERA_SIMPLE_RP2040_WIFI_HPP_

#define ERA_NO_RTOS

#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaWiFiClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Zigbee/ERaZigbeeArduino.hpp>
#include <Utility/ERaFlashLittleFS.hpp>
#include <Task/ERaTaskRp2040.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<TinyGsmClient, MQTTClient> mqtt;
    ERaWiFi< ERaMqtt<TinyGsmClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaWiFi< ERaMqtt<TinyGsmClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_RP2040_WIFI_HPP_ */
