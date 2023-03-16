#ifndef INC_WRAPPER_UNSIGNED_LONG_HPP_
#define INC_WRAPPER_UNSIGNED_LONG_HPP_

#include "WrapperBase.hpp"

class WrapperUnsignedLong
    : public WrapperBase
{
public:
    WrapperUnsignedLong(unsigned long& _value)
        : value(_value)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG;
    }

    WrapperUnsignedLong& operator = (unsigned long _value) {
        this->value = _value;
        return *this;
    }

protected:
	float get() const override {
		return (float)this->value;
	}

	void set(float _value) override {
		this->value = (unsigned long)_value;
	}

    void* getPtr() const override {
        return nullptr;
    }

    void setPtr(const void* _value) override {
        (void)_value;
    }

private:
    unsigned long& value;
};

#endif /* INC_WRAPPER_UNSIGNED_LONG_HPP_ */
