#ifndef INC_ERA_SIMPLE_ARDUINO_WIFI_AT_HPP_
#define INC_ERA_SIMPLE_ARDUINO_WIFI_AT_HPP_

#define ERA_NO_RTOS

#include <WiFiEspAT.h>
#include <WiFiClient.h>
#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaArduinoWiFiClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Storage/ERaFlashConfig.hpp>
#include <Task/ERaTaskConfig.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiClient ERaWiFiClient;
    static ERaMqtt<WiFiClient, MQTTClient> mqtt(ERaWiFiClient);
    ERaWiFi< ERaMqtt<WiFiClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaWiFi< ERaMqtt<WiFiClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ARDUINO_WIFI_AT_HPP_ */
