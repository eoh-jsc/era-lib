#ifndef INC_ERA_SIMPLE_TINY_HPP_
#define INC_ERA_SIMPLE_TINY_HPP_

#if !defined(ERA_ZIGBEE)
    #define ERA_NO_RTOS
#endif

#if !defined(TINY_GSM_MODEM_ESP32) && \
    !defined(TINY_GSM_MODEM_ESP8266_AT_17)
    #define TINY_GSM_MODEM_ESP32
#endif

#include <PnP/ERaPnPTiny.hpp>
#include <Task/ERaTaskConfig.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<TinyGsmClient, MQTTClient> mqtt;
    ERaPnP< ERaMqtt<TinyGsmClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaPnP< ERaMqtt<TinyGsmClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_TINY_HPP_ */
