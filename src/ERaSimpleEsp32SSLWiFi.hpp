#ifndef INC_ERA_SIMPLE_ESP32_SSL_WIFI_HPP_
#define INC_ERA_SIMPLE_ESP32_SSL_WIFI_HPP_

#define ERA_MQTT_SSL

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Adapters/ERaArduinoWiFiClient.hpp>
#include <ERa/ERaApiEsp32.hpp>
#include <Modbus/ERaModbusEsp32.hpp>
#include <Zigbee/ERaZigbeeEsp32.hpp>
#include <Storage/ERaFlashEsp32.hpp>
#include <MQTT/ERaMqttSecure.hpp>
#include <Task/ERaTaskEsp32.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiClientSecure ERaWiFiClient;
    static ERaMqttSecure<WiFiClientSecure, MQTTClient> mqtt(ERaWiFiClient);
    ERaWiFi< ERaMqttSecure<WiFiClientSecure, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaWiFi< ERaMqttSecure<WiFiClientSecure, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP32_SSL_WIFI_HPP_ */
