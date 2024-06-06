#if !defined(INC_ERA_MODBUS_STM32_HPP_) && \
    defined(ERA_MODBUS)
#define INC_ERA_MODBUS_STM32_HPP_

#include <Modbus/ERaModbus.hpp>

#if !defined(SerialMB)
    #define SerialMB    SerialModBus<HardwareSerial, uint32_t, uint32_t>::serial(PA3, PA2)
#endif

inline
void ERaModbusStream::begin() {
    if (this->stream != NULL) {
        return;
    }
    if (this->initialized) {
        return;
    }

    SerialMB.begin(MODBUS_BAUDRATE);
    this->initialized = true;
}

inline
void ERaModbusStream::end() {
    if (!this->initialized) {
        return;
    }

    SerialMB.flush();
    SerialMB.end();
    this->initialized = false;
}

inline
void ERaModbusStream::setBaudRate(uint32_t baudrate) {
    ERaModbusBaudrate(baudrate);
    if (!this->initialized) {
        return;
    }

    SerialMB.flush();
    SerialMB.end();
    SerialMB.begin(baudrate);
}

inline
void ERaModbusStream::flushBytes() {
    if (this->stream != NULL) {
        return this->stream->flush();
    }

    SerialMB.flush();
}

inline
int ERaModbusStream::availableBytes() {
    if (this->stream != NULL) {
        return this->stream->available();
    }

    return SerialMB.available();
}

inline
void ERaModbusStream::sendByte(uint8_t byte) {
    ERaGuardLock(this->mutex);
    if (this->stream != NULL) {
        this->stream->write(byte);
    }
    else {
        SerialMB.write(byte);
    }
    ERaGuardUnlock(this->mutex);
}

inline
void ERaModbusStream::sendBytes(const uint8_t* pData, size_t pDataLen) {
    ERaGuardLock(this->mutex);
    if (this->stream != NULL) {
        this->stream->write(pData, pDataLen);
    }
    else {
        SerialMB.write(pData, pDataLen);
    }
    ERaGuardUnlock(this->mutex);
}

inline
int ERaModbusStream::readByte() {
    if (this->stream != NULL) {
        return this->stream->read();
    }

    return SerialMB.read();
}

inline
int ERaModbusStream::readBytes(uint8_t* buf, size_t size) {
    uint8_t* begin = buf;
    uint8_t* end = buf + size;
    MillisTime_t startMillis = ERaMillis();
    while ((begin < end) && ((ERaMillis() - startMillis) < this->timeout)) {
        int c {0};
        if (this->stream != NULL) {
            c = this->stream->read();
        }
        else {
            c = SerialMB.read();
        }
        if (c < 0) {
            continue;
        }
        *begin++ = (uint8_t)c;
    }
    return ((int)(begin - buf));
}

#endif /* INC_ERA_MODBUS_STM32_HPP_ */
