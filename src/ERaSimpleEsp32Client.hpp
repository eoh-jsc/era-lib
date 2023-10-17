#ifndef INC_ERA_SIMPLE_ESP32_CLIENT_HPP_
#define INC_ERA_SIMPLE_ESP32_CLIENT_HPP_

#include <PnP/ERaPnPEsp32.hpp>
#include <Task/ERaTaskEsp32.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiClient ERaWiFiClient;
    static ERaMqtt<Client, MQTTClient> mqtt(ERaWiFiClient);
    ERaPnP< ERaMqtt<Client, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaPnP< ERaMqtt<Client, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP32_CLIENT_HPP_ */
