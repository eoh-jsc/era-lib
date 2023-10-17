#ifndef INC_ERA_SIMPLE_ESP8266_SSL_WIFI_HPP_
#define INC_ERA_SIMPLE_ESP8266_SSL_WIFI_HPP_

#define ERA_NO_RTOS
#define ERA_MQTT_SSL

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaArduinoWiFiClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Storage/ERaFlashEsp8266.hpp>
#include <MQTT/ERaMqttSecure.hpp>
#include <Task/ERaTaskEsp8266.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiClientSecure ERaWiFiClient;
    static ERaMqttSecure<WiFiClientSecure, MQTTClient> mqtt(ERaWiFiClient);
    ERaWiFi< ERaMqttSecure<WiFiClientSecure, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaWiFi< ERaMqttSecure<WiFiClientSecure, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP8266_SSL_WIFI_HPP_ */
