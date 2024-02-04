#ifndef INC_WRAPPER_STRING_HPP_
#define INC_WRAPPER_STRING_HPP_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "WrapperBase.hpp"

class WrapperString;

#if defined(ARDUINO) && \
    defined(ERA_USE_ARDUINO_STRING)

    #define ERA_ARDUINO_STRING

    #include <Arduino.h>

    class ERaString
        : public String
    {
        friend class WrapperString;

    public:
        ERaString()
            : String("")
        {}
        ERaString(const String& str)
            : String(str)
        {}
        ERaString(const ERaParam& param)
            : String("")
        {
            if (param.isNumber()) {
                String::operator = (param.getFloat());
            }
            else if (param.isString()) {
                String::operator = (param.getString());
            }
            else if (param.isObject()) {
                String::operator = (param.getObject()->getString());
            }
        }
        ERaString(const ERaDataJson& rjs)
            : String("")
        {
            if (!rjs.isValid()) {
                return;
            }
            String::operator = (const_cast<ERaDataJson&>(rjs).getString());
        }
        ERaString(const char* cstr)
            : String(cstr)
        {}
        ERaString(char* str)
            : String(str)
        {}
        ERaString(char c)
            : String(c)
        {}
        ERaString(unsigned char num)
            : String(num)
        {}
        ERaString(int num)
            : String(num)
        {}
        ERaString(unsigned int num)
            : String(num)
        {}
        ERaString(long num)
            : String(num)
        {}
        ERaString(unsigned long num)
            : String(num)
        {}
        ERaString(long long num)
            : String(num)
        {}
        ERaString(unsigned long long num)
            : String(num)
        {}
        ERaString(float num)
            : String(num)
        {}
        ERaString(double num)
            : String(num)
        {}

        const char* getString() const {
            return String::c_str();
        }

        void setString(const char* cstr) {
            String::operator = (cstr);
        }

        ERaDataJson toJSON() const {
            return ERaDataJson(String::c_str());
        }

    protected:
    private:
        bool updated(String& estr) {
            if ((*this) == estr) {
                return false;
            }
            estr = (*this);
            return true;
        }

    };

