#ifndef INC_MVP_SIMPLE_ESP8266_HPP_
#define INC_MVP_SIMPLE_ESP8266_HPP_

#include <PnP/MVPPnPEsp8266.hpp>

#if defined(MVP_MODBUS)
    template <class Api>
    void MVPModbus<Api>::initModbusTask() {
    }

    template <class Api>
    void MVPModbus<Api>::modbusTask(void* args) {
    }

    template <class Api>
    void MVPModbus<Api>::writeModbusTask(void* args) {
    }
#endif

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::initMVPTask() {
#if defined(MVP_MODBUS)
	Base::MVPModbus::begin();
#endif
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::runMVPTask() {
#if defined(MVP_MODBUS)
	Base::MVPModbus::runRead();
	Base::MVPModbus::runWrite();
#endif
}

static WiFiClient mvpWifiClient;
static MVPMqtt<WiFiClient, MQTTClient> mqtt(mvpWifiClient);
static MVPFlash flash;
MVPPnP mvp(mqtt, flash);

#endif /* INC_MVP_SIMPLE_ESP8266_HPP_ */