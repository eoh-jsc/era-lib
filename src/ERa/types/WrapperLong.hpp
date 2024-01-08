#ifndef INC_WRAPPER_LONG_HPP_
#define INC_WRAPPER_LONG_HPP_

#include "WrapperBase.hpp"

class WrapperLong
    : public WrapperBase
{
public:
    WrapperLong(long& num)
        : value(num)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_LONG;
    }

    WrapperLong& operator = (long num) {
        this->value = num;
        return (*this);
    }

protected:
    float get() const override {
        return (float)this->value;
    }

    void set(float num) override {
        this->value = (long)num;
    }

    const void* getPointer() const override {
        return nullptr;
    }

    void setPointer(const void* cptr) override {
        (void)cptr;
    }

private:
    long& value;
};

#endif /* INC_WRAPPER_LONG_HPP_ */
