#ifndef INC_WRAPPER_BOOL_HPP_
#define INC_WRAPPER_BOOL_HPP_

#include "WrapperBase.hpp"

class WrapperBool
    : public WrapperBase
{
public:
    WrapperBool(bool& num)
        : value(num)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_BOOL;
    }

    WrapperBool& operator = (bool num) {
        this->value = num;
        return (*this);
    }

protected:
    float get() const override {
        return (float)this->value;
    }

    void set(float num) override {
        this->value = (bool)num;
    }

    const void* getPointer() const override {
        return nullptr;
    }

    void setPointer(const void* cptr) override {
        (void)cptr;
    }

private:
    bool& value;
};

#endif /* INC_WRAPPER_BOOL_HPP_ */
