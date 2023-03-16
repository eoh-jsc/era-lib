#ifndef INC_WRAPPER_BOOL_HPP_
#define INC_WRAPPER_BOOL_HPP_

#include "WrapperBase.hpp"

class WrapperBool
    : public WrapperBase
{
public:
    WrapperBool(bool& _value)
        : value(_value)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_BOOL;
    }

    WrapperBool& operator = (bool _value) {
        this->value = _value;
        return *this;
    }

protected:
	float get() const override {
		return (float)this->value;
	}

	void set(float _value) override {
		this->value = (bool)_value;
	}

    void* getPtr() const override {
        return nullptr;
    }

    void setPtr(const void* _value) override {
        (void)_value;
    }

private:
    bool& value;
};

#endif /* INC_WRAPPER_BOOL_HPP_ */
