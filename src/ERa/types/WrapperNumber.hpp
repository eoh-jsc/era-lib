#ifndef INC_WRAPPER_NUMBER_HPP_
#define INC_WRAPPER_NUMBER_HPP_

#include "WrapperBase.hpp"

template <typename T>
class WrapperNumber
    : public WrapperBase
{
public:
    WrapperNumber(T& _value)
        : value(_value)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_NUMBER;
    }

    WrapperNumber& operator = (T _value) {
        this->value = _value;
        return *this;
    }

protected:
	float get() const override {
		return (float)this->value;
	}

	void set(float _value) override {
		this->value = (T)_value;
	}

    void* getPtr() const override {
        return nullptr;
    }

    void setPtr(const void* _value) override {
        (void)_value;
    }

private:
    T& value;
};

#endif /* INC_WRAPPER_NUMBER_HPP_ */
