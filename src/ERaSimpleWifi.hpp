#ifndef INC_ERA_SIMPLE_WIFI_HPP_
#define INC_ERA_SIMPLE_WIFI_HPP_

#define ERA_NO_RTOS
#include <ERa/ERaDetect.hpp>
#if !defined(TINY_GSM_RX_BUFFER)
    #define TINY_GSM_RX_BUFFER ERA_MQTT_BUFFER_SIZE
#endif

#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaWiFiClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Utility/ERaFlashConfig.hpp>
#include <Task/ERaTaskArduino.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<TinyGsmClient, MQTTClient> mqtt;
    ERaWiFi< ERaMqtt<TinyGsmClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaWiFi< ERaMqtt<TinyGsmClient, MQTTClient> > ERa;
#endif

#endif /* INC_ERA_SIMPLE_WIFI_HPP_ */
