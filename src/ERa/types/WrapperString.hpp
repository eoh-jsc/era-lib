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

#if defined(ESP32) || defined(ESP8266)

        ERaString(long long num)
            : String(num)
        {}
        ERaString(unsigned long long num)
            : String(num)
        {}

#elif !defined(__AVR__)

        ERaString(long long num)
            : String((long)num)
        {}
        ERaString(unsigned long long num)
            : String((unsigned long)num)
        {}

#endif

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

        int indexOfIgnoreCase(char c) const {
            return this->indexOfIgnoreCase(c, 0);
        }

        int indexOfIgnoreCase(char c, size_t index) const {
            return this->indexOfCaseSensitive(c, index, false);
        }

        int indexOfCaseSensitive(char c, size_t index, bool caseSensitive) const {
            if (index >= String::len()) {
                return -1;
            }
            const char* ptr = ERaStrChr(String::buffer() + index, c, caseSensitive);
            if (ptr == nullptr) {
                return -1;
            }
            return (int)(ptr - String::buffer());
        }

        int indexOfIgnoreCase(const ERaString& str) const {
            return this->indexOfIgnoreCase(str, 0);
        }

        int indexOfIgnoreCase(const ERaString& str, size_t index) const {
            return this->indexOfCaseSensitive(str, index, false);
        }

        int indexOfCaseSensitive(const ERaString& str, size_t index, bool caseSensitive) const {
            if (!str.length()) {
                return -1;
            }
            if (index >= String::len()) {
                return -1;
            }
            const char* ptr = ERaStrStr(String::buffer() + index, str.buffer(), caseSensitive);
            if (ptr == nullptr) {
                return -1;
            }
            return (int)(ptr - String::buffer());
        }

        int lastIndexOfIgnoreCase(char c) const {
            return this->lastIndexOfIgnoreCase(c, this->length() - 1);
        }

        int lastIndexOfIgnoreCase(char c, size_t index) const {
            return this->lastIndexOfCaseSensitive(c, index, false);
        }

        int lastIndexOfCaseSensitive(char c, size_t index, bool caseSensitive) const {
            if (index >= String::len()) {
                return -1;
            }
            char temp = String::buffer()[index + 1];
            String::wbuffer()[index + 1] = '\0';
            char* ptr = ERaStrrChr(String::wbuffer(), c, caseSensitive);
            String::wbuffer()[index + 1] = temp;
            if (ptr == nullptr) {
                return -1;
            }
            return (int)(ptr - String::buffer());
        }

        int lastIndexOfIgnoreCase(const ERaString& str) const {
            return this->lastIndexOfIgnoreCase(str, this->length() - str.length());
        }

        int lastIndexOfIgnoreCase(const ERaString& str, size_t index) const {
            return this->lastIndexOfCaseSensitive(str, index, false);
        }

        int lastIndexOfCaseSensitive(const ERaString& str, size_t index, bool caseSensitive) const {
            if (!str.len() || !String::len()) {
                return -1;
            }
            if (str.len() > String::len()) {
                return -1;
            }
            if (index >= String::len()) {
                index = (String::len() - 1);
            }
            int found = -1;
            for (char* ptr = String::wbuffer(); ptr <= String::wbuffer() + index; ++ptr) {
                ptr = ERaStrStr(ptr, str.buffer(), caseSensitive);
                if (ptr == nullptr) {
                    break;
                }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
                if ((ptr - String::wbuffer()) <= index) {
                    found = (ptr - String::buffer());
                }
#pragma GCC diagnostic pop
            }
            return found;
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

        typedef void (ERaString::*ERaStringIfHelperType)(void) const;
        void ERaStringIfHelper() const {}

    public:
        ERaString()
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {}
        ERaString(const ERaString& estr)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(estr);
        }
        ERaString(const ERaParam& param)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(param);
        }
        ERaString(const ERaDataJson& rjs)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(rjs);
        }
        ERaString(const char* cstr)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(cstr);
        }
        ERaString(char* str)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(str);
        }
        ERaString(char c)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(c);
        }
        ERaString(unsigned char num)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(int num)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(unsigned int num)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(long num)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(unsigned long num)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(long long num)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(unsigned long long num)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(float num)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(num);
        }
        ERaString(double num)
            : value {
                .buffer = nullptr,
                .capacity = 0UL,
                .length = 0UL
            }
            , isUpdated(false)
        {
            this->concat(num);
        }
        ~ERaString()
        {
            this->invalidate();
        }

        void invalidate() {
            if (this->value.buffer != nullptr) {
                free(this->value.buffer);
            }
            this->value.buffer = nullptr;
            this->value.capacity = 0UL;
            this->value.length = 0UL;
            this->isUpdated = false;
        }

        bool reserve(size_t cap) {
            if (cap > CapacityMax) {
                return false;
            }
            if (cap < this->value.capacity) {
                return true;
            }
            char* copy = (char*)realloc(this->value.buffer, cap + 1);
            if (copy != nullptr) {
                if (this->value.buffer == nullptr) {
                    memset(copy, 0, cap + 1);
                }
                this->value.buffer = copy;
                this->value.buffer[cap] = '\0';
                this->value.capacity = (cap + 1);
                return true;
            }
            else {
                this->invalidate();
                return false;
            }
        }

        const char* c_str() const {
            return ((this->value.buffer != nullptr) ? this->value.buffer : "");
        }

        const char* getString() const {
            return ((this->value.buffer != nullptr) ? this->value.buffer : "");
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
            this->concat((const char*)estr.value.buffer);
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
            if (this->value.buffer == nullptr) {
                return this->setString(cstr);
            }
            if (!strlen(cstr)) {
                return;
            }
            size_t oldLen = strlen(this->value.buffer);
            size_t newLen = oldLen;
            newLen += strlen(cstr);
            if (newLen > CapacityMax) {
                return;
            }
            if (newLen >= this->value.capacity) {
                char* copy = (char*)realloc(this->value.buffer, newLen + 1);
                if (copy == nullptr) {
                    return;
                }
                this->value.buffer = copy;
                this->value.capacity = (newLen + 1);
            }
            snprintf(this->value.buffer + oldLen, newLen - oldLen + 1, "%s", cstr);
            this->value.buffer[newLen] = '\0';
            this->value.length = newLen;
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
            ERaLltoa(num, str, sizeof(str), 10);
            this->concat((const char*)str);
        }

        void concat(unsigned long long num) {
            char str[1 + 8 * sizeof(unsigned long long)] {0};
            ERaUlltoa(num, str, sizeof(str), 10);
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
            return this->value.length;
        }

        size_t length() const {
            return this->value.length;
        }

        char& at(size_t index) {
            if (this->value.buffer == nullptr) {
                static char c {0};
                return c;
            }
            return this->value.buffer[index];
        }

        void toLowerCase() {
            size_t len = this->length();
            for (size_t i = 0; i < len; ++i) {
                this->value.buffer[i] = ::tolower(this->value.buffer[i]);
            }
        }

        void toUpperCase() {
            size_t len = this->length();
            for (size_t i = 0; i < len; ++i) {
                this->value.buffer[i] = ::toupper(this->value.buffer[i]);
            }
        }

        void trim() {
            if (!this->length()) {
                return;
            }
            char* begin = this->value.buffer;
            while (isspace(*begin)) {
                begin++;
            }
            char* end = (this->value.buffer + this->length() - 1);
            while (isspace(*end) && (end >= begin)) {
                end--;
            }
            unsigned int newLen = (end - begin + 1);
            if (begin > this->value.buffer) {
                this->isUpdated = true;
                memmove(this->value.buffer, begin, newLen);
            }
            this->value.buffer[newLen] = '\0';
            this->value.length = newLen;
        }

        int indexOf(char c) const {
            return this->indexOf(c, 0);
        }

        int indexOfIgnoreCase(char c) const {
            return this->indexOfIgnoreCase(c, 0);
        }

        int indexOf(char c, size_t index) const {
            return this->indexOfCaseSensitive(c, index, true);
        }

        int indexOfIgnoreCase(char c, size_t index) const {
            return this->indexOfCaseSensitive(c, index, false);
        }

        int indexOfCaseSensitive(char c, size_t index, bool caseSensitive) const {
            if (index >= this->length()) {
                return -1;
            }
            const char* ptr = ERaStrChr(this->value.buffer + index, c, caseSensitive);
            if (ptr == nullptr) {
                return -1;
            }
            return (int)(ptr - this->value.buffer);
        }

        int indexOf(const ERaString& str) const {
            return this->indexOf(str, 0);
        }

        int indexOfIgnoreCase(const ERaString& str) const {
            return this->indexOfIgnoreCase(str, 0);
        }

        int indexOf(const ERaString& str, size_t index) const {
            return this->indexOfCaseSensitive(str, index, true);
        }

        int indexOfIgnoreCase(const ERaString& str, size_t index) const {
            return this->indexOfCaseSensitive(str, index, false);
        }

        int indexOfCaseSensitive(const ERaString& str, size_t index, bool caseSensitive) const {
            if (!str.length()) {
                return -1;
            }
            if (index >= this->length()) {
                return -1;
            }
            const char* ptr = ERaStrStr(this->value.buffer + index, str.value.buffer, caseSensitive);
            if (ptr == nullptr) {
                return -1;
            }
            return (int)(ptr - this->value.buffer);
        }

        int lastIndexOf(char c) const {
            return this->lastIndexOf(c, this->length() - 1);
        }

        int lastIndexOfIgnoreCase(char c) const {
            return this->lastIndexOfIgnoreCase(c, this->length() - 1);
        }

        int lastIndexOf(char c, size_t index) const {
            return this->lastIndexOfCaseSensitive(c, index, true);
        }

        int lastIndexOfIgnoreCase(char c, size_t index) const {
            return this->lastIndexOfCaseSensitive(c, index, false);
        }

        int lastIndexOfCaseSensitive(char c, size_t index, bool caseSensitive) const {
            if (index >= this->length()) {
                return -1;
            }
            const char temp = this->value.buffer[index + 1];
            this->value.buffer[index + 1] = '\0';
            const char* ptr = ERaStrrChr(this->value.buffer, c, caseSensitive);
            this->value.buffer[index + 1] = temp;
            if (ptr == nullptr) {
                return -1;
            }
            return (int)(ptr - this->value.buffer);
        }

        int lastIndexOf(const ERaString& str) const {
            return this->lastIndexOf(str, this->length() - str.length());
        }

        int lastIndexOfIgnoreCase(const ERaString& str) const {
            return this->lastIndexOfIgnoreCase(str, this->length() - str.length());
        }

        int lastIndexOf(const ERaString& str, size_t index) const {
            return this->lastIndexOfCaseSensitive(str, index, true);
        }

        int lastIndexOfIgnoreCase(const ERaString& str, size_t index) const {
            return this->lastIndexOfCaseSensitive(str, index, false);
        }

        int lastIndexOfCaseSensitive(const ERaString& str, size_t index, bool caseSensitive) const {
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
            for (char* ptr = this->value.buffer; ptr <= (this->value.buffer + index); ++ptr) {
                ptr = ERaStrStr(ptr, str.value.buffer, caseSensitive);
                if (ptr == nullptr) {
                    break;
                }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
                if ((ptr - this->value.buffer) <= index) {
                    found = (ptr - this->value.buffer);
                }
#pragma GCC diagnostic pop
            }
            return found;
        }

        void replace(char find, char replace) {
            size_t len = this->length();
            for (size_t i = 0; i < len; ++i) {
                if (this->value.buffer[i] != find) {
                    continue;
                }
                this->value.buffer[i] = replace;
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
            char* readFrom = this->value.buffer;
            if (!diff) {
                while ((foundAt = strstr(readFrom, find.value.buffer)) != nullptr) {
                    memmove(foundAt, replace.value.buffer, replace.length());
                    readFrom = (foundAt + find.length());
                }
            }
            else if (diff < 0) {
                size_t len = this->length();
                char* writeTo = this->value.buffer;
                while ((foundAt = strstr(readFrom, find.value.buffer)) != nullptr) {
                    size_t n = (foundAt - readFrom);
                    memmove(writeTo, readFrom, n);
                    writeTo += n;
                    memmove(writeTo, replace.value.buffer, replace.length());
                    writeTo += replace.length();
                    readFrom = (foundAt + find.length());
                    len += diff;
                }
                memmove(writeTo, readFrom, strlen(readFrom) + 1);
                this->value.length = len;
            }
            else {
                size_t size = this->length();
                while ((foundAt = strstr(readFrom, find.value.buffer)) != nullptr) {
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
                if (this->value.buffer == nullptr) {
                    return;
                }
                int index = (this->length() - 1);
                while ((index >= 0) && ((index = this->lastIndexOf(find, index)) >= 0)) {
                    readFrom = (this->value.buffer + index + find.length());
                    memmove(readFrom + diff, readFrom, this->length() - (readFrom - this->value.buffer));
                    size_t newLen = (this->length() + diff);
                    memmove(this->value.buffer + index, replace.value.buffer, replace.length());
                    this->value.buffer[newLen] = '\0';
                    this->value.length = newLen;
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
            char* writeTo = (this->value.buffer + index);
            size_t newLen = (this->length() - count);
            memmove(writeTo, this->value.buffer + index + count, newLen - index);
            this->value.buffer[newLen] = '\0';
            this->value.length = newLen;
        }

        ERaString substring(size_t beginIndex) const {
            return this->substring(beginIndex, this->length());
        }

        ERaString substring(size_t beginIndex, size_t endIndex) const {
            ERaString out;
            if (beginIndex >= endIndex) {
                return out;
            }
            if (beginIndex >= this->length()) {
                return out;
            }
            if (endIndex > this->length()) {
                endIndex = this->length();
            }
            char temp = this->value.buffer[endIndex];
            this->value.buffer[endIndex] = '\0';
            out.assign(this->value.buffer + beginIndex);
            this->value.buffer[endIndex] = temp;
            return out;
        }

        int toInt() const {
            if (this->value.buffer == nullptr) {
                return 0;
            }
            return atol(this->value.buffer);
        }

        float toFloat() const {
            if (this->value.buffer == nullptr) {
                return 0.0f;
            }
            return (float)atof(this->value.buffer);
        }

        double toDouble() const {
            if (this->value.buffer == nullptr) {
                return 0.0;
            }
            return atof(this->value.buffer);
        }

        ERaDataJson toJSON() const {
            return ERaDataJson(this->c_str());
        }

        void clear() {
            if (this->value.buffer == nullptr) {
                return;
            }
            memset(this->value.buffer, 0, this->value.capacity);
            this->value.length = 0UL;
        }

        bool startsWith(const ERaString& res) const {
            return this->startsWith((const char*)res.value.buffer);
        }

        bool startsWith(const ERaString& res, size_t offset) const {
            return this->startsWith((const char*)res.value.buffer, offset);
        }

        bool startsWith(const char* cstr) const {
            return this->startsWith(cstr, 0);
        }

        bool startsWith(const char* cstr, size_t offset) const {
            if (cstr == nullptr) {
                return false;
            }
            if (this->value.buffer == nullptr) {
                return false;
            }
            if (offset > this->length()) {
                return false;
            }
            if (offset > (this->length() - strlen(cstr))) {
                return false;
            }
            return !strncmp(&this->value.buffer[offset], cstr, strlen(cstr));
        }

        bool endsWith(const ERaString& res) const {
            return this->endsWith((const char*)res.value.buffer);
        }

        bool endsWith(const char* cstr) const {
            if (cstr == nullptr) {
                return false;
            }
            if (this->value.buffer == nullptr) {
                return false;
            }
            if (this->length() < strlen(cstr)) {
                return false;
            }
            size_t len = (this->length() - strlen(cstr));
            return !strcmp(&this->value.buffer[len], cstr);
        }

        int compareTo(const ERaString& res) const {
            if (this == &res) {
                return true;
            }
            if ((this->value.buffer == nullptr) ||
                (res.value.buffer == nullptr)) {
                if ((this->value.buffer != nullptr) && (this->value.length > 0)) {
                    return (*(unsigned char*)this->value.buffer);
                }
                if ((res.value.buffer != nullptr) && (res.value.length > 0)) {
                    return (0 - (*(unsigned char*)res.value.buffer));
                }
                return 0;
            }
            return strcmp(this->value.buffer, res.value.buffer);
        }

        bool equals(const ERaString& res) const {
            if (this == &res) {
                return true;
            }
            return this->equals((const char*)res.value.buffer);
        }

        bool equals(const char* cstr) const {
            return this->StringCompare(cstr);
        }

        bool equalsIgnoreCase(const ERaString& res) const  {
            if (this == &res) {
                return true;
            }
            if (this->length() != res.length()) {
                return false;
            }
            if (!this->length()) {
                return true;
            }
            const char* str1 = this->value.buffer;
            const char* str2 = res.value.buffer;
            while (*str1) {
                if (::tolower(*str1++) != ::tolower(*str2++)) {
                    return false;
                }
            }
            return true;
        }

        char* begin() {
            return this->value.buffer;
        }

        char* end() {
            return (this->value.buffer + this->value.length);
        }

        const char* begin() const {
            return this->value.buffer;
        }

        const char* end() const {
            return (this->value.buffer + this->value.length);
        }

        operator const char*() const {
            if (this->value.buffer == nullptr) {
                return "";
            }
            return this->value.buffer;
        }

        ERaStringIfHelperType operator() (void) const {
            if (this->value.buffer == nullptr) {
                return nullptr;
            }
            return &ERaString::ERaStringIfHelper;
        }

        char& operator [] (int index) {
            return this->at(index);
        }

    #if defined(__GXX_EXPERIMENTAL_CXX0X__)
        ERaString& operator = (ERaString&& res) {
            if (this != &res) {
                this->move(res);
            }
            return (*this);
        }

        ERaString& operator = (ERaStringHelper&& res);
    #endif

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
            return operator == ((const char*)res.value.buffer);
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
            return operator != ((const char*)res.value.buffer);
        }

        bool operator != (const char* cstr) const {
            return !this->StringCompare(cstr);
        }

        bool operator != (char* str) const {
            return operator != ((const char*)str);
        }

        bool operator < (const ERaString& res) const {
            return (this->compareTo(res) < 0);
        }

        bool operator > (const ERaString& res) const {
            return (this->compareTo(res) > 0);
        }

        bool operator <= (const ERaString& res) const {
            return (this->compareTo(res) <= 0);
        }

        bool operator >= (const ERaString& res) const {
            return (this->compareTo(res) >= 0);
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
    #if defined(__GXX_EXPERIMENTAL_CXX0X__)
        void move(ERaString& estr) {
            bool changed {false};
            changed =  this->isNewString((const char*)estr.value.buffer);
            changed |= this->isUpdated;

            if (this->value.buffer != nullptr) {
                if ((estr.value.buffer != nullptr) &&
                    (this->value.capacity > estr.value.length)) {
                    strcpy(this->value.buffer, estr.value.buffer);
                    this->value.buffer[estr.value.length] = '\0';
                    this->value.length = estr.value.length;
                    estr.value.length = 0;
                    this->isUpdated = changed;
                    return;
                }
                else {
                    this->invalidate();
                }
            }
            this->value.buffer = estr.value.buffer;
            this->value.capacity = estr.value.capacity;
            this->value.length = estr.value.length;
            estr.value.buffer = nullptr;
            estr.value.capacity = 0UL;
            estr.value.length = 0UL;
            this->isUpdated = changed;
        }
    #endif

        void assign(const ERaString& estr) {
            this->assign((const char*)estr.value.buffer);
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
            ERaLltoa(num, str, sizeof(str), 10);
            this->assign((const char*)str);
        }

        void assign(unsigned long long num) {
            char str[1 + 8 * sizeof(unsigned long long)] {0};
            ERaUlltoa(num, str, sizeof(str), 10);
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
            if (this->value.buffer == nullptr) {
                this->value.buffer = this->stringdup(cstr);
                if (this->value.buffer != nullptr) {
                    this->isUpdated = true;
                    this->value.length = strlen(this->value.buffer);
                    this->value.capacity = (this->value.length + 1);
                }
                return;
            }
            size_t len = strlen(cstr);
            if (len > CapacityMax) {
                return;
            }
            if (len >= this->value.capacity) {
                char* copy = (char*)realloc(this->value.buffer, len + 1);
                if (copy == nullptr) {
                    return;
                }
                snprintf(copy, len + 1, "%s", cstr);
                this->value.buffer = copy;
                this->value.capacity = (len + 1);
            }
            else {
                snprintf(this->value.buffer, this->value.capacity, "%s", cstr);
            }
            this->value.buffer[len] = '\0';
            this->value.length = len;
            this->isUpdated = true;
        }

        bool isNewString(const char* cstr) const {
            if (cstr == nullptr) {
                return false;
            }
            if (this->value.buffer == nullptr) {
                return true;
            }
            return strcmp(this->value.buffer, cstr);
        }

        bool StringCompare(const char* cstr) const {
            return ERaStringCompare((const char*)this->value.buffer, cstr);
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

        struct {
            char* buffer;
            size_t capacity;
            size_t length;
        } value;

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

#if defined(__GXX_EXPERIMENTAL_CXX0X__)
    inline
    ERaString& ERaString::operator = (ERaStringHelper&& res) {
        if (this != &res) {
            this->move(res);
        }
        return (*this);
    }
#endif

#endif

inline
int WrapperBase::CompareTo(const char* cstr) const {
    return ERaString(this->getString()).compareTo(cstr);
}

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
