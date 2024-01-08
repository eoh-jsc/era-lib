#ifndef INC_WRAPPER_INT_HPP_
#define INC_WRAPPER_INT_HPP_

#include "WrapperBase.hpp"

class WrapperInt
    : public WrapperBase
{
public:
    WrapperInt(int& num)
        : value(num)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_INT;
    }

    WrapperInt& operator = (int num) {
        this->value = num;
        return (*this);
    }

protected:
    float get() const override {
        return (float)this->value;
    }

    void set(float num) override {
        this->value = (int)num;
    }

    const void* getPointer() const override {
        return nullptr;
    }

    void setPointer(const void* cptr) override {
        (void)cptr;
    }

private:
    int& value;
};

#endif /* INC_WRAPPER_INT_HPP_ */
