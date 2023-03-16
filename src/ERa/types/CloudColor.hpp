#ifndef INC_CLOUD_COLOR_HPP_
#define INC_CLOUD_COLOR_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "WrapperUnsignedLong.hpp"

class CloudColor
    : public WrapperUnsignedLong
{
public:
    CloudColor()
        : WrapperUnsignedLong(value)
        , value(0)
    {}
    CloudColor(unsigned long _value)
        : WrapperUnsignedLong(value)
        , value(_value)
    {}

    void setColor(unsigned long _value) {
        operator = (_value);
    }

    void setColor(const char* _value) {
        operator = (_value);
    }

    void setColor(uint8_t red, uint8_t green, uint8_t blue) {
        this->value = (unsigned long)blue;
        this->value |= (unsigned long)(green << 8);
        this->value |= (unsigned long)(red << 16);
    }

    void getColor(uint8_t& red, uint8_t& green, uint8_t& blue) {
        red = (uint8_t)((this->value >> 16) & 0xFF);
        green = (uint8_t)((this->value >> 8) & 0xFF);
        blue = (uint8_t)(this->value & 0xFF);
    }

    unsigned long getColor() const {
        return this->value;
    }

    operator unsigned long() const {
        return this->value;
    }

    CloudColor& operator = (const CloudColor& _value) {
        if (this == &_value) {
            return *this;
        }
        return operator = (_value.value);
    }

    CloudColor& operator = (unsigned long _value) {
        this->value = _value;
        return *this;
    }

    CloudColor& operator = (const char* _value) {
        if (_value == nullptr) {
            this->value = 0;
        }
        else if ((*_value == '#') &&
                (strlen(_value) == 7)) {
            this->value = strtoul(_value + 1, nullptr, 16);
        }
        return *this;
    }

    bool operator == (const CloudColor& _value) const {
        if (this == &_value) {
            return true;
        }
        return this->value == _value.value;
    }

    bool operator == (unsigned long _value) const {
        return this->value == _value;
    }

    bool operator == (const char* _value) const {
        unsigned long color {0};
        if ((_value != nullptr) &&
            (*_value == '#') &&
            (strlen(_value) == 7)) {
            color = strtoul(_value + 1, nullptr, 16);
        }
        return this->value == color;
    }

    bool operator != (const CloudColor& _value) const {
        if (this == &_value) {
            return false;
        }
        return this->value != _value.value;
    }

    bool operator != (unsigned long _value) const {
        return this->value != _value;
    }

    bool operator != (const char* _value) const {
        return !(operator == (_value));
    }

protected:
private:
    unsigned long value;
};

#endif /* INC_CLOUD_COLOR_HPP_ */
