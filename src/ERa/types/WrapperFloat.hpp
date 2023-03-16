#ifndef INC_WRAPPER_FLOAT_HPP_
#define INC_WRAPPER_FLOAT_HPP_

#include "WrapperBase.hpp"

class WrapperFloat
    : public WrapperBase
{
public:
    WrapperFloat(float& _value)
        : value(_value)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_FLOAT;
    }

    WrapperFloat& operator = (float _value) {
        this->value = _value;
        return *this;
    }

protected:
	float get() const override {
		return this->value;
	}

	void set(float _value) override {
		this->value = _value;
	}

    void* getPtr() const override {
        return nullptr;
    }

    void setPtr(const void* _value) override {
        (void)_value;
    }

private:
    float& value;
};

#endif /* INC_WRAPPER_FLOAT_HPP_ */
