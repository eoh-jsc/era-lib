#ifndef INC_WRAPPER_UNSIGNED_INT_HPP_
#define INC_WRAPPER_UNSIGNED_INT_HPP_

#include "WrapperBase.hpp"

class WrapperUnsignedInt
    : public WrapperBase
{
public:
    WrapperUnsignedInt(unsigned int& _value)
        : value(_value)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_UNSIGNED_INT;
    }

    WrapperUnsignedInt& operator = (unsigned int _value) {
        this->value = _value;
        return *this;
    }

protected:
	float get() const override {
		return (float)this->value;
	}

	void set(float _value) override {
		this->value = (unsigned int)_value;
	}

    void* getPtr() const override {
        return nullptr;
    }

    void setPtr(const void* _value) override {
        (void)_value;
    }

private:
    unsigned int& value;
};

#endif /* INC_WRAPPER_UNSIGNED_INT_HPP_ */
