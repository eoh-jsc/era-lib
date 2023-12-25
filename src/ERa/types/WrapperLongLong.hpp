#ifndef INC_WRAPPER_LONG_LONG_HPP_
#define INC_WRAPPER_LONG_LONG_HPP_

#include "WrapperBase.hpp"

class WrapperLongLong
    : public WrapperBase
{
public:
    WrapperLongLong(long long& _value)
        : value(_value)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_LONG_LONG;
    }

    WrapperLongLong& operator = (long long _value) {
        this->value = _value;
        return (*this);
    }

protected:
    float get() const override {
        return (float)this->value;
    }

    void set(float _value) override {
        this->value = (long long)_value;
    }

    void* getPointer() const override {
        return nullptr;
    }

    void setPointer(const void* _value) override {
        (void)_value;
    }

private:
    long long& value;
};

#endif /* INC_WRAPPER_LONG_LONG_HPP_ */
