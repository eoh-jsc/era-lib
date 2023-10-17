#ifndef INC_ERA_SIMPLE_REALTEK_SSL_HPP_
#define INC_ERA_SIMPLE_REALTEK_SSL_HPP_

#define ERA_MQTT_SSL

#include <WiFiSSLClient.h>
#include <PnP/ERaPnPRealtek.hpp>
#include <MQTT/ERaMqttSecure.hpp>
#include <Task/ERaTaskBase.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiSSLClient ERaWiFiClient;
    static ERaMqttSecure<WiFiSSLClient, MQTTClient> mqtt(ERaWiFiClient);
    ERaPnP< ERaMqttSecure<WiFiSSLClient, MQTTClient> > ERa(mqtt, flash);

    ERA_OPTION_CONNECTED() {
        ERaWiFiClient.setRecvTimeout(ERA_SOCKET_TIMEOUT);
    }
#else
    extern ERaPnP< ERaMqttSecure<WiFiSSLClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_REALTEK_SSL_HPP_ */
