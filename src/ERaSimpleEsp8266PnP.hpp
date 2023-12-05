#ifndef INC_ERA_SIMPLE_ESP8266_PNP_HPP_
#define INC_ERA_SIMPLE_ESP8266_PNP_HPP_

#define ERA_OTA_SSL
#define ERA_DETECT_SSL

#include <PnP/ERaPnPEsp8266.hpp>
#include <Task/ERaTaskEsp8266.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<Client, MQTTClient> mqtt;
    ERaPnP< ERaMqtt<Client, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaPnP< ERaMqtt<Client, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP8266_PNP_HPP_ */
