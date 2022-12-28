#ifndef INC_ERA_SIMPLE_ESP8266_SSL_HPP_
#define INC_ERA_SIMPLE_ESP8266_SSL_HPP_

#define ERA_NO_RTOS
#define ERA_MQTT_SSL

#include <WiFiClientSecure.h>
#include <PnP/ERaPnPEsp8266.hpp>
#include <MQTT/ERaMqttSecure.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::initERaTask() {
#if defined(ERA_MODBUS)
	Base::Modbus::begin();
#endif
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::runERaTask() {
#if defined(ERA_MODBUS)
	Base::Modbus::runRead();
	Base::Modbus::runWrite();
#endif
}

static ERaFlash flash;
static WiFiClientSecure ERaWiFiClient;
static ERaMqttSecure<WiFiClientSecure, MQTTClient> mqtt(ERaWiFiClient);
ERaPnP< ERaMqttSecure<WiFiClientSecure, MQTTClient> > ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_ESP8266_SSL_HPP_ */
