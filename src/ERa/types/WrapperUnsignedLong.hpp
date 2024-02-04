#ifndef INC_WRAPPER_UNSIGNED_LONG_HPP_
#define INC_WRAPPER_UNSIGNED_LONG_HPP_

#include "WrapperBase.hpp"

class WrapperUnsignedLong
    : public WrapperBase
{
public:
    WrapperUnsignedLong(unsigned long& num)
        : value(num)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG;
    }

    WrapperUnsignedLong& operator = (unsigned long num) {
        this->value = num;
        return (*this);
    }

protected:
    double get() const override {
        return (double)this->value;
    }

    void set(double num) override {
        this->value = (unsigned long)num;
    }

    const void* getPointer() const override {
        return nullptr;
    }

    void setPointer(const void* cptr) override {
        (void)cptr;
    }

private:
    unsigned long& value;
};

#endif /* INC_WRAPPER_UNSIGNED_LONG_HPP_ */
