#ifndef INC_ERA_SIMPLE_STM32_WIFI_HPP_
#define INC_ERA_SIMPLE_STM32_WIFI_HPP_

#include <ERa/ERaDetect.hpp>
#if !defined(TINY_GSM_RX_BUFFER)
    #define TINY_GSM_RX_BUFFER ERA_MQTT_BUFFER_SIZE
#endif

#include <ERa/ERaTask.hpp>
#include <ERa/ERaApiStm32Def.hpp>
#include <Adapters/ERaWiFiClient.hpp>
#include <ERa/ERaApiStm32.hpp>
#include <Modbus/ERaModbusStm32.hpp>
#include <Utility/ERaFlashStm32.hpp>
#include <Task/ERaTaskStm32.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<TinyGsmClient, MQTTClient> mqtt;
    ERaWiFi< ERaMqtt<TinyGsmClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaWiFi< ERaMqtt<TinyGsmClient, MQTTClient> > ERa;
#endif

#endif /* INC_ERA_SIMPLE_STM32_WIFI_HPP_ */
