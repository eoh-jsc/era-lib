#ifndef INC_ERA_MODBUS_STM32_HPP_
#define INC_ERA_MODBUS_STM32_HPP_

#include <Modbus/ERaModbus.hpp>

HardwareSerial SerialMB(PA3, PA2);

template <class Api>
void ERaModbus<Api>::configModbus() {
    SerialMB.begin(9600);
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
        if (!SerialMB.available()) {
            ERaDelay(10);
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
        ERaDelay(10);
    } while (ERaRemainingTime(prevMillis, MAX_TIMEOUT_MODBUS));
    return false;
}

template <class Api>
void ERaModbus<Api>::sendCommand(const vector<uint8_t>& data) {
    SerialMB.write(data.data(), data.size());
    SerialMB.flush();
}

#endif /* INC_ERA_MODBUS_STM32_HPP_ */
