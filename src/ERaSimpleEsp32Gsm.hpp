#ifndef INC_ERA_SIMPLE_ESP32_GSM_HPP_
#define INC_ERA_SIMPLE_ESP32_GSM_HPP_

#if !defined(ERA_ZIGBEE)
    #define ERA_NO_RTOS
#endif

#include <Adapters/ERaTinyGsmClient.hpp>
#include <ERa/ERaApiEsp32.hpp>
#include <Modbus/ERaModbusEsp32.hpp>
#include <Zigbee/ERaZigbeeEsp32.hpp>
#include <Storage/ERaFlashEsp32.hpp>
#include <Task/ERaTaskEsp32.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<TinyGsmClient, MQTTClient> mqtt;
    ERaGsm< ERaMqtt<TinyGsmClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaGsm< ERaMqtt<TinyGsmClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_ESP32_GSM_HPP_ */
