#ifndef INC_ERA_MODBUS_MESSAGE_HPP_
#define INC_ERA_MODBUS_MESSAGE_HPP_

#include <stdint.h>
#include <stddef.h>

class ERaModbusMessage
{
public:
    ERaModbusMessage(uint8_t _length)
        : buffer(nullptr)
        , length(_length)
        , index(0)
    {
        this->buffer = new uint8_t[_length] {0};
    }
    virtual ~ERaModbusMessage()
    {
        delete[] this->buffer;
    }

    uint8_t* getMessage() {
        return this->buffer;
    }

    uint8_t getSize() {
        return this->length;
    }

    uint8_t getPosition() {
        return this->index;
    }

    void add(uint8_t value) {
        if (this->index >= this->length) {
            return;
        }
        this->buffer[this->index++] = value;
    }

protected:
    uint8_t* buffer;
    uint8_t length;
    uint8_t index;
};

#endif /* INC_ERA_MODBUS_MESSAGE_HPP_ */