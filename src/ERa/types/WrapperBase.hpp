#ifndef INC_WRAPPER_BASE_HPP_
#define INC_WRAPPER_BASE_HPP_

#include <math.h>
#include <float.h>
#include <stdint.h>
#include <string.h>

enum WrapperTypeT : uint8_t {
    WRAPPER_TYPE_INVALID = 0,
    WRAPPER_TYPE_BOOL = 1,
    WRAPPER_TYPE_INT = 2,
    WRAPPER_TYPE_UNSIGNED_INT = 3,
    WRAPPER_TYPE_LONG = 4,
    WRAPPER_TYPE_UNSIGNED_LONG = 5,
    WRAPPER_TYPE_LONG_LONG = 6,
    WRAPPER_TYPE_UNSIGNED_LONG_LONG = 7,
    WRAPPER_TYPE_FLOAT = 8,
    WRAPPER_TYPE_DOUBLE = 9,
    WRAPPER_TYPE_NUMBER = 10,
    WRAPPER_TYPE_STRING = 11
};

class WrapperBase
{
public:
    WrapperBase()
        : type(WrapperTypeT::WRAPPER_TYPE_INVALID)
        , updated(false)
    {}
    ~WrapperBase()
    {}

    WrapperTypeT getType() const {
        return this->type;
    }

    bool isBool() const {
        return this->type == WrapperTypeT::WRAPPER_TYPE_BOOL;
    }

    bool isInt() const {
        return this->type == WrapperTypeT::WRAPPER_TYPE_INT;
    }

    bool isUnsignedInt() const {
        return this->type == WrapperTypeT::WRAPPER_TYPE_UNSIGNED_INT;
    }

    bool isLong() const {
        return this->type == WrapperTypeT::WRAPPER_TYPE_LONG;
    }

    bool isUnsignedLong() const {
        return this->type == WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG;
    }

    bool isLongLong() const {
        return this->type == WrapperTypeT::WRAPPER_TYPE_LONG_LONG;
    }

    bool isUnsignedLongLong() const {
        return this->type == WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG_LONG;
    }

    bool isFloat() const {
        return this->type == WrapperTypeT::WRAPPER_TYPE_FLOAT;
    }

    bool isDouble() const {
        return this->type == WrapperTypeT::WRAPPER_TYPE_DOUBLE;
    }

    bool isNumber() const {
        return ((this->type != WrapperTypeT::WRAPPER_TYPE_INVALID) &&
                (this->type != WrapperTypeT::WRAPPER_TYPE_STRING));
    }

    bool isString() const {
        return this->type == WrapperTypeT::WRAPPER_TYPE_STRING;
    }

    bool isUpdated() {
        bool _updated = this->updated;
        this->updated = false;
        return _updated;
    }

    bool getBool() const {
        return (bool)this->get();
    }

    int getInt() const {
        return (int)this->get();
    }

    unsigned int getUnsignedInt() const {
        return (unsigned int)this->get();
    }

    long getLong() const {
        return (long)this->get();
    }

    unsigned long getUnsignedLong() const {
        return (unsigned long)this->get();
    }

    long long getLongLong() const {
        return (long long)this->get();
    }

    unsigned long long getUnsignedLongLong() const {
        return (unsigned long long)this->get();
    }

    float getFloat() const {
        return this->get();
    }

    double getDouble() const {
        return (double)this->get();
    }

    float getNumber() const {
        return this->get();
    }

    const char* getString() const {
        return (const char*)this->getPtr();
    }

    operator float() const {
        return this->get();
    }

    operator const char*() const {
        return (const char*)this->getPtr();
    }

    operator char*() const {
        return (char*)operator const char*();
    }

    bool operator() (void) {
        bool _updated = this->updated;
        this->updated = false;
        return _updated;
    }

    WrapperBase& operator = (const WrapperBase& _value) {
        if (this == &_value) {
            return *this;
        }
        if (this->type == WrapperTypeT::WRAPPER_TYPE_STRING) {
            return operator = ((const char*)_value.getPtr());
        }
        else {
            return operator = ((float)_value.get());
        }
    }

    WrapperBase& operator = (float _value) {
        this->set(_value);
        return *this;
    }

    WrapperBase& operator = (bool _value) {
        return operator = ((float)_value);
    }

    WrapperBase& operator = (int _value) {
        return operator = ((float)_value);
    }

    WrapperBase& operator = (unsigned int _value) {
        return operator = ((float)_value);
    }

