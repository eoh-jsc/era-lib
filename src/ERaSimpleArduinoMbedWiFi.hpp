#ifndef INC_ERA_SIMPLE_ARDUINO_MBED_WIFI_HPP_
#define INC_ERA_SIMPLE_ARDUINO_MBED_WIFI_HPP_

#if !defined(ERA_ZIGBEE)
    #define ERA_NO_RTOS
#endif

#include <WiFi.h>
#include <WiFiClient.h>
#include <ERa/ERaApiMbedDef.hpp>
#include <Adapters/ERaArduinoWiFiClient.hpp>
#include <ERa/ERaApiMbed.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Zigbee/ERaZigbeeArduino.hpp>
#include <Storage/ERaFlashMbed.hpp>
#include <Task/ERaTaskMbed.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiClient ERaWiFiClient;
    static ERaMqtt<WiFiClient, MQTTClient> mqtt(ERaWiFiClient);
    ERaWiFi< ERaMqtt<WiFiClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaWiFi< ERaMqtt<WiFiClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ARDUINO_MBED_WIFI_HPP_ */
