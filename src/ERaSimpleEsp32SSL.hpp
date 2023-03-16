#ifndef INC_ERA_SIMPLE_ESP32_SSL_HPP_
#define INC_ERA_SIMPLE_ESP32_SSL_HPP_

#define ERA_MQTT_SSL

#include <WiFiClientSecure.h>
#include <PnP/ERaPnPEsp32.hpp>
#include <MQTT/ERaMqttSecure.hpp>
#include <Task/ERaTaskEsp32.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiClientSecure ERaWiFiClient;
    static ERaMqttSecure<WiFiClientSecure, MQTTClient> mqtt(ERaWiFiClient);
    ERaPnP< ERaMqttSecure<WiFiClientSecure, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaPnP< ERaMqttSecure<WiFiClientSecure, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP32_SSL_HPP_ */
