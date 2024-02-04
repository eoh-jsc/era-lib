#ifndef INC_WRAPPER_DOUBLE_HPP_
#define INC_WRAPPER_DOUBLE_HPP_

#include "WrapperBase.hpp"

class WrapperDouble
    : public WrapperBase
{
public:
    WrapperDouble(double& num)
        : value(num)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_DOUBLE;
    }

    WrapperDouble& operator = (double num) {
        this->value = num;
        return (*this);
    }

protected:
    double get() const override {
        return this->value;
    }

    void set(double num) override {
        this->value = num;
    }

    const void* getPointer() const override {
        return nullptr;
    }

    void setPointer(const void* cptr) override {
        (void)cptr;
    }

private:
    double& value;
};

#endif /* INC_WRAPPER_DOUBLE_HPP_ */
