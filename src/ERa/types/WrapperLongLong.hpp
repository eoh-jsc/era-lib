#ifndef INC_WRAPPER_LONG_LONG_HPP_
#define INC_WRAPPER_LONG_LONG_HPP_

#include "WrapperBase.hpp"

class WrapperLongLong
    : public WrapperBase
{
public:
    WrapperLongLong(long long& num)
        : value(num)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_LONG_LONG;
    }

    WrapperLongLong& operator = (long long num) {
        this->value = num;
        return (*this);
    }

protected:
    float get() const override {
        return (float)this->value;
    }

    void set(float num) override {
        this->value = (long long)num;
    }

    const void* getPointer() const override {
        return nullptr;
    }

    void setPointer(const void* cptr) override {
        (void)cptr;
    }

private:
    long long& value;
};

#endif /* INC_WRAPPER_LONG_LONG_HPP_ */
