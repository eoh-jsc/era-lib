#ifndef INC_ERA_SIMPLE_ESP32_WIFI_HPP_
#define INC_ERA_SIMPLE_ESP32_WIFI_HPP_

#include <WiFi.h>
#include <WiFiClient.h>
#include <Adapters/ERaArduinoWiFiClient.hpp>
#include <ERa/ERaApiEsp32.hpp>
#include <Modbus/ERaModbusEsp32.hpp>
#include <Zigbee/ERaZigbeeEsp32.hpp>
#include <Storage/ERaFlashEsp32.hpp>
#include <Task/ERaTaskEsp32.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiClient ERaWiFiClient;
    static ERaMqtt<WiFiClient, MQTTClient> mqtt(ERaWiFiClient);
    ERaWiFi< ERaMqtt<WiFiClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaWiFi< ERaMqtt<WiFiClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP32_WIFI_HPP_ */
