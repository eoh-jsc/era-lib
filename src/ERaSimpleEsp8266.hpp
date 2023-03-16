#ifndef INC_ERA_SIMPLE_ESP8266_HPP_
#define INC_ERA_SIMPLE_ESP8266_HPP_

#define ERA_NO_RTOS

#include <PnP/ERaPnPEsp8266.hpp>
#include <Task/ERaTaskEsp8266.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiClient ERaWiFiClient;
    static ERaMqtt<WiFiClient, MQTTClient> mqtt(ERaWiFiClient);
    ERaPnP< ERaMqtt<WiFiClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaPnP< ERaMqtt<WiFiClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP8266_HPP_ */
