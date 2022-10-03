#ifndef INC_MVP_MODBUS_ARDUINO_HPP_
#define INC_MVP_MODBUS_ARDUINO_HPP_

#include <Modbus/MVPModbus.hpp>

template <class Api>
void MVPModbus<Api>::configModbus() {
    Serial1.setRxBufferSize(MODBUS_BUFFER_SIZE);
    Serial1.begin(9600);
}

template <class Api>
bool MVPModbus<Api>::waitResponse(ModbusConfig_t& param, uint8_t* modbusData) {
    int length {0};

    if (this->total++ > 99) {
        this->total = 1;
        this->failRead = 0;
        this->failWrite = 0;
    }

    unsigned long tick = MVPMillis();

    do
    {
        length = Serial1.available();
        if (!length) {
            MVPDelay(10);
            continue;
        }
        Serial1.read(modbusData, (length > 256 ? 256 : length));
        if (modbusData[0] == param.addr && modbusData[1] == param.func) {
            if (this->checkReceiveCRC(param, modbusData)) {
                return true;
            }
        }
        MVPDelay(10);
    } while (MVPRemainingTime(tick, MAX_TIMEOUT_MODBUS));
    return false;
}

template <class Api>
void MVPModbus<Api>::sendCommand(const vector<uint8_t>& data) {
    Serial1.write(data.data(), data.size());
    Serial1.flush();
}

#endif /* INC_MVP_MODBUS_ARDUINO_HPP_ */