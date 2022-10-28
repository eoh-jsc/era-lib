#ifndef INC_ERA_MODBUS_ESP32_HPP_
#define INC_ERA_MODBUS_ESP32_HPP_

#include <wiringSerial.h>
#include <Modbus/ERaModbus.hpp>

template <class Api>
void ERaModbus<Api>::configModbus() {
    this->fd = serialOpen("/dev/ttyAMA0", 9600);
}

template <class Api>
bool ERaModbus<Api>::waitResponse(ModbusConfig_t& param, uint8_t* modbusData) {
    int length {0};

    if (this->total++ > 99) {
        this->total = 1;
        this->failRead = 0;
        this->failWrite = 0;
    }

    unsigned long prevMillis = ERaMillis();

    do
    {
        if (!serialDataAvail(this->fd)) {
            ERaDelay(10);
            continue;
        }
        length = 0;
        do {
            modbusData[length] = serialGetchar(this->fd);
        } while ((++length < 256) && serialDataAvail(this->fd));
        if (modbusData[0] == param.addr && modbusData[1] == param.func) {
            if (this->checkReceiveCRC(param, modbusData)) {
                return true;
            }
        }
        ERaDelay(10);
    } while (ERaRemainingTime(prevMillis, MAX_TIMEOUT_MODBUS));
    return false;
}

template <class Api>
void ERaModbus<Api>::sendCommand(const vector<uint8_t>& data) {
    for (const auto& var : data) {
        serialPutchar(this->fd, var);
    }
    serialFlush(this->fd);
}

#endif /* INC_ERA_MODBUS_ESP32_HPP_ */