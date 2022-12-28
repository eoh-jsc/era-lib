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
static WiFiClient ERaWiFiClient;
static ERaMqtt<WiFiClient, MQTTClient> mqtt(ERaWiFiClient);
ERaPnP< ERaMqtt<WiFiClient, MQTTClient> > ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_ESP8266_HPP_ */
