#ifndef INC_ERA_SIMPLE_REALTEK_SSL_WIFI_HPP_
#define INC_ERA_SIMPLE_REALTEK_SSL_WIFI_HPP_

#define ERA_MQTT_SSL

#include <WiFi.h>
#include <WiFiSSLClient.h>
#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaArduinoWiFiClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Storage/ERaFlashRealtek.hpp>
#include <MQTT/ERaMqttSecure.hpp>
#include <Task/ERaTaskBase.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiSSLClient ERaWiFiClient;
    static ERaMqttSecure<WiFiSSLClient, MQTTClient> mqtt(ERaWiFiClient);
    ERaWiFi< ERaMqttSecure<WiFiSSLClient, MQTTClient> > ERa(mqtt, flash);

    ERA_OPTION_CONNECTED() {
        ERaWiFiClient.setRecvTimeout(ERA_SOCKET_TIMEOUT);
    }
#else
    extern ERaWiFi< ERaMqttSecure<WiFiSSLClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_REALTEK_SSL_WIFI_HPP_ */