#else
    class ERaStringHelper;

    class ERaString
    {
        friend class WrapperString;
        const static size_t CapacityMax = ERA_STRING_CAPACITY_MAX;

    public:
        ERaString()
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {}
        ERaString(const ERaString& estr)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(estr);
        }
        ERaString(const ERaParam& param)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(param);
        }
        ERaString(const ERaDataJson& rjs)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(rjs);
        }
        ERaString(const char* cstr)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(cstr);
        }
        ERaString(char* str)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(str);
        }
        ERaString(char c)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(c);
        }
        ERaString(unsigned char num)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(int num)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(unsigned int num)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(long num)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(unsigned long num)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(long long num)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(unsigned long long num)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(float num)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(double num)
            : value(nullptr)
            , valueCapacity(0UL)
            , isUpdated(false)
        {
            this->concat(num);
        }
        ~ERaString()
        {
            this->invalidate();
        }

        void invalidate() {
            if (this->value != nullptr) {
                free(this->value);
            }
            this->value = nullptr;
            this->valueCapacity = 0UL;
            this->isUpdated = false;
        }

        void reserve(size_t cap) {
            if (cap > CapacityMax) {
                return;
            }
            if (cap < this->valueCapacity) {
                return;
            }
            char* copy = (char*)realloc(this->value, cap + 1);
            if (copy != nullptr) {
                if (this->value == nullptr) {
                    memset(copy, 0, cap + 1);
                }
                this->value = copy;
                this->value[cap] = '\0';
                this->valueCapacity = (cap + 1);
            }
            else {
                this->valueCapacity = 0;
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
            if ((len < 0) || (len > (int)CapacityMax)) {
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

        void print(const char* cstr) {
            this->printf(cstr);
        }

        void println() {
            this->print("\r\n");
        }

        void println(const char* cstr) {
            this->printf("%s\r\n", cstr);
        }

        void concat(const ERaString& estr) {
            this->concat((const char*)estr.value);
        }

        void concat(const ERaParam& param) {
            if (param.isNumber()) {
                this->concat(param.getFloat());
            }
            else if (param.isString()) {
                this->concat(param.getString());
            }
            else if (param.isObject()) {
                this->concat(param.getObject());
            }
        }

        void concat(const ERaDataJson& rjs) {
            if (!rjs.isValid()) {
                return;
            }
            this->concat(const_cast<ERaDataJson&>(rjs).getString());
        }

        void concat(const ERaDataJson* rjs) {
            if (rjs == nullptr) {
                return;
            }
            this->concat(*rjs);
        }

        void concat(const char* cstr) {
            if (cstr == nullptr) {
                return;
            }
            if (this->value == nullptr) {
                return this->setString(cstr);
            }
            if (!strlen(cstr)) {
                return;
            }
            size_t oldLen = strlen(this->value);
            size_t newLen = oldLen;
            newLen += strlen(cstr);
            if (newLen > CapacityMax) {
                return;
            }
            if (newLen >= this->valueCapacity) {
                char* copy = (char*)realloc(this->value, newLen + 1);
                if (copy == nullptr) {
                    return;
                }
                this->value = copy;
                this->valueCapacity = newLen + 1;
            }
            snprintf(this->value + oldLen, newLen - oldLen + 1, cstr);
            this->value[newLen] = '\0';
            this->isUpdated = true;
        }

        void concat(char* str) {
            this->concat((const char*)str);
        }

        void concat(char c) {
            const char str[2] = { c, '\0' };
            this->concat(str);
        }

    #if defined(__AVR__) || defined(ARDUINO_ARCH_ARC32)

        void concat(unsigned char num) {
            char str[1 + 8 * sizeof(unsigned char)] {0};
            utoa(num, str, 10);
            this->concat((const char*)str);
        }

        void concat(int num) {
            char str[2 + 8 * sizeof(int)] {0};
            itoa(num, str, 10);
            this->concat((const char*)str);
        }

        void concat(unsigned int num) {
            char str[1 + 8 * sizeof(unsigned int)] {0};
            utoa(num, str, 10);
            this->concat((const char*)str);
        }

        void concat(long num) {
            char str[2 + 8 * sizeof(long)] {0};
            ltoa(num, str, 10);
            this->concat((const char*)str);
        }

        void concat(unsigned long num) {
            char str[1 + 8 * sizeof(unsigned long)] {0};
            ultoa(num, str, 10);
            this->concat((const char*)str);
        }

        void concat(long long num) {
            char str[2 + 8 * sizeof(long long)] {0};
            ltoa(num, str, 10);
            this->concat((const char*)str);
        }

        void concat(unsigned long long num) {
            char str[1 + 8 * sizeof(unsigned long long)] {0};
            ultoa(num, str, 10);
            this->concat((const char*)str);
        }

        void concat(float num) {
            char str[33] {0};
            dtostrf(num, 5, 2, str);
            this->concat((const char*)str);
        }

        void concat(double num) {
            char str[33] {0};
            dtostrf(num, 5, 5, str);
            this->concat((const char*)str);
        }

    #else

        void concat(unsigned char num) {
            char str[20] {0};
            snprintf(str, sizeof(str), "%u", num);
            this->concat((const char*)str);
        }

        void concat(int num) {
            char str[20] {0};
            snprintf(str, sizeof(str), "%i", num);
            this->concat((const char*)str);
        }

        void concat(unsigned int num) {
            char str[20] {0};
            snprintf(str, sizeof(str), "%u", num);
            this->concat((const char*)str);
        }

        void concat(long num) {
            char str[20] {0};
            snprintf(str, sizeof(str), "%li", num);
            this->concat((const char*)str);
        }

        void concat(unsigned long num) {
            char str[20] {0};
            snprintf(str, sizeof(str), "%lu", num);
            this->concat((const char*)str);
        }

        void concat(long long num) {
            char str[33] {0};
            snprintf(str, sizeof(str), "%lli", num);
            this->concat((const char*)str);
        }

        void concat(unsigned long long num) {
            char str[33] {0};
            snprintf(str, sizeof(str), "%llu", num);
            this->concat((const char*)str);
        }

    #if defined(ERA_USE_ERA_DTOSTRF)

        void concat(float num) {
            char str[33] {0};
            ERaDtostrf(num, 2, str);
            this->concat((const char*)str);
        }

        void concat(double num) {
            char str[33] {0};
            ERaDtostrf(num, 5, str);
            this->concat((const char*)str);
        }

    #else

        void concat(float num) {
            char str[33] {0};
            snprintf(str, sizeof(str), "%.2f", num);
            this->concat((const char*)str);
        }

        void concat(double num) {
            char str[33] {0};
            snprintf(str, sizeof(str), "%.5f", num);
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
            if (this->value == nullptr) {
                static char c {0};
                return c;
            }
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

        void trim() {
            if (!this->length()) {
                return;
            }
            char* begin = this->value;
            while (isspace(*begin)) {
                begin++;
            }
            char* end = (this->value + this->length() - 1);
            while (isspace(*end) && (end >= begin)) {
                end--;
            }
            unsigned int newLen = (end - begin + 1);
            if (begin > this->value) {
                this->isUpdated = true;
                memmove(this->value, begin, newLen);
            }
            this->value[newLen] = '\0';
        }

        int indexOf(char c) const {
            return this->indexOf(c, 0);
        }

        int indexOf(char c, size_t index) const {
            if (index >= this->length()) {
                return -1;
            }
            const char* ptr = strchr(this->value + index, c);
            if (ptr == nullptr) {
                return -1;
            }
            return (int)(ptr - this->value);
        }

        int indexOf(const ERaString& str) const {
            return this->indexOf(str, 0);
        }

        int indexOf(const ERaString& str, size_t index) const {
            if (!str.length()) {
                return -1;
            }
            if (index >= this->length()) {
                return -1;
            }
            const char* ptr = strstr(this->value + index, str.value);
            if (ptr == nullptr) {
                return -1;
            }
            return (int)(ptr - this->value);
        }

        int lastIndexOf(char c) const {
            return this->lastIndexOf(c, this->length() - 1);
        }

        int lastIndexOf(char c, size_t index) const {
            if (index >= this->length()) {
                return -1;
            }
            const char temp = this->value[index + 1];
            this->value[index + 1] = '\0';
            const char* ptr = strrchr(this->value, c);
            this->value[index + 1] = temp;
            if (ptr == nullptr) {
                return -1;
            }
            return (int)(ptr - this->value);
        }

        int lastIndexOf(const ERaString& str) const {
            return this->lastIndexOf(str, this->length() - str.length());
        }

        int lastIndexOf(const ERaString& str, size_t index) const {
            if (!this->length() || !str.length()) {
                return -1;
            }
            if (str.length() > this->length()) {
                return -1;
            }
            if (index >= this->length()) {
                index = (this->length() - 1);
            }
            int found = -1;
            for (char* ptr = this->value; ptr <= (this->value + index); ++ptr) {
                ptr = strstr(ptr, str.value);
                if (ptr == nullptr) {
                    break;
                }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
                if ((ptr - this->value) <= index) {
                    found = (ptr - this->value);
                }
#pragma GCC diagnostic pop
            }
            return found;
        }

        void replace(char find, char replace) {
            size_t len = this->length();
            for (size_t i = 0; i < len; ++i) {
                if (this->value[i] != find) {
                    continue;
                }
                this->value[i] = replace;
            }
        }

        void replace(const ERaString& find, const ERaString& replace) {
            if (!find.length()) {
                return;
            }
            if (!replace.length()) {
                return;
            }
            int diff = (replace.length() - find.length());
            char* foundAt = nullptr;
            char* readFrom = this->value;
            if (!diff) {
                while ((foundAt = strstr(readFrom, find.value)) != nullptr) {
                    memmove(foundAt, replace.value, replace.length());
                    readFrom = (foundAt + find.length());
                }
            }
            else if (diff < 0) {
                char* writeTo = this->value;
                while ((foundAt = strstr(readFrom, find.value)) != nullptr) {
                    size_t n = (foundAt - readFrom);
                    memmove(writeTo, readFrom, n);
                    writeTo += n;
                    memmove(writeTo, replace.value, replace.length());
                    writeTo += replace.length();
                    readFrom = (foundAt + find.length());
                }
                memmove(writeTo, readFrom, strlen(readFrom) + 1);
            }
            else {
                size_t size = this->length();
                while ((foundAt = strstr(readFrom, find.value)) != nullptr) {
                    readFrom = (foundAt + find.length());
                    size += diff;
                }
                if (size == this->length()) {
                    return;
                }
                if (size > CapacityMax) {
                    return;
                }
                this->reserve(size);
                if (this->value == nullptr) {
                    return;
                }
                int index = (this->length() - 1);
                while ((index >= 0) && ((index = this->lastIndexOf(find, index)) >= 0)) {
                    size_t len = this->length();
                    readFrom = (this->value + index + find.length());
                    memmove(readFrom + diff, readFrom, len - (readFrom - this->value));
                    size_t newLen = (len + diff);
                    memmove(this->value + index, replace.value, replace.length());
                    this->value[newLen] = '\0';
                    index--;
                }
            }
        }

        void remove(size_t index) {
            this->remove(index, (size_t)(-1));
        }

        void remove(size_t index, size_t count) {
            if (index >= this->length()) {
                return;
            }
            if (!count) {
                return;
            }
            if (count > (this->length() - index)) {
                count = (this->length() - index);
            }
            char* writeTo = (this->value + index);
            size_t newLen = (this->length() - count);
            memmove(writeTo, this->value + index + count, newLen - index);
            this->value[newLen] = '\0';
        }

        int toInt() const {
            if (this->value == nullptr) {
                return 0;
            }
            return atol(this->value);
        }

        float toFloat() const {
            if (this->value == nullptr) {
                return 0.0f;
            }
            return (float)atof(this->value);
        }

        double toDouble() const {
            if (this->value == nullptr) {
                return 0.0;
            }
            return atof(this->value);
        }

        ERaDataJson toJSON() const {
            return ERaDataJson(this->c_str());
        }

        void clear() {
            if (this->value == nullptr) {
                return;
            }
            memset(this->value, 0, this->valueCapacity);
        }

        operator const char*() const {
            if (this->value == nullptr) {
                return "";
            }
            return this->value;
        }

        char& operator [] (int index) {
            return this->at(index);
        }

        ERaString& operator = (const ERaString& res) {
            if (this == &res) {
                return (*this);
            }
            this->assign(res);
            return (*this);
        }

        ERaString& operator = (const ERaParam& param) {
            this->assign(param);
            return (*this);
        }

        ERaString& operator = (const ERaDataJson& rjs) {
            this->assign(rjs);
            return (*this);
        }

        ERaString& operator = (const char* cstr) {
            this->assign(cstr);
            return (*this);
        }

        ERaString& operator = (char* str) {
            this->assign(str);
            return (*this);
        }

        ERaString& operator = (char c) {
            this->assign(c);
            return (*this);
        }

        ERaString& operator = (unsigned char num) {
            this->assign(num);
            return (*this);
        }

        ERaString& operator = (int num) {
            this->assign(num);
            return (*this);
        }

        ERaString& operator = (unsigned int num) {
            this->assign(num);
            return (*this);
        }

        ERaString& operator = (long num) {
            this->assign(num);
            return (*this);
        }

        ERaString& operator = (unsigned long num) {
            this->assign(num);
            return (*this);
        }

        ERaString& operator = (long long num) {
            this->assign(num);
            return (*this);
        }

        ERaString& operator = (unsigned long long num) {
            this->assign(num);
            return (*this);
        }

        ERaString& operator = (float num) {
            this->assign(num);
            return (*this);
        }

        ERaString& operator = (double num) {
            this->assign(num);
            return (*this);
        }

        ERaString& operator += (const ERaString& res) {
            this->concat(res);
            return (*this);
        }

        ERaString& operator += (const ERaParam& param) {
            this->concat(param);
            return (*this);
        }

        ERaString& operator += (const ERaDataJson& rjs) {
            this->concat(rjs);
            return (*this);
        }

        ERaString& operator += (const char* cstr) {
            this->concat(cstr);
            return (*this);
        }

        ERaString& operator += (char* str) {
            this->concat(str);
            return (*this);
        }

        ERaString& operator += (char c) {
            this->concat(c);
            return (*this);
        }

        ERaString& operator += (unsigned char num) {
            this->concat(num);
            return (*this);
        }

        ERaString& operator += (int num) {
            this->concat(num);
            return (*this);
        }

        ERaString& operator += (unsigned int num) {
            this->concat(num);
            return (*this);
        }

        ERaString& operator += (long num) {
            this->concat(num);
            return (*this);
        }

        ERaString& operator += (unsigned long num) {
            this->concat(num);
            return (*this);
        }

        ERaString& operator += (long long num) {
            this->concat(num);
            return (*this);
        }

        ERaString& operator += (unsigned long long num) {
            this->concat(num);
            return (*this);
        }

        ERaString& operator += (float num) {
            this->concat(num);
            return (*this);
        }

        ERaString& operator += (double num) {
            this->concat(num);
            return (*this);
        }

        bool operator == (const ERaString& res) const {
            if (this == &res) {
                return true;
            }
            return operator == ((const char*)res.value);
        }

        bool operator == (const char* cstr) const {
            return this->StringCompare(cstr);
        }

        bool operator == (char* str) const {
            return operator == ((const char*)str);
        }

        bool operator != (const ERaString& res) const {
            if (this == &res) {
                return false;
            }
            return operator != ((const char*)res.value);
        }

        bool operator != (const char* cstr) const {
            return !this->StringCompare(cstr);
        }

        bool operator != (char* str) const {
            return operator != ((const char*)str);
        }

        friend ERaStringHelper& operator + (const ERaStringHelper& esh, const ERaString& res);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, const ERaParam& param);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, const ERaDataJson& rjs);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, const char* cstr);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, char* str);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, char c);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, unsigned char num);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, int num);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, unsigned int num);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, long num);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, unsigned long num);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, long long num);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, unsigned long long num);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, float num);
        friend ERaStringHelper& operator + (const ERaStringHelper& esh, double num);

    protected:
    private:
        void assign(const ERaString& estr) {
            this->assign((const char*)estr.value);
        }

        void assign(const ERaParam& param) {
            if (param.isNumber()) {
                this->assign(param.getFloat());
            }
            else if (param.isString()) {
                this->assign(param.getString());
            }
            else if (param.isObject()) {
                this->assign(param.getObject());
            }
        }

        void assign(const ERaDataJson& rjs) {
            if (!rjs.isValid()) {
                return;
            }
            this->assign(const_cast<ERaDataJson&>(rjs).getString());
        }

        void assign(const ERaDataJson* rjs) {
            if (rjs == nullptr) {
                return;
            }
            this->assign(*rjs);
        }

        void assign(const char* cstr) {
            bool changed {false};
            changed =  this->isNewString(cstr);
            changed |= this->isUpdated;
            this->clear();
            this->concat(cstr);
            this->isUpdated = changed;
        }

        void assign(char* str) {
            this->assign((const char*)str);
        }

        void assign(char c) {
            const char str[2] = { c, '\0' };
            this->assign(str);
        }

    #if defined(__AVR__) || defined(ARDUINO_ARCH_ARC32)

        void assign(unsigned char num) {
            char str[1 + 8 * sizeof(unsigned char)] {0};
            utoa(num, str, 10);
            this->assign((const char*)str);
        }

        void assign(int num) {
            char str[2 + 8 * sizeof(int)] {0};
            itoa(num, str, 10);
            this->assign((const char*)str);
        }

        void assign(unsigned int num) {
            char str[1 + 8 * sizeof(unsigned int)] {0};
            utoa(num, str, 10);
            this->assign((const char*)str);
        }

        void assign(long num) {
            char str[2 + 8 * sizeof(long)] {0};
            ltoa(num, str, 10);
            this->assign((const char*)str);
        }

        void assign(unsigned long num) {
            char str[1 + 8 * sizeof(unsigned long)] {0};
            ultoa(num, str, 10);
            this->assign((const char*)str);
        }

        void assign(long long num) {
            char str[2 + 8 * sizeof(long long)] {0};
            ltoa(num, str, 10);
            this->assign((const char*)str);
        }

        void assign(unsigned long long num) {
            char str[1 + 8 * sizeof(unsigned long long)] {0};
            ultoa(num, str, 10);
            this->assign((const char*)str);
        }

        void assign(float num) {
            char str[33] {0};
            dtostrf(num, 5, 2, str);
            this->assign((const char*)str);
        }

        void assign(double num) {
            char str[33] {0};
            dtostrf(num, 5, 5, str);
            this->assign((const char*)str);
        }

    #else

        void assign(unsigned char num) {
            char str[20] {0};
            snprintf(str, sizeof(str), "%u", num);
            this->assign((const char*)str);
        }

        void assign(int num) {
            char str[20] {0};
            snprintf(str, sizeof(str), "%i", num);
            this->assign((const char*)str);
        }

        void assign(unsigned int num) {
            char str[20] {0};
            snprintf(str, sizeof(str), "%u", num);
            this->assign((const char*)str);
        }

        void assign(long num) {
            char str[20] {0};
            snprintf(str, sizeof(str), "%li", num);
            this->assign((const char*)str);
        }

        void assign(unsigned long num) {
            char str[20] {0};
            snprintf(str, sizeof(str), "%lu", num);
            this->assign((const char*)str);
        }

        void assign(long long num) {
            char str[33] {0};
            snprintf(str, sizeof(str), "%lli", num);
            this->assign((const char*)str);
        }

        void assign(unsigned long long num) {
            char str[33] {0};
            snprintf(str, sizeof(str), "%llu", num);
            this->assign((const char*)str);
        }

    #if defined(ERA_USE_ERA_DTOSTRF)

        void assign(float num) {
            char str[33] {0};
            ERaDtostrf(num, 2, str);
            this->assign((const char*)str);
        }

        void assign(double num) {
            char str[33] {0};
            ERaDtostrf(num, 5, str);
            this->assign((const char*)str);
        }

    #else

        void assign(float num) {
            char str[33] {0};
            snprintf(str, sizeof(str), "%.2f", num);
            this->assign((const char*)str);
        }

        void assign(double num) {
            char str[33] {0};
            snprintf(str, sizeof(str), "%.5f", num);
            this->assign((const char*)str);
        }

    #endif

    #endif

        bool updated() {
            bool ret = this->isUpdated;
            this->isUpdated = false;
            return ret;
        }

        void setString(const char* cstr) {
            if (!this->isNewString(cstr)) {
                return;
            }
            this->updateString(cstr);
        }

        void updateString(const char* cstr) {
            if (cstr == nullptr) {
                return;
            }
            if (this->value == nullptr) {
                this->value = this->stringdup(cstr);
                if (this->value != nullptr) {
                    this->isUpdated = true;
                    this->valueCapacity = strlen(this->value) + 1;
                }
                return;
            }
            size_t len = strlen(cstr);
            if (len > CapacityMax) {
                return;
            }
            if (len >= this->valueCapacity) {
                char* copy = (char*)realloc(this->value, len + 1);
                if (copy == nullptr) {
                    return;
                }
                snprintf(copy, len + 1, cstr);
                this->value = copy;
                this->valueCapacity = len + 1;
            }
            else {
                snprintf(this->value, this->valueCapacity, cstr);
            }
            this->value[len] = '\0';
            this->isUpdated = true;
        }

        bool isNewString(const char* cstr) const {
            if (cstr == nullptr) {
                return false;
            }
            if (this->value == nullptr) {
                return true;
            }
            return strcmp(this->value, cstr);
        }

        bool StringCompare(const char* cstr) const {
            if (this->value == cstr) {
                return true;
            }
            if (cstr == nullptr) {
                return false;
            }
            if (this->value == nullptr) {
                return false;
            }
            return !strcmp((const char*)this->value, cstr);
        }

        char* stringdup(const char* cstr) {
            if (cstr == nullptr) {
                return nullptr;
            }

            size_t length {0};
            char* copy = nullptr;

            length = strlen(cstr) + sizeof("");
            if (length > CapacityMax) {
                return nullptr;
            }
            copy = (char*)malloc(length);
            if (copy == nullptr) {
                return nullptr;
            }
            memcpy(copy, cstr, length);

            return copy;
        }

        char* value;
        size_t valueCapacity;
        bool isUpdated;
    };

    class ERaStringHelper
        : public ERaString
    {
    public:
        ERaStringHelper()
            : ERaString()
        {}
        ERaStringHelper(const ERaString& estr)
            : ERaString(estr)
        {}
        ERaStringHelper(const ERaParam& param)
            : ERaString(param)
        {}
        ERaStringHelper(const ERaDataJson& rjs)
            : ERaString(rjs)
        {}
        ERaStringHelper(const char* cstr)
            : ERaString(cstr)
        {}
        ERaStringHelper(char* str)
            : ERaString(str)
        {}
        ERaStringHelper(char c)
            : ERaString(c)
        {}
        ERaStringHelper(unsigned char num)
            : ERaString(num)
        {}
        ERaStringHelper(int num)
            : ERaString(num)
        {}
        ERaStringHelper(unsigned int num)
            : ERaString(num)
        {}
        ERaStringHelper(long num)
            : ERaString(num)
        {}
        ERaStringHelper(unsigned long num)
            : ERaString(num)
        {}
        ERaStringHelper(long long num)
            : ERaString(num)
        {}
        ERaStringHelper(unsigned long long num)
            : ERaString(num)
        {}
        ERaStringHelper(float num)
            : ERaString(num)
        {}
        ERaStringHelper(double num)
            : ERaString(num)
        {}
    };

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, const ERaString& res) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(res);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, const ERaParam& param) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(param);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, const ERaDataJson& rjs) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(rjs);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, const char* cstr) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(cstr);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, char* str) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(str);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, char c) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(c);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, unsigned char num) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(num);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, int num) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(num);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, unsigned int num) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(num);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, long num) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(num);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, unsigned long num) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(num);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, long long num) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(num);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, unsigned long long num) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(num);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, float num) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(num);
        return a;
    }

    inline
    ERaStringHelper& operator + (const ERaStringHelper& esh, double num) {
        ERaStringHelper& a = const_cast<ERaStringHelper&>(esh);
        a.concat(num);
        return a;
    }

#endif

class WrapperString
    : public WrapperBase
{
public:
    WrapperString(ERaString& estr)
        : value(estr)
#if defined(ERA_ARDUINO_STRING)
        , local()
#endif
        , options(0)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_STRING;
    }
    ~WrapperString() override
    {
        if (!this->options) {
            return;
        }
        this->value.~ERaString();
    }

    WrapperString& operator = (const ERaString& estr) {
        this->value = estr;
        return (*this);
    }

#if defined(ERA_ARDUINO_STRING)
    bool updated() override {
        return this->value.updated(this->local);
    }
#else
    bool updated() override {
        return this->value.updated();
    }
#endif

    void setOptions(int option) override {
        this->options = option;
    }

protected:
    double get() const override {
        return 0.0f;
    }

    void set(double num) override {
        (void)num;
    }

    const void* getPointer() const override {
        return (const void*)this->value.getString();
    }

    void setPointer(const void* cptr) override {
        this->value.setString((const char*)cptr);
    }

private:
    ERaString& value;
#if defined(ERA_ARDUINO_STRING)
    ERaString  local;
#endif
    int options;
};

#endif /* INC_WRAPPER_STRING_HPP_ */
