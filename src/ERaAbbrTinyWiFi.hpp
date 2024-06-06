#ifndef INC_ERA_ABBREVIATION_TINY_WIFI_HPP_
#define INC_ERA_ABBREVIATION_TINY_WIFI_HPP_

#define ERA_ABBR
#define ERA_NO_RTOS

#include <ERa/ERaApiDetectDef.hpp>
#include <Adapters/ERaTinyWiFiClient.hpp>
#include <ERa/ERaApiDetect.hpp>
#include <Storage/ERaFlashDetect.hpp>
#include <Task/ERaTaskDetect.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<TinyGsmClient, MQTTClient> mqtt;
    ERaWiFi< ERaMqtt<TinyGsmClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaWiFi< ERaMqtt<TinyGsmClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_ABBREVIATION_TINY_WIFI_HPP_ */
