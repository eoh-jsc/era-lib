#ifndef INC_ERA_SIMPLE_WIFI_NINA_HPP_
#define INC_ERA_SIMPLE_WIFI_NINA_HPP_

#if !defined(ERA_ZIGBEE)
    #define ERA_NO_RTOS
#endif

#include <PnP/ERaPnPWiFiNINA.hpp>
#include <Task/ERaTaskConfig.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static WiFiClient ERaWiFiClient;
    static ERaMqtt<Client, MQTTClient> mqtt(ERaWiFiClient);
    ERaPnP< ERaMqtt<Client, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaPnP< ERaMqtt<Client, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_WIFI_NINA_HPP_ */
