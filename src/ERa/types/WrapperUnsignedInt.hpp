#ifndef INC_WRAPPER_UNSIGNED_INT_HPP_
#define INC_WRAPPER_UNSIGNED_INT_HPP_

#include "WrapperBase.hpp"

class WrapperUnsignedInt
    : public WrapperBase
{
public:
    WrapperUnsignedInt(unsigned int& num)
        : value(num)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_UNSIGNED_INT;
    }

    WrapperUnsignedInt& operator = (unsigned int num) {
        this->value = num;
        return (*this);
    }

protected:
    float get() const override {
        return (float)this->value;
    }

    void set(float num) override {
        this->value = (unsigned int)num;
    }

    const void* getPointer() const override {
        return nullptr;
    }

    void setPointer(const void* cptr) override {
        (void)cptr;
    }

private:
    unsigned int& value;
};

#endif /* INC_WRAPPER_UNSIGNED_INT_HPP_ */
