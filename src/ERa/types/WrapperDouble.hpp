#ifndef INC_WRAPPER_DOUBLE_HPP_
#define INC_WRAPPER_DOUBLE_HPP_

#include "WrapperBase.hpp"

class WrapperDouble
    : public WrapperBase
{
public:
    WrapperDouble(double& _value)
        : value(_value)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_DOUBLE;
    }

    WrapperDouble& operator = (double _value) {
        this->value = _value;
        return *this;
    }

protected:
	float get() const override {
		return (float)this->value;
	}

	void set(float _value) override {
		this->value = (double)_value;
	}

    void* getPtr() const override {
        return nullptr;
    }

    void setPtr(const void* _value) override {
        (void)_value;
    }

private:
    double& value;
};

#endif /* INC_WRAPPER_DOUBLE_HPP_ */
