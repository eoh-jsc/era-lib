#if !defined(INC_ERA_MODBUS_ARDUINO_HPP_) && \
    defined(ERA_MODBUS)
#define INC_ERA_MODBUS_ARDUINO_HPP_

#include <Modbus/ERaModbus.hpp>

#if !defined(SerialMB)
    #if defined(ESP8266)
        #include <SoftwareSerial.h>

        #define SerialMB     SerialModBus<SoftwareSerial, int, int>::serial(14, 12)
    #elif defined(ARDUINO_ARCH_STM32)
        #define SerialMB     SerialModBus<HardwareSerial, uint32_t, uint32_t>::serial(PA3, PA2)
    #elif defined(ARDUINO_ARCH_RP2040)
        #define SerialMB     Serial1
    #elif !defined(__MBED__) &&         \
        defined(ARDUINO_ARCH_NRF5)
        #define SerialMB     Serial
    #elif defined(ARDUINO_ARCH_AVR) ||  \
        defined(ARDUINO_ARCH_ARC32)
        #if defined(HAVE_HWSERIAL1)
            #define SerialMB Serial1
        #else
            #include <SoftwareSerial.h>

            #define SerialMB SerialModBus<SoftwareSerial, uint8_t, uint8_t>::serial(10, 11)
        #endif
    #elif defined(ARDUINO_ARCH_SAMD)
        #define SerialMB     Serial1
    #elif defined(ARDUINO_ARCH_ARM)
        #define SerialMB     Serial1
    #elif defined(PARTICLE) ||          \
        defined(SPARK)
        #define SerialMB     Serial1
    #elif defined(RTL8722DM) ||         \
        defined(ARDUINO_AMEBA)
        #define SerialMB     Serial1
    #elif defined(__MBED__)
        #define SerialMB     Serial1
    #else
        #define SerialMB     Serial1
    #endif
#endif

inline
void ERaModbusStream::begin() {
    if (this->stream != NULL) {
        return;
    }
    if (this->initialized) {
        return;
    }

#if defined(ERA_MODBUS_DISABLE_RX_PULLUP)
    SerialMB.enableRxGPIOPullUp(false);
#endif
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

#endif /* INC_ERA_MODBUS_ARDUINO_HPP_ */
