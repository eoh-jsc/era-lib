#ifndef INC_ERA_ABBREVIATION_WIFI_HPP_
#define INC_ERA_ABBREVIATION_WIFI_HPP_

#define ERA_ABBR
#define ERA_NO_RTOS

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
#else
    #include <WiFi.h>
#endif

#include <WiFiClient.h>
#include <ERa/ERaApiDetectDef.hpp>
#include <Adapters/ERaArduinoWiFiClient.hpp>
#include <ERa/ERaApiDetect.hpp>
#include <Storage/ERaFlashDetect.hpp>
#include <Task/ERaTaskDetect.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiClient ERaWiFiClient;
    static ERaMqtt<WiFiClient, MQTTClient> mqtt(ERaWiFiClient);
    ERaWiFi< ERaMqtt<WiFiClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaWiFi< ERaMqtt<WiFiClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_ABBREVIATION_WIFI_HPP_ */
