#ifndef INC_ERA_SIMPLE_REALTEK_WIFI_HPP_
#define INC_ERA_SIMPLE_REALTEK_WIFI_HPP_

#include <WiFi.h>
#include <WiFiClient.h>
#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaArduinoWiFiClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Storage/ERaFlashRealtek.hpp>
#include <Task/ERaTaskBase.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiClient ERaWiFiClient;
    static ERaMqtt<WiFiClient, MQTTClient> mqtt(ERaWiFiClient);
    ERaWiFi< ERaMqtt<WiFiClient, MQTTClient> > ERa(mqtt, flash);

    ERA_OPTION_CONNECTED() {
        ERaWiFiClient.setRecvTimeout(ERA_SOCKET_TIMEOUT);
    }
#else
    extern ERaWiFi< ERaMqtt<WiFiClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_REALTEK_WIFI_HPP_ */
