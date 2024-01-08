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
    CloudColor(unsigned long num)
        : WrapperUnsignedLong(value)
        , value(num)
    {}

    void setColor(unsigned long num) {
        operator = (num);
    }

    void setColor(const char* cstr) {
        operator = (cstr);
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

    CloudColor& operator = (const CloudColor& rcc) {
        if (this == &rcc) {
            return (*this);
        }
        return operator = (rcc.value);
    }

    CloudColor& operator = (unsigned long num) {
        this->value = num;
        return (*this);
    }

    CloudColor& operator = (const char* cstr) {
        if (cstr == nullptr) {
            this->value = 0;
        }
        else if ((*cstr == '#') &&
                (strlen(cstr) == 7)) {
            this->value = strtoul(cstr + 1, nullptr, 16);
        }
        return (*this);
    }

    bool operator == (const CloudColor& rcc) const {
        if (this == &rcc) {
            return true;
        }
        return (this->value == rcc.value);
    }

    bool operator == (unsigned long num) const {
        return (this->value == num);
    }

    bool operator == (const char* cstr) const {
        unsigned long color {0};
        if ((cstr != nullptr) &&
            (*cstr == '#') &&
            (strlen(cstr) == 7)) {
            color = strtoul(cstr + 1, nullptr, 16);
        }
        return (this->value == color);
    }

    bool operator != (const CloudColor& rcc) const {
        if (this == &rcc) {
            return false;
        }
        return (this->value != rcc.value);
    }

    bool operator != (unsigned long num) const {
        return (this->value != num);
    }

    bool operator != (const char* cstr) const {
        return !(operator == (cstr));
    }

protected:
private:
    unsigned long value;
};

#endif /* INC_CLOUD_COLOR_HPP_ */
