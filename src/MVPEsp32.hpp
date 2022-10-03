#ifndef INC_MVP_ESP32_HPP_
#define INC_MVP_ESP32_HPP_

#include <PnP/MVPPnPEsp32.hpp>

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

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::initMVPTask() {
	Base::MVPModbus::begin();
}

template <class Transp, class Flash>
void MVPProto<Transp, Flash>::runMVPTask() {
}

static MVPMqtt<WiFiClient, MQTTClient> mqtt;
static MVPFlash flash;
MVPPnP mvp(mqtt, flash);

#endif /* INC_MVP_ESP32_HPP_ */