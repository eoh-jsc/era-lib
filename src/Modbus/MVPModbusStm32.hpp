#ifndef INC_MVP_MODBUS_STM32_HPP_
#define INC_MVP_MODBUS_STM32_HPP_

#include <Modbus/MVPModbus.hpp>

HardwareSerial SerialMB(PA3, PA2);

template <class Api>
void MVPModbus<Api>::configModbus() {
    SerialMB.begin(9600);
}

template <class Api>
bool MVPModbus<Api>::waitResponse(ModbusConfig_t& param, uint8_t* modbusData) {
    int length {0};

    if (this->total++ > 99) {
        this->total = 1;
        this->failRead = 0;
        this->failWrite = 0;
    }

    unsigned long prevMillis = MVPMillis();

    do
    {
        if (!SerialMB.available()) {
            MVPDelay(10);
            continue;
        }
        length = 0;
        do {
            modbusData[length] = SerialMB.read();
        } while ((++length < 256) && SerialMB.available());
        if (modbusData[0] == param.addr && modbusData[1] == param.func) {
            if (this->checkReceiveCRC(param, modbusData)) {
                return true;
            }
        }
        MVPDelay(10);
    } while (MVPRemainingTime(prevMillis, MAX_TIMEOUT_MODBUS));
    return false;
}

template <class Api>
void MVPModbus<Api>::sendCommand(const vector<uint8_t>& data) {
    SerialMB.write(data.data(), data.size());
    SerialMB.flush();
}

#endif /* INC_MVP_MODBUS_STM32_HPP_ */