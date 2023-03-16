#ifndef INC_WRAPPER_LONG_HPP_
#define INC_WRAPPER_LONG_HPP_

#include "WrapperBase.hpp"

class WrapperLong
    : public WrapperBase
{
public:
    WrapperLong(long& _value)
        : value(_value)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_LONG;
    }

    WrapperLong& operator = (long _value) {
        this->value = _value;
        return *this;
    }

protected:
	float get() const override {
		return (float)this->value;
	}

	void set(float _value) override {
		this->value = (long)_value;
	}

    void* getPtr() const override {
        return nullptr;
    }

    void setPtr(const void* _value) override {
        (void)_value;
    }

private:
    long& value;
};

#endif /* INC_WRAPPER_LONG_HPP_ */
