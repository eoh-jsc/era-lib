#if !defined(INC_ERA_MODBUS_STM32_HPP_) && \
    defined(ERA_MODBUS)
#define INC_ERA_MODBUS_STM32_HPP_

#include <Modbus/ERaModbus.hpp>

#if !defined(SerialMB)
    #define SerialMB    SerialModBus<HardwareSerial, uint32_t, uint32_t>::serial(PA3, PA2)
#endif

template <class Api>
void ERaModbus<Api>::configModbus() {
    if (this->streamRTU != NULL) {
        return;
    }

    this->streamRTU = &SerialMB;
    SerialMB.begin(MODBUS_BAUDRATE);
    this->_streamDefault = true;
}

template <class Api>
void ERaModbus<Api>::endModbus() {
    if (!this->streamDefault()) {
        return;
    }

    SerialMB.flush();
    SerialMB.end();
}

template <class Api>
void ERaModbus<Api>::setBaudRate(uint32_t baudrate) {
    ERaModbusBaudrate(baudrate);
    if (!this->streamDefault()) {
        return;
    }

    SerialMB.flush();
    SerialMB.end();
    SerialMB.begin(baudrate);
}

template <class Api>
bool ERaModbus<Api>::waitResponse(ERaModbusResponse* response) {
    if (response == nullptr) {
        return false;
    }
    if (this->stream == NULL) {
        return false;
    }

    this->updateTotalTransmit();

    MillisTime_t startMillis = ERaMillis();

    do {
        if (!this->stream->available()) {
#if defined(ERA_NO_RTOS)
            if (this->runApiResponse) {
                this->thisApi().run();
                this->thisApi().runZigbee();
            }
#endif
            if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
                break;
            }
            ERA_MODBUS_YIELD();
            continue;
        }

        do {
            int c = this->stream->read();
            if (c < 0) {
                continue;
            }
            response->add((uint8_t)c);
        } while (this->stream->available());

        if (response->isComplete()) {
            ERaLogHex("MB <<", response->getMessage(), response->getPosition());
            return response->isSuccess();
        }
        ERA_MODBUS_YIELD();
    } while (ERaRemainingTime(startMillis, this->timeout));
    return false;
}

template <class Api>
void ERaModbus<Api>::sendCommand(uint8_t* data, size_t size) {
    if (data == nullptr) {
        return;
    }
    if (this->stream == NULL) {
        return;
    }

    ERaLogHex("MB >>", data, size);
    this->switchToTransmit();
    this->stream->write(data, size);
    this->stream->flush();
    this->switchToReceive();
}

#endif /* INC_ERA_MODBUS_STM32_HPP_ */
