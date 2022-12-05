#ifndef INC_ERA_MODBUS_LINUX_HPP_
#define INC_ERA_MODBUS_LINUX_HPP_

#include <Modbus/ERaModbus.hpp>

template <class Api>
void ERaModbus<Api>::configModbus() {
    if (this->fd < 0) {
        this->fd = serialOpen("/dev/ttyAMA0", MODBUS_BAUDRATE);
        this->_streamDefault = true;
    }
}

template <class Api>
void ERaModbus<Api>::setBaudRate(uint32_t baudrate) {
    ERaModbusBaudrate(baudrate);
    if ((this->fd < 0) || !this->streamDefault()) {
        return;
    }

    serialFlush(this->fd);
    serialClose(this->fd);
    this->fd = serialOpen("/dev/ttyAMA0", baudrate);
}

template <class Api>
bool ERaModbus<Api>::waitResponse(ModbusConfig_t& param, uint8_t* modbusData) {
    if (this->fd < 0) {
        return false;
    }

    int length {0};

    if (this->total++ > 99) {
        this->total = 1;
        this->failRead = 0;
        this->failWrite = 0;
    }

    MillisTime_t startMillis = ERaMillis();

    do
    {
        if (!serialDataAvail(this->fd)) {
#if defined(ERA_NO_RTOS)
            ERaOnWaiting();
            this->thisApi().run();
#endif
            if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
                break;
            }
            ERA_MODBUS_YIELD();
            continue;
        }

        do {
            modbusData[length] = serialGetchar(this->fd);
        } while ((++length < 256) && serialDataAvail(this->fd));

        ERaLogHex("MB <<", modbusData, length);

        if (modbusData[0] == param.addr && modbusData[1] == param.func) {
            if (this->checkReceiveCRC(param, modbusData)) {
                return true;
            }
        }
        ERA_MODBUS_YIELD();
    } while (ERaRemainingTime(startMillis, MAX_TIMEOUT_MODBUS));
    return false;
}

template <class Api>
void ERaModbus<Api>::sendCommand(const vector<uint8_t>& data) {
    if (this->fd < 0) {
        return;
    }

    ERaLogHex("MB >>", data.data(), data.size());
    for (const auto& var : data) {
        serialPutchar(this->fd, var);
    }
    serialFlush(this->fd);
}

#endif /* INC_ERA_MODBUS_LINUX_HPP_ */
