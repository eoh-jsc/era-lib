#ifndef INC_ERA_SIMPLE_ESP32_HPP_
#define INC_ERA_SIMPLE_ESP32_HPP_

#include <PnP/ERaPnPEsp32.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
        xTaskCreatePinnedToCore(this->modbusTask, "modbusTask", 1024 * 5, this, 17, &this->_modbusTask, 1);
        xTaskCreatePinnedToCore(this->writeModbusTask, "writeModbusTask", 1024 * 5, this, 17, &this->_writeModbusTask, 1);
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(true);
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(false);
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
}

static WiFiClient eraWiFiClient;
static ERaMqtt<WiFiClient, MQTTClient> mqtt(eraWiFiClient);
static ERaFlash flash;
ERaPnP ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_ESP32_HPP_ */
