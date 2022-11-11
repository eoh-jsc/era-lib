#ifndef INC_ERA_MODBUS_ARDUINO_HPP_
#define INC_ERA_MODBUS_ARDUINO_HPP_

#include <Modbus/ERaModbus.hpp>

template <class Api>
void ERaModbus<Api>::configModbus() {
    Serial1.setRxBufferSize(MODBUS_BUFFER_SIZE);
    Serial1.begin(9600);
}

template <class Api>
bool ERaModbus<Api>::waitResponse(ModbusConfig_t& param, uint8_t* modbusData) {
    int length {0};

    if (this->total++ > 99) {
        this->total = 1;
        this->failRead = 0;
        this->failWrite = 0;
    }

    unsigned long startMillis = ERaMillis();

    do {
        length = Serial1.available();
        if (!length) {
#if defined(ERA_NO_RTOS)
            eraOnWaiting();
            static_cast<Api*>(this)->run();
#endif
            ERaDelay(10);
            continue;
        }
        Serial1.read(modbusData, (length > 256 ? 256 : length));
        if (modbusData[0] == param.addr && modbusData[1] == param.func) {
            if (this->checkReceiveCRC(param, modbusData)) {
                return true;
            }
        }
        ERaDelay(10);
    } while (ERaRemainingTime(startMillis, MAX_TIMEOUT_MODBUS));
    return false;
}

template <class Api>
void ERaModbus<Api>::sendCommand(const vector<uint8_t>& data) {
    Serial1.write(data.data(), data.size());
    Serial1.flush();
}

#endif /* INC_ERA_MODBUS_ARDUINO_HPP_ */
