#ifndef INC_ERA_MODBUS_STREAM_HPP_
#define INC_ERA_MODBUS_STREAM_HPP_

#include <stdio.h>
#include <stdint.h>
#include <Modbus/ERaModbusStreamDef.hpp>

class ERaModbusStream
{
    const char* TAG = "Modbus";

    typedef void* QueueMessage_t;

public:
    ERaModbusStream()
        : stream(NULL)
        , state(ModbusStreamStateT::MB_STREAM_STATE_BEGIN)
        , initialized(false)
        , messageHandle(NULL)
        , mutex(NULL)
        , timeout(1000UL)
    {}
    ~ERaModbusStream()
    {}

    void setModbusStreamTimeout(unsigned long _timeout) {
        this->timeout = _timeout;
    }

protected:
    void begin();
    void end();
    void setBaudRate(uint32_t baudrate);
    void flushBytes();
    int availableBytes();
    void sendByte(uint8_t byte);
    void sendBytes(const uint8_t* pData, size_t pDataLen);
    int readByte();
    int readBytes(uint8_t* buf, size_t size);

    void streamStart() {
        this->state = ModbusStreamStateT::MB_STREAM_STATE_BEGIN;
    }

    void streamEnd() {
        this->state = ModbusStreamStateT::MB_STREAM_STATE_END;
    }

    Stream* stream;
    ModbusStreamStateT state;

private:
    bool initialized;

    QueueMessage_t messageHandle;

    ERaMutex_t mutex;
    unsigned long timeout;
};

#endif /* INC_ERA_MODBUS_STREAM_HPP_ */
