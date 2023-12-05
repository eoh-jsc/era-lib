#ifndef INC_WRAPPER_STRING_HPP_
#define INC_WRAPPER_STRING_HPP_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "WrapperBase.hpp"

class WrapperStringHelper;

class WrapperString
    : public WrapperBase
{
public:
    WrapperString()
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(const WrapperString& _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(const ERaParam& _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(const char* _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(char* _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(char _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(unsigned char _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(int _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(unsigned int _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(long _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(unsigned long _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(long long _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(unsigned long long _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(float _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(double _value)
        : value(nullptr)
        , valueCapacity(0UL)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    ~WrapperString()
    {
        free(this->value);
        this->value = nullptr;
        this->valueCapacity = 0UL;
        this->type = WrapperTypeT::WRAPPER_TYPE_INVALID;
    }

    void reserve(size_t cap) {
        if (cap <= this->valueCapacity) {
            return;
        }
        if (this->value != nullptr) {
            free(this->value);
        }
        this->value = (char*)malloc(cap);
        if (this->value != nullptr) {
            this->valueCapacity = cap;
            memset(this->value, 0, cap);
        }
    }

    const char* c_str() const {
        return ((this->value != nullptr) ? this->value : "");
    }

    const char* getString() const {
        return ((this->value != nullptr) ? this->value : "");
    }

    void printf(const char* format, ...) {
        va_list arg;
        va_list copy;
        va_start(arg, format);
        va_copy(copy, arg);
        char locBuf[128] {0};
        char* buf = locBuf;
        int len = vsnprintf(buf, sizeof(locBuf), format, copy);
        va_end(copy);
        if (len < 0) {
            va_end(arg);
            return;
        }
        if (len >= (int)sizeof(locBuf)) {
            buf = (char*)malloc(len + 1);
            if (buf == nullptr) {
                va_end(arg);
                return;
            }
            len = vsnprintf(buf, len + 1, format, arg);
        }
        va_end(arg);
        this->setString(buf);
        if (buf != locBuf) {
            free(buf);
        }
        buf = nullptr;
    }

    void print(const char* _value) {
        this->printf(_value);
    }

    void println() {
        this->print("\r\n");
    }

    void println(const char* _value) {
        this->printf("%s\r\n", _value);
    }

    void concat(const WrapperString& _value) {
        this->concat((const char*)_value.value);
    }

    void concat(const ERaParam& _value) {
        if (_value.isNumber()) {
            this->concat(_value.getFloat());
        }
        else if (_value.isString()) {
            this->concat(_value.getString());
        }
    }

    void concat(const char* _value) {
        if (_value == nullptr) {
            return;
        }
        if (this->value == nullptr) {
            return this->setString(_value);
        }
        if (!strlen(_value)) {
            return;
        }
        size_t oldLen = strlen(this->value);
        size_t newLen = oldLen;
        newLen += strlen(_value);
        if (newLen >= this->valueCapacity) {
            char* copy = (char*)realloc(this->value, newLen + 1);
            if (copy == nullptr) {
                return;
            }
            this->value = copy;
            this->valueCapacity = newLen + 1;
        }
        snprintf(this->value + oldLen, newLen - oldLen + 1, _value);
        this->value[newLen] = '\0';
        this->updated = true;
    }

    void concat(char* _value) {
        this->concat((const char*)_value);
    }

    void concat(char _value) {
        const char str[2] = { _value, '\0' };
        this->concat(str);
    }

#if defined(__AVR__) || defined(ARDUINO_ARCH_ARC32)

    void concat(unsigned char _value) {
        char str[1 + 8 * sizeof(unsigned char)] {0};
        utoa(_value, str, 10);
        this->concat((const char*)str);
    }

    void concat(int _value) {
		char str[2 + 8 * sizeof(int)] {0};
		itoa(_value, str, 10);
        this->concat((const char*)str);
    }

    void concat(unsigned int _value) {
        char str[1 + 8 * sizeof(unsigned int)] {0};
        utoa(_value, str, 10);
        this->concat((const char*)str);
    }

    void concat(long _value) {
        char str[2 + 8 * sizeof(long)] {0};
        ltoa(_value, str, 10);
        this->concat((const char*)str);
    }

    void concat(unsigned long _value) {
        char str[1 + 8 * sizeof(unsigned long)] {0};
        ultoa(_value, str, 10);
        this->concat((const char*)str);
    }

    void concat(long long _value) {
        char str[2 + 8 * sizeof(long long)] {0};
        ltoa(_value, str, 10);
        this->concat((const char*)str);
    }

    void concat(unsigned long long _value) {
        char str[1 + 8 * sizeof(unsigned long long)] {0};
        ultoa(_value, str, 10);
        this->concat((const char*)str);
    }

    void concat(float _value) {
        char str[33] {0};
        dtostrf(_value, 5, 2, str);
        this->concat((const char*)str);
    }

    void concat(double _value) {
        char str[33] {0};
        dtostrf(_value, 5, 5, str);
        this->concat((const char*)str);
    }

#else

    void concat(unsigned char _value) {
        char str[20] {0};
        snprintf(str, sizeof(str), "%u", _value);
        this->concat((const char*)str);
    }

    void concat(int _value) {
        char str[20] {0};
        snprintf(str, sizeof(str), "%i", _value);
        this->concat((const char*)str);
    }

    void concat(unsigned int _value) {
        char str[20] {0};
        snprintf(str, sizeof(str), "%u", _value);
        this->concat((const char*)str);
    }

    void concat(long _value) {
        char str[20] {0};
        snprintf(str, sizeof(str), "%li", _value);
        this->concat((const char*)str);
    }

    void concat(unsigned long _value) {
        char str[20] {0};
        snprintf(str, sizeof(str), "%lu", _value);
        this->concat((const char*)str);
    }

    void concat(long long _value) {
        char str[33] {0};
        snprintf(str, sizeof(str), "%lli", _value);
        this->concat((const char*)str);
    }

    void concat(unsigned long long _value) {
        char str[33] {0};
        snprintf(str, sizeof(str), "%llu", _value);
        this->concat((const char*)str);
    }

#if defined(ERA_USE_ERA_DTOSTRF)

    void concat(float _value) {
        char str[33] {0};
        ERaDtostrf(_value, 2, str);
        this->concat((const char*)str);
    }

    void concat(double _value) {
        char str[33] {0};
        ERaDtostrf(_value, 5, str);
        this->concat((const char*)str);
    }

#else

    void concat(float _value) {
        char str[33] {0};
        snprintf(str, sizeof(str), "%.2f", _value);
        this->concat((const char*)str);
    }

    void concat(double _value) {
        char str[33] {0};
        snprintf(str, sizeof(str), "%.5f", _value);
        this->concat((const char*)str);
    }

#endif

#endif

    size_t size() const {
        if (this->value == nullptr) {
            return 0;
        }
        return strlen(this->value);
    }

    size_t length() const {
        if (this->value == nullptr) {
            return 0;
        }
        return strlen(this->value);
    }

    char& at(size_t index) {
        return this->value[index];
    }

    void toLowerCase() {
        size_t len = this->length();
        for (size_t i = 0; i < len; ++i) {
            this->value[i] = ::tolower(this->value[i]);
        }
    }

    void toUpperCase() {
        size_t len = this->length();
        for (size_t i = 0; i < len; ++i) {
            this->value[i] = ::toupper(this->value[i]);
        }
    }

    char& operator [] (int index) {
        return this->value[index];
    }

    WrapperString& operator = (const WrapperString& _value) {
        if (this == &_value) {
            return (*this);
        }
        return operator = ((const char*)_value.value);
    }

    WrapperString& operator = (const ERaParam& _value) {
        if (_value.isNumber()) {
            *this = WrapperString(_value.getFloat());
        }
        else if (_value.isString()) {
            operator = (_value.getString());
        }
        return (*this);
    }

    WrapperString& operator = (const char* _value) {
        this->setString(_value);
        return (*this);
    }

    WrapperString& operator = (char* _value) {
        return operator = ((const char*)_value);
    }

    WrapperString& operator += (const WrapperString& _value) {
        this->concat(_value);
        return (*this);
    }

    WrapperString& operator += (const ERaParam& _value) {
        this->concat(_value);
        return (*this);
    }

    WrapperString& operator += (const char* _value) {
        this->concat(_value);
        return (*this);
    }

    WrapperString& operator += (char* _value) {
        this->concat(_value);
        return (*this);
    }

    WrapperString& operator += (char _value) {
        this->concat(_value);
        return (*this);
    }

    WrapperString& operator += (unsigned char _value) {
        this->concat(_value);
        return (*this);
    }

    WrapperString& operator += (int _value) {
        this->concat(_value);
        return (*this);
    }

    WrapperString& operator += (unsigned int _value) {
        this->concat(_value);
        return (*this);
    }

    WrapperString& operator += (long _value) {
        this->concat(_value);
        return (*this);
    }

    WrapperString& operator += (unsigned long _value) {
        this->concat(_value);
        return (*this);
    }

    WrapperString& operator += (long long _value) {
        this->concat(_value);
        return (*this);
    }

    WrapperString& operator += (unsigned long long _value) {
        this->concat(_value);
        return (*this);
    }

    WrapperString& operator += (float _value) {
        this->concat(_value);
        return (*this);
    }

    WrapperString& operator += (double _value) {
        this->concat(_value);
        return (*this);
    }

    bool operator == (const WrapperString& _value) const {
        if (this == &_value) {
            return true;
        }
        return operator == ((const char*)_value.value);
    }

    bool operator == (const char* _value) const {
        return this->StringCompare(_value);
    }

    bool operator == (char* _value) const {
        return operator == ((const char*)_value);
    }

    bool operator != (const WrapperString& _value) const {
        if (this == &_value) {
            return false;
        }
        return operator != ((const char*)_value.value);
    }

    bool operator != (const char* _value) const {
        return !this->StringCompare(_value);
    }

    bool operator != (char* _value) const {
        return operator != ((const char*)_value);
    }

    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, const WrapperString& _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, const ERaParam& _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, const char* _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, char* _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, char _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, unsigned char _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, int _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, unsigned int _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, long _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, unsigned long _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, long long _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, unsigned long long _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, float _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, double _value);

protected:
	float get() const override {
		return 0.0f;
	}

	void set(float _value) override {
		(void)_value;
	}

    void* getPtr() const override {
        return (void*)this->getString();
    }

    void setPtr(const void* _value) override {
        this->setString((const char*)_value);
    }

private:
    void setString(const char* _value) {
        if (!this->isNewString(_value)) {
            return;
        }
        this->updateString(_value);
    }

    void updateString(const char* _value) {
        if (_value == nullptr) {
            return;
        }
        if (this->value == nullptr) {
            this->value = this->stringdup(_value);
            if (this->value != nullptr) {
                this->updated = true;
                this->valueCapacity = strlen(this->value) + 1;
            }
            return;
        }
        size_t len = strlen(_value);
        if (len >= this->valueCapacity) {
            char* copy = (char*)realloc(this->value, len + 1);
            if (copy == nullptr) {
                return;
            }
            snprintf(copy, len + 1, _value);
            this->value = copy;
            this->valueCapacity = len + 1;
        }
        else {
            snprintf(this->value, this->valueCapacity, _value);
        }
        this->value[len] = '\0';
        this->updated = true;
    }

    bool isNewString(const char* _value) const {
        if (_value == nullptr) {
            return false;
        }
        if (this->value == nullptr) {
            return true;
        }
        return strcmp(this->value, _value);
    }

    bool StringCompare(const char* _value) const {
        if (this->value == _value) {
            return true;
        }
        if (_value == nullptr) {
            return false;
        }
        if (this->value == nullptr) {
            return false;
        }
        return !strcmp((const char*)this->value, _value);
    }

    char* stringdup(const char* str) {
        if (str == nullptr) {
            return nullptr;
        }

        size_t length {0};
        char* copy = nullptr;

        length = strlen(str) + sizeof("");
        copy = (char*)malloc(length);
        if (copy == nullptr) {
            return nullptr;
        }
        memcpy(copy, str, length);

        return copy;
    }

    char* value;
    size_t valueCapacity;
};

typedef WrapperString ERaString;

class WrapperStringHelper
    : public WrapperString
{
public:
    WrapperStringHelper()
        : WrapperString()
    {}
    WrapperStringHelper(const WrapperString& _value)
        : WrapperString(_value)
    {}
    WrapperStringHelper(const char* _value)
        : WrapperString(_value)
    {}
    WrapperStringHelper(char* _value)
        : WrapperString(_value)
    {}
    WrapperStringHelper(int _value)
        : WrapperString(_value)
    {}
    WrapperStringHelper(unsigned int _value)
        : WrapperString(_value)
    {}
    WrapperStringHelper(long _value)
        : WrapperString(_value)
    {}
    WrapperStringHelper(unsigned long _value)
        : WrapperString(_value)
    {}
    WrapperStringHelper(long long _value)
        : WrapperString(_value)
    {}
    WrapperStringHelper(unsigned long long _value)
        : WrapperString(_value)
    {}
    WrapperStringHelper(float _value)
        : WrapperString(_value)
    {}
    WrapperStringHelper(double _value)
        : WrapperString(_value)
    {}
};

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, const WrapperString& _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, const ERaParam& _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, const char* _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, char* _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, char _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, unsigned char _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, int _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, unsigned int _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, long _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, unsigned long _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, long long _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, unsigned long long _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, float _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

inline
WrapperStringHelper& operator + (const WrapperStringHelper& wsh, double _value) {
    WrapperStringHelper& a = const_cast<WrapperStringHelper&>(wsh);
    a.concat(_value);
    return a;
}

#endif /* INC_WRAPPER_STRING_HPP_ */
