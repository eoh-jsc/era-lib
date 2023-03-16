#ifndef INC_WRAPPER_INT_HPP_
#define INC_WRAPPER_INT_HPP_

#include "WrapperBase.hpp"

class WrapperInt
    : public WrapperBase
{
public:
    WrapperInt(int& _value)
        : value(_value)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_INT;
    }

    WrapperInt& operator = (int _value) {
        this->value = _value;
        return *this;
    }

protected:
	float get() const override {
		return (float)this->value;
	}

	void set(float _value) override {
		this->value = (int)_value;
	}

    void* getPtr() const override {
        return nullptr;
    }

    void setPtr(const void* _value) override {
        (void)_value;
    }

private:
    int& value;
};

#endif /* INC_WRAPPER_INT_HPP_ */
