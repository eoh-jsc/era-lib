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
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(const WrapperString& _value)
        : value(nullptr)
    {
        *this = _value;
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(const char* _value)
        : value(nullptr)
    {
        this->setString(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(char* _value)
        : value(nullptr)
    {
        this->setString((const char*)_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(int _value)
        : value(nullptr)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(unsigned int _value)
        : value(nullptr)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(long _value)
        : value(nullptr)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(unsigned long _value)
        : value(nullptr)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(long long _value)
        : value(nullptr)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(unsigned long long _value)
        : value(nullptr)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(float _value)
        : value(nullptr)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    WrapperString(double _value)
        : value(nullptr)
    {
        this->concat(_value);
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    ~WrapperString()
    {
        free(this->value);
        this->type = WrapperTypeT::WRAPPER_TYPE_INVALID;
    }

    const char* c_str() const {
        return this->value;
    }

    const char* getString() const {
        return this->value;
    }

    void printf(const char* format, ...) {
		va_list arg;
		va_start(arg, format);
		size_t len = vsnprintf(NULL, 0, format, arg);
		char* buf = (char*)malloc(len + 1);
		if (buf != nullptr) {
			vsnprintf(buf, len + 1, format, arg);
            if (this->isNewString((const char*)buf)) {
                this->updateString(buf);
            }
            else {
                free(buf);
            }
		}
		va_end(arg);
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
        char* copy = (char*)realloc(this->value, newLen + 1);
        if (copy == nullptr) {
            return;
        }
        snprintf(copy + oldLen, newLen - oldLen + 1, _value);
        this->value = copy;
        this->updated = true;
    }

#if defined(__AVR__) || defined(ARDUINO_ARCH_ARC32)

    void concat(int _value) {
		char str[2 + 8 * sizeof(_value)] {0};
		itoa(_value, str, 10);
        this->concat((const char*)str);
    }

    void concat(unsigned int _value) {
        char str[1 + 8 * sizeof(_value)] {0};
        utoa(_value, str, 10);
        this->concat((const char*)str);
    }

    void concat(long _value) {
        char str[2 + 8 * sizeof(_value)] {0};
        ltoa(_value, str, 10);
        this->concat((const char*)str);
    }

    void concat(unsigned long _value) {
        char str[1 + 8 * sizeof(_value)] {0};
        ultoa(_value, str, 10);
        this->concat((const char*)str);
    }

    void concat(long long _value) {
        char str[2 + 8 * sizeof(_value)] {0};
        ltoa(_value, str, 10);
        this->concat((const char*)str);
    }

    void concat(unsigned long long _value) {
        char str[1 + 8 * sizeof(_value)] {0};
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
            return *this;
        }
        return operator = ((const char*)_value.value);
    }

    WrapperString& operator = (const char* _value) {
        this->setString(_value);
        return *this;
    }

    WrapperString& operator = (char* _value) {
        return operator = ((const char*)_value);
    }

    WrapperString& operator += (const WrapperString& _value) {
        return operator += ((const char*)_value.value);
    }

    WrapperString& operator += (const char* _value) {
        this->concat(_value);
        return *this;
    }

    WrapperString& operator += (char* _value) {
        return operator += ((const char*)_value);
    }

    WrapperString& operator += (int _value) {
        this->concat(_value);
        return *this;
    }

    WrapperString& operator += (unsigned int _value) {
        this->concat(_value);
        return *this;
    }

    WrapperString& operator += (long _value) {
        this->concat(_value);
        return *this;
    }

    WrapperString& operator += (unsigned long _value) {
        this->concat(_value);
        return *this;
    }

    WrapperString& operator += (long long _value) {
        this->concat(_value);
        return *this;
    }

    WrapperString& operator += (unsigned long long _value) {
        this->concat(_value);
        return *this;
    }

    WrapperString& operator += (float _value) {
        this->concat(_value);
        return *this;
    }

    WrapperString& operator += (double _value) {
        this->concat(_value);
        return *this;
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
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, const char* _value);
    friend WrapperStringHelper& operator + (const WrapperStringHelper& wsh, char* _value);
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
        this->updateString(this->stringdup(_value));
    }

    void updateString(char* _value) {
        if (_value == nullptr) {
            return;
        }
        free(this->value);
        this->value = _value;
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
    a.concat((const char*)_value.value);
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
    a.concat((const char*)_value);
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
