#ifndef INC_ERA_SIMPLE_REALTEK_HPP_
#define INC_ERA_SIMPLE_REALTEK_HPP_

#include <PnP/ERaPnPRealtek.hpp>
#include <Task/ERaTaskBase.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiClient ERaWiFiClient;
    static ERaMqtt<WiFiClient, MQTTClient> mqtt(ERaWiFiClient);
    ERaPnP< ERaMqtt<WiFiClient, MQTTClient> > ERa(mqtt, flash);

    ERA_OPTION_CONNECTED() {
        ERaWiFiClient.setRecvTimeout(ERA_SOCKET_TIMEOUT);
    }
#else
    extern ERaPnP< ERaMqtt<WiFiClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_REALTEK_HPP_ */
