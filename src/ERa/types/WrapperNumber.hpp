#ifndef INC_WRAPPER_NUMBER_HPP_
#define INC_WRAPPER_NUMBER_HPP_

#include "WrapperBase.hpp"

template <typename T>
class WrapperNumber
    : public WrapperBase
{
public:
    WrapperNumber(T& num)
        : value(num)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_NUMBER;
    }

    WrapperNumber& operator = (T num) {
        this->value = num;
        return (*this);
    }

protected:
    double get() const override {
        return (double)this->value;
    }

    void set(double num) override {
        this->value = (T)num;
    }

    const void* getPointer() const override {
        return nullptr;
    }

    void setPointer(const void* cptr) override {
        (void)cptr;
    }

private:
    T& value;
};

#endif /* INC_WRAPPER_NUMBER_HPP_ */