    WrapperBase& operator = (long _value) {
        return operator = ((float)_value);
    }

    WrapperBase& operator = (unsigned long _value) {
        return operator = ((float)_value);
    }

    WrapperBase& operator = (long long _value) {
        return operator = ((float)_value);
    }

    WrapperBase& operator = (unsigned long long _value) {
        return operator = ((float)_value);
    }

    WrapperBase& operator = (double _value) {
        return operator = ((float)_value);
    }

    template <typename T>
    WrapperBase& operator = (T _value) {
        return operator = ((float)_value);
    }

    WrapperBase& operator = (const char* _value) {
        this->setPtr((const void*)_value);
        return *this;
    }

    WrapperBase& operator = (char* _value) {
        return operator = ((const char*)_value);
    }

    bool operator == (const WrapperBase& _value) const {
        if (this == &_value) {
            return true;
        }
        if (this->type == WrapperTypeT::WRAPPER_TYPE_STRING) {
            return operator == ((const char*)_value.getPtr());
        }
        else {
            return operator == ((float)_value.get());
        }
    }

    bool operator == (float _value) const {
        return this->FloatCompare(_value);
    }

    bool operator == (bool _value) const {
        return operator == ((float)_value);
    }

    bool operator == (int _value) const {
        return operator == ((float)_value);
    }

    bool operator == (unsigned int _value) const {
        return operator == ((float)_value);
    }

    bool operator == (long _value) const {
        return operator == ((float)_value);
    }

    bool operator == (unsigned long _value) const {
        return operator == ((float)_value);
    }

    bool operator == (long long _value) const {
        return operator == ((float)_value);
    }

    bool operator == (unsigned long long _value) const {
        return operator == ((float)_value);
    }

    bool operator == (double _value) const {
        return operator == ((float)_value);
    }

    template <typename T>
    bool operator == (T _value) const {
        return operator == ((float)_value);
    }

    bool operator == (const char* _value) const {
        return this->StringCompare(_value);
    }

    bool operator == (char* _value) const {
        return operator == ((const char*)_value);
    }

    bool operator != (const WrapperBase& _value) const {
        if (this == &_value) {
            return false;
        }
        if (this->type == WrapperTypeT::WRAPPER_TYPE_STRING) {
            return operator != ((const char*)_value.getPtr());
        }
        else {
            return operator != ((float)_value.get());
        }
    }

    bool operator != (float _value) const {
        return !this->FloatCompare(_value);
    }

    bool operator != (bool _value) const {
        return operator != ((float)_value);
    }

    bool operator != (int _value) const {
        return operator != ((float)_value);
    }

    bool operator != (unsigned int _value) const {
        return operator != ((float)_value);
    }

    bool operator != (long _value) const {
        return operator != ((float)_value);
    }

    bool operator != (unsigned long _value) const {
        return operator != ((float)_value);
    }

    bool operator != (long long _value) const {
        return operator != ((float)_value);
    }

    bool operator != (unsigned long long _value) const {
        return operator != ((float)_value);
    }

    bool operator != (double _value) const {
        return operator != ((float)_value);
    }

    template <typename T>
    bool operator != (T _value) const {
        return operator != ((float)_value);
    }

    bool operator != (const char* _value) const {
        return !this->StringCompare(_value);
    }

    bool operator != (char* _value) const {
        return operator != ((const char*)_value);
    }

protected:
    virtual float get() const = 0;
    virtual void set(float) = 0;
    virtual void* getPtr() const = 0;
    virtual void setPtr(const void*) = 0;

    WrapperTypeT type;
    bool updated;

private:
    bool FloatCompare(float _value) const {
        if (this->type == WrapperTypeT::WRAPPER_TYPE_STRING) {
            return false;
        }
        float maxVal = (fabs(this->get()) > fabs(_value)) ? fabs(this->get()) : fabs(_value);
        return (fabs(this->get() - _value) <= (maxVal * FLT_EPSILON));
    }

    bool StringCompare(const char* _value) const {
        if (this->type != WrapperTypeT::WRAPPER_TYPE_STRING) {
            return false;
        }
        if (this->getPtr() == _value) {
            return true;
        }
        if (_value == nullptr) {
            return false;
        }
        if (this->getPtr() == nullptr) {
            return false;
        }
        return !strcmp((const char*)this->getPtr(), _value);
    }

};

#endif /* INC_WRAPPER_BASE_HPP_ */
