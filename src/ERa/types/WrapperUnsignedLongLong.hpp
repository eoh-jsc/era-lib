#ifndef INC_WRAPPER_UNSIGNED_LONG_LONG_HPP_
#define INC_WRAPPER_UNSIGNED_LONG_LONG_HPP_

#include "WrapperBase.hpp"

class WrapperUnsignedLongLong
    : public WrapperBase
{
public:
    WrapperUnsignedLongLong(unsigned long long& num)
        : value(num)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG_LONG;
    }

    WrapperUnsignedLongLong& operator = (unsigned long long num) {
        this->value = num;
        return (*this);
    }

protected:
    float get() const override {
        return (float)this->value;
    }

    void set(float num) override {
        this->value = (unsigned long long)num;
    }

    const void* getPointer() const override {
        return nullptr;
    }

    void setPointer(const void* cptr) override {
        (void)cptr;
    }

private:
    unsigned long long& value;
};

#endif /* INC_WRAPPER_UNSIGNED_LONG_LONG_HPP_ */
