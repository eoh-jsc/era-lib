#ifndef INC_ERA_MODBUS_ARDUINO_HPP_
#define INC_ERA_MODBUS_ARDUINO_HPP_

#include <Modbus/ERaModbus.hpp>

template <class Api>
void ERaModbus<Api>::configModbus() {
    if (this->stream != NULL) {
        return;
    }

    this->stream = &Serial1;
    Serial1.setRxBufferSize(MODBUS_BUFFER_SIZE);
    Serial1.begin(9600);
}

template <class Api>
bool ERaModbus<Api>::waitResponse(ModbusConfig_t& param, uint8_t* modbusData) {
    if (this->stream == NULL) {
        return false;
    }

    int length {0};

    if (this->total++ > 99) {
        this->total = 1;
        this->failRead = 0;
        this->failWrite = 0;
    }

    unsigned long startMillis = ERaMillis();

    do {
        if (!this->stream->available()) {
#if defined(ERA_NO_RTOS)
            eraOnWaiting();
            static_cast<Api*>(this)->run();
            if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
                break;
            }
#endif
            ERaDelay(10);
            continue;
        }

        do {
            modbusData[length] = this->stream->read();
        } while ((++length < 256) && this->stream->available());

        ERaLogHex("MB <<", modbusData, length);

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
    if (this->stream == NULL) {
        return;
    }

    ERaLogHex("MB >>", data.data(), data.size());
    this->stream->write(data.data(), data.size());
    this->stream->flush();
}

#endif /* INC_ERA_MODBUS_ARDUINO_HPP_ */
