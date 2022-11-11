#ifndef INC_ERA_SIMPLE_ESP8266_HPP_
#define INC_ERA_SIMPLE_ESP8266_HPP_

#define ERA_NO_RTOS

#include <PnP/ERaPnPEsp8266.hpp>

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
	Base::ERaModbus::begin();
#endif
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::runERaTask() {
#if defined(ERA_MODBUS)
	Base::ERaModbus::runRead();
	Base::ERaModbus::runWrite();
#endif
}

static WiFiClient eraWiFiClient;
static ERaMqtt<WiFiClient, MQTTClient> mqtt(eraWiFiClient);
static ERaFlash flash;
ERaPnP ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_ESP8266_HPP_ */
