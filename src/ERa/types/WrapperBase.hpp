#ifndef INC_WRAPPER_BASE_HPP_
#define INC_WRAPPER_BASE_HPP_

#include <math.h>
#include <float.h>
#include <stdint.h>
#include <string.h>
#include <ERa/ERaData.hpp>

enum WrapperTypeT
    : uint8_t {
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
    {}
    virtual ~WrapperBase()
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

    virtual bool updated() {
        return false;
    }

    virtual void setOptions(int) {
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
        return (float)this->get();
    }

    double getDouble() const {
        return this->get();
    }

    double getNumber() const {
        return this->get();
    }

    const char* getString() const {
        return (const char*)this->getPointer();
    }

    ERaDataJson toJSON() const {
        if (!this->isString()) {
            return ERaDataJson();
        }
        return ERaDataJson((const char*)this->getPointer());
    }

    operator double() const {
        return this->get();
    }

    operator const char*() const {
        return (const char*)this->getPointer();
    }

    operator char*() const {
        return (char*)operator const char*();
    }

    bool operator() (void) {
        return this->updated();
    }

    WrapperBase& operator = (const WrapperBase& rwb) {
        if (this == &rwb) {
            return (*this);
        }
        if (this->type == WrapperTypeT::WRAPPER_TYPE_STRING) {
            return operator = ((const char*)rwb.getPointer());
        }
        else {
            return operator = (rwb.get());
        }
    }

    WrapperBase& operator = (float num) {
        return operator = ((double)num);
    }

    WrapperBase& operator = (bool num) {
        return operator = ((double)num);
    }

    WrapperBase& operator = (int num) {
        return operator = ((double)num);
    }

    WrapperBase& operator = (unsigned int num) {
        return operator = ((double)num);
    }

    WrapperBase& operator = (long num) {
        return operator = ((double)num);
    }

    WrapperBase& operator = (unsigned long num) {
        return operator = ((double)num);
    }

    WrapperBase& operator = (long long num) {
        return operator = ((double)num);
    }

    WrapperBase& operator = (unsigned long long num) {
        return operator = ((double)num);
    }

    WrapperBase& operator = (double num) {
        if (!ERaIsSpN(num)) {
            this->set(num);
        }
        return (*this);
    }

    template <typename T>
    WrapperBase& operator = (T rt) {
        return operator = ((double)rt);
    }

    WrapperBase& operator = (const char* cstr) {
        this->setPointer((const void*)cstr);
        return (*this);
    }

    WrapperBase& operator = (char* str) {
        return operator = ((const char*)str);
    }

    bool operator == (const WrapperBase& rwb) const {
        if (this == &rwb) {
            return true;
        }
        if (this->type == WrapperTypeT::WRAPPER_TYPE_STRING) {
            return operator == ((const char*)rwb.getPointer());
        }
        else {
            return operator == (rwb.get());
        }
    }

    bool operator == (float num) const {
        return operator == ((double)num);
    }

    bool operator == (bool num) const {
        return operator == ((double)num);
    }

    bool operator == (int num) const {
        return operator == ((double)num);
    }

    bool operator == (unsigned int num) const {
        return operator == ((double)num);
    }

    bool operator == (long num) const {
        return operator == ((double)num);
    }

    bool operator == (unsigned long num) const {
        return operator == ((double)num);
    }

    bool operator == (long long num) const {
        return operator == ((double)num);
    }

    bool operator == (unsigned long long num) const {
        return operator == ((double)num);
    }

    bool operator == (double num) const {
        return this->DoubleCompare(num);
    }

    template <typename T>
    bool operator == (T rt) const {
        return operator == ((double)rt);
    }

    bool operator == (const char* cstr) const {
        return this->StringCompare(cstr);
    }

    bool operator == (char* str) const {
        return operator == ((const char*)str);
    }

    bool operator != (const WrapperBase& rwb) const {
        if (this == &rwb) {
            return false;
        }
        if (this->type == WrapperTypeT::WRAPPER_TYPE_STRING) {
            return operator != ((const char*)rwb.getPointer());
        }
        else {
            return operator != (rwb.get());
        }
    }

    bool operator != (float num) const {
        return operator != ((double)num);
    }

    bool operator != (bool num) const {
        return operator != ((double)num);
    }

    bool operator != (int num) const {
        return operator != ((double)num);
    }

    bool operator != (unsigned int num) const {
        return operator != ((double)num);
    }

    bool operator != (long num) const {
        return operator != ((double)num);
    }

    bool operator != (unsigned long num) const {
        return operator != ((double)num);
    }

    bool operator != (long long num) const {
        return operator != ((double)num);
    }

    bool operator != (unsigned long long num) const {
        return operator != ((double)num);
    }

    bool operator != (double num) const {
        return !this->DoubleCompare(num);
    }

    template <typename T>
    bool operator != (T rt) const {
        return operator != ((double)rt);
    }

    bool operator != (const char* cstr) const {
        return !this->StringCompare(cstr);
    }

    bool operator != (char* str) const {
        return operator != ((const char*)str);
    }

protected:
    virtual double get() const = 0;
    virtual void set(double) = 0;
    virtual const void* getPointer() const = 0;
    virtual void setPointer(const void*) = 0;

    WrapperTypeT type;

private:
    bool FloatCompare(float num) const {
        if (this->type == WrapperTypeT::WRAPPER_TYPE_STRING) {
            return false;
        }
        float maxVal = (fabs(this->get()) > fabs(num)) ? fabs(this->get()) : fabs(num);
        return (fabs(this->get() - num) <= (maxVal * FLT_EPSILON));
    }

    bool DoubleCompare(double num) const {
        if (this->type == WrapperTypeT::WRAPPER_TYPE_STRING) {
            return false;
        }
        double maxVal = (fabs(this->get()) > fabs(num)) ? fabs(this->get()) : fabs(num);
        return (fabs(this->get() - num) <= (maxVal * DBL_EPSILON));
    }

    bool StringCompare(const char* cstr) const {
        if (this->type != WrapperTypeT::WRAPPER_TYPE_STRING) {
            return false;
        }
        if (this->getPointer() == cstr) {
            return true;
        }
        if (cstr == nullptr) {
            return false;
        }
        if (this->getPointer() == nullptr) {
            return false;
        }
        return !strcmp((const char*)this->getPointer(), cstr);
    }

};

#endif /* INC_WRAPPER_BASE_HPP_ */
