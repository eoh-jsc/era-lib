#ifndef INC_MVP_SIMPLE_ESP32_HPP_
#define INC_MVP_SIMPLE_ESP32_HPP_

#include <PnP/MVPPnPEsp32.hpp>

#if defined(MVP_MODBUS)
    template <class Api>
    void MVPModbus<Api>::initModbusTask() {
        xTaskCreatePinnedToCore(this->modbusTask, "modbusTask", 1024 * 5, this, 17, &this->_modbusTask, 1);
        xTaskCreatePinnedToCore(this->writeModbusTask, "writeModbusTask", 1024 * 5, this, 17, &this->_writeModbusTask, 1);
    }

    template <class Api>
    void MVPModbus<Api>::modbusTask(void* args) {
        MVPModbus* modbus = (MVPModbus*)args;
        modbus->run(true);
    }

    template <class Api>
    void MVPModbus<Api>::writeModbusTask(void* args) {
        MVPModbus* modbus = (MVPModbus*)args;
        modbus->run(false);
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
}

static WiFiClient mvpWifiClient;
static MVPMqtt<WiFiClient, MQTTClient> mqtt(mvpWifiClient);
static MVPFlash flash;
MVPPnP mvp(mqtt, flash);

#endif /* INC_MVP_SIMPLE_ESP32_HPP_ */