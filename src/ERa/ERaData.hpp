#ifndef INC_ERA_DATA_HPP_
#define INC_ERA_DATA_HPP_

#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ERa/ERaDefine.hpp>

class ERaParam;
class ERaDataJson;

class ERaDataBuff
{
public:
    class iterator
    {
    public:
        iterator()
            : ptr(nullptr)
            , limit(nullptr)
        {}
        iterator(char* str, char* estr)
            : ptr(str)
            , limit(estr)
        {}
        ~iterator()
        {}

        static iterator invalid() {
            return iterator(nullptr, nullptr);
        }

        const char* getString() const {
            return this->ptr;
        }

        int getInt() const {
            if (!this->isValid()) {
                return 0;
            }
            return atoi(this->ptr);
        }

        int getInt(int base) const {
            return (int)this->getLong(base);
        }

        long getLong() const {
            if (!this->isValid()) {
                return 0;
            }
            return atol(this->ptr);
        }

        long getLong(int base) const {
            if (!this->isValid()) {
                return 0;
            }
            return strtol(this->ptr, nullptr, base);
        }

#if defined(ERA_USE_ERA_ATOLL)
        long long getLongLong() const {
            if (!this->isValid()) {
                return 0;
            }
            return ERaAtoll(this->ptr);
        }
#else
        long long getLongLong() const {
            if (!this->isValid()) {
                return 0;
            }
            return atoll(this->ptr);
        }

        long long getLongLong(int base) const {
            if (!this->isValid()) {
                return 0;
            }
            return strtoll(this->ptr, nullptr, base);
        }
#endif

        float getFloat() const {
            if (!this->isValid()) {
                return 0.0f;
            }
            return (float)atof(this->ptr);
        }

        double getDouble() const {
            if (!this->isValid()) {
                return 0.0;
            }
            return atof(this->ptr);
        }

        ERaDataJson toJSON() const;

        bool isValid() const {
            return ((this->ptr != nullptr) && (this->ptr < this->limit));
        }

        bool isEmpty() const {
            if (!this->isValid()) {
                return true;
            }
            return (*this->ptr == '\0');
        }

        operator const char* () const {
            return this->getString();
        }

        bool operator < (const iterator& it) {
            return this->ptr < it.ptr;
        }

        bool operator >= (const iterator& it) {
            return this->ptr >= it.ptr;
        }

        bool operator == (const char* cstr) {
            if (!this->isValid()) {
                return false;
            }
            if (cstr == nullptr) {
                return false;
            }
            return !strcmp(this->ptr, cstr);
        }

        bool operator != (const char* cstr) {
            if (!this->isValid()) {
                return false;
            }
            if (cstr == nullptr) {
                return true;
            }
            return strcmp(this->ptr, cstr);
        }

        iterator& operator ++ () {
            if (this->isValid()) {
                this->ptr += strlen(this->ptr) + 1;
            }
            return (*this);
        }

    private:
        const char* ptr;
        const char* limit;
    };

    ERaDataBuff(const void* cptr, size_t length)
        : buff((char*)cptr)
        , len(length)
        , buffSize(length)
        , dataLen(length)
        , changed(false)
    {}
    ERaDataBuff(void* ptr, size_t length, size_t bsize)
        : buff((char*)ptr)
        , len(length)
        , buffSize(bsize)
        , dataLen(length)
        , changed(false)
    {}
    ~ERaDataBuff()
    {}

    const char* c_str() const {
        return this->buff;
    }

    const char* getString() const {
        return this->buff;
    }

    bool isValid() const {
        return this->buff != nullptr;
    }

    bool isEmpty() const {
        return (*this->buff == '\0');
    }

    bool isChange() {
        bool ret = this->changed;
        this->changed = false;
        return ret;
    }

    void clear() {
        this->len = 0;
    }

    void clearBuffer() {
        this->len = 0;
        memset(this->buff, 0, this->buffSize);
    }

    void add(const void* ptr, size_t size);

    void add(const char* ptr) {
        this->add(ptr, strlen(ptr));
    }

    void add_hex(uint8_t value);
    void add_hex_array(const uint8_t* ptr, size_t size);
    void add_zero_array(size_t size);
    void add_on_change(const char* value);
    void add(int value);
    void add(unsigned int value);
    void add(long value);
    void add(unsigned long value);
    void add(long long value);
    void add(unsigned long long value);
    void add(float value);
    void add(double value);

#if defined(ERA_HAS_PROGMEM)
    void add(const __FlashStringHelper* ptr);
#endif

    template <typename T> 
    void add_multi(const T last) {
        this->add(last);
    }

    template <typename T, typename... Args> 
    void add_multi(const T head, Args... tail) {
        this->add(head);
        this->add_multi(tail...);
    }

    bool next();
    bool next(size_t pDataLen);
    void remove(size_t index);
    void remove(const char* key);

    void done() {
        size_t index {0};
        this->dataLen = this->len;
        index = (this->buffSize - 1);
        index = ERaMin(this->len, index);
        this->buff[index] = '\0';
    }

    const char* getBuffer() const {
        return this->buff;
    }

    size_t getLen() const {
        return this->len;
    }

    size_t getDataLen() const {
        return this->dataLen;
    }

    iterator begin() const {
        return iterator(this->buff, this->buff + this->len);
    }

    iterator end() const {
        return iterator(this->buff + this->len, this->buff + this->len);
    }

    iterator begin(size_t pos) const {
        return iterator(this->buff, this->buff + pos);
    }

    iterator end(size_t pos) const {
        return iterator(this->buff + pos, this->buff + pos);
    }

    iterator at(int index) const;
    iterator at(const char* key) const;

    size_t size() const;
    size_t split(const char* delims);

    operator char* () const;

    void operator += (const char* ptr) {
        this->add(ptr);
    }

    void operator += (uint8_t value) {
        this->add(value);
    }

    iterator operator [] (int index) const;
    iterator operator [] (const char* key) const;

protected:
    void onChange(const void* ptr, size_t size);
    void onChange(const char* value);
    void onChangeHex(uint8_t value);
    void onChangeHex(const uint8_t* ptr, size_t size);

    size_t unescape();

    char* buff;
    size_t len;
    size_t buffSize;
    size_t dataLen;
    bool changed;
};

inline
void ERaDataBuff::add(const void* ptr, size_t size) {
    if (ptr == nullptr) {
        return;
    }
    if ((this->len + size) >= this->buffSize) {
        return;
    }
    memcpy(this->buff + this->len, ptr, size);
    this->len += size;
    this->buff[this->len++] = '\0';
}

inline
void ERaDataBuff::add_hex(uint8_t value) {
    if ((this->len + 2) >= this->buffSize) {
        return;
    }
    this->onChangeHex(value);
    this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%02x", value);
    this->buff[this->len++] = '\0';
}

inline
void ERaDataBuff::add_hex_array(const uint8_t* ptr, size_t size) {
    if (ptr == nullptr || !size) {
        return;
    }
    if ((this->len + (size * 2)) >= this->buffSize) {
        return;
    }
    this->onChangeHex(ptr, size);
    for (size_t i = 0; i < size; ++i) {
        this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%02x", ptr[i]);
    }
    this->buff[this->len++] = '\0';
}

inline
void ERaDataBuff::add_zero_array(size_t size) {
    if (!size) {
        return;
    }
    if ((this->len + (size * 2)) >= this->buffSize) {
        return;
    }
    for (size_t i = 0; i < size; ++i) {
        this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%02x", 0);
    }
    this->buff[this->len++] = '\0';
}

inline
void ERaDataBuff::add_on_change(const char* value) {
    this->onChange(value);
    this->add(value);
}

#if defined(__AVR__) || defined(ARDUINO_ARCH_ARC32)

    #include <stdlib.h>

    inline
    void ERaDataBuff::add(int value) {
        char str[2 + 8 * sizeof(int)] {0};
        itoa(value, str, 10);
        this->add(str);
    }

    inline
    void ERaDataBuff::add(unsigned int value) {
        char str[1 + 8 * sizeof(unsigned int)] {0};
        utoa(value, str, 10);
        this->add(str);
    }

    inline
    void ERaDataBuff::add(long value) {
        char str[2 + 8 * sizeof(long)] {0};
        ltoa(value, str, 10);
        this->add(str);
    }

    inline
    void ERaDataBuff::add(unsigned long value) {
        char str[1 + 8 * sizeof(unsigned long)] {0};
        ultoa(value, str, 10);
        this->add(str);
    }

    inline
    void ERaDataBuff::add(long long value) {
        char str[2 + 8 * sizeof(long long)] {0};
        ltoa(value, str, 10);
        this->add(str);
    }

    inline
    void ERaDataBuff::add(unsigned long long value) {
        char str[1 + 8 * sizeof(unsigned long long)] {0};
        ultoa(value, str, 10);
        this->add(str);
    }

    inline
    void ERaDataBuff::add(float value) {
        char str[33] {0};
        dtostrf(value, 5, 2, str);
        this->add(str);
    }

    inline
    void ERaDataBuff::add(double value) {
        char str[33] {0};
        dtostrf(value, 5, 5, str);
        this->add(str);
    }

#else

    inline
    void ERaDataBuff::add(int value) {
        this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%i", value);
        this->buff[this->len++] = '\0';
    }

    inline
    void ERaDataBuff::add(unsigned int value) {
        this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%u", value);
        this->buff[this->len++] = '\0';
    }

    inline
    void ERaDataBuff::add(long value) {
        this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%li", value);
        this->buff[this->len++] = '\0';
    }

    inline
    void ERaDataBuff::add(unsigned long value) {
        this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%lu", value);
        this->buff[this->len++] = '\0';
    }

    inline
    void ERaDataBuff::add(long long value) {
        this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%lli", value);
        this->buff[this->len++] = '\0';
    }

    inline
    void ERaDataBuff::add(unsigned long long value) {
        this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%llu", value);
        this->buff[this->len++] = '\0';
    }

#if defined(ERA_USE_ERA_DTOSTRF)

    inline
    void ERaDataBuff::add(float value) {
        char str[33] {0};
        ERaDtostrf(value, 2, str);
        this->add(str);
    }

    inline
    void ERaDataBuff::add(double value) {
        char str[33] {0};
        ERaDtostrf(value, 5, str);
        this->add(str);
    }

#else

    inline
    void ERaDataBuff::add(float value) {
        this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%.2f", value);
        this->buff[this->len++] = '\0';
    }

    inline
    void ERaDataBuff::add(double value) {
        this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%.5f", value);
        this->buff[this->len++] = '\0';
    }

#endif

#endif

#if defined(ERA_HAS_PROGMEM)

    inline
    void ERaDataBuff::add(const __FlashStringHelper* ptr) {
        if (ptr == nullptr) {
            return;
        }
        PGM_P p = reinterpret_cast<PGM_P>(ptr);
        size_t size = strlen_P(p);
        if ((this->len + size) >= this->buffSize) {
            return;
        }
        memcpy_P(this->buff + this->len, p, size);
        this->len += size;
        this->buff[this->len++] = '\0';
    }

#endif

inline
bool ERaDataBuff::next() {
    char* ptr = this->buff + this->len;
    if (*ptr != '\0') {
        this->len += strlen(ptr);
        this->buff[this->len++] = '\0';
        return true;
    }
    return false;
}

inline
bool ERaDataBuff::next(size_t pDataLen) {
    pDataLen *= 2;
    char* ptr = this->buff + this->len;
    if (*ptr != '\0') {
        size_t size = strlen(ptr);
        if (size < pDataLen) {
            return false;
        }
        pDataLen = ERaMin(size, pDataLen);
        this->len += pDataLen;
        this->buff[this->len++] = '\0';
        return true;
    }
    return false;
}

inline
void ERaDataBuff::onChange(const void* ptr, size_t size) {
    if (ptr == nullptr) {
        return;
    }
    if (this->buff[this->len] == '\0') {
        this->changed = true;
        return;
    }
    if (this->changed) {
        return;
    }
    this->changed = memcmp(this->buff + this->len, ptr, size);
}

inline
void ERaDataBuff::onChange(const char* value) {
    if (value == nullptr) {
        return;
    }
    this->onChange((void*)value, strlen(value));
}

inline
void ERaDataBuff::onChangeHex(uint8_t value) {
    char buf[3] {0};
    snprintf(buf, sizeof(buf), "%02x", value);
    this->onChange(buf);
}

inline
void ERaDataBuff::onChangeHex(const uint8_t* ptr, size_t size) {
    if ((ptr == nullptr) || !size) {
        return;
    }
    size_t len = size * 2 + 1;
    char locBuf[32] {0};
    char* buf = locBuf;
    if (len > sizeof(locBuf)) {
        buf = (char*)malloc(len);
        if (buf == nullptr) {
            return;
        }
    }
    size_t pos {0};
    for (size_t i = 0; i < size; ++i) {
        pos += snprintf(buf + pos, len - pos, "%02x", ptr[i]);
    }
    this->onChange(buf);
    if (buf != locBuf) {
        free(buf);
    }
    buf = nullptr;
}

inline
void ERaDataBuff::remove(size_t index) {
    const iterator e = this->end();
    for (iterator it = this->begin(); it < e; ++it) {
        if (!index--) {
            const char* dst = it;
            ++it;
            const char* src = it;
            memmove((void*)dst, src, this->buff + this->len - src);
            this->len -= (src - dst);
            break;
        }
    }
}

inline
void ERaDataBuff::remove(const char* key) {
    const iterator e = this->end();
    for (iterator it = this->begin(); it < e; ++it) {
        if (it == key) {
            const char* dst = it;
            ++it; ++it;
            const char* src = it;
            memmove((void*)dst, src, this->buff + this->len - src);
            this->len -= (src - dst);
            break;
        }
    }
}

inline
ERaDataBuff::iterator ERaDataBuff::at(int index) const {
    const iterator e = this->end();
    for (iterator it = this->begin(); it < e; ++it) {
        if (!index--) {
            return it;
        }
    }
    return iterator::invalid();
}

inline
ERaDataBuff::iterator ERaDataBuff::at(const char* key) const {
    const iterator e = this->end();
    for (iterator it = this->begin(); it < e; ++it) {
        if (it == key) {
            return ++it;
        }
    }
    return iterator::invalid();
}

inline
size_t ERaDataBuff::size() const {
    size_t retSize {0};
    const iterator e = this->end();
    for (iterator it = this->begin(); it < e; ++it) {
        ++retSize;
    }
    return retSize;
}

inline
size_t ERaDataBuff::split(const char* delims) {
    if ((this->buff == nullptr) ||
        (delims == nullptr)) {
        return 0;
    }
    if (!strlen(this->buff)) {
        return 0;
    }
    if (!strlen(delims)) {
        return 0;
    }
    char* foundAt = nullptr;
    char* readFrom = this->buff;
    const char replace[2] { '\0', '\0' };
    const size_t replaceLen {1};
    const size_t findLen = strlen(delims);
    int diff = (1 - findLen);
    if (!diff) {
        while ((foundAt = strstr(readFrom, delims)) != nullptr) {
            memmove(foundAt, replace, replaceLen);
            readFrom = (foundAt + findLen);
        }
    }
    else {
        size_t l = this->len;
        char* writeTo = this->buff;
        while ((foundAt = strstr(readFrom, delims)) != nullptr) {
            size_t n = (foundAt - readFrom);
            memmove(writeTo, readFrom, n);
            writeTo += n;
            memmove(writeTo, replace, replaceLen);
            writeTo += replaceLen;
            readFrom = (foundAt + findLen);
            l += diff;
        }
        memmove(writeTo, readFrom, strlen(readFrom) + 1);
        this->len = l;
        this->dataLen = l;
    }
    return this->unescape();
}

inline
size_t ERaDataBuff::unescape() {
    if (!this->len) {
        return 0;
    }
    size_t ret {1};
    char* buf = this->buff;
    char* pOut = this->buff;
    size_t l = (this->len - 1);
    while (l--) {
        if (*buf == '\0') {
            switch (*(buf + 1)) {
                case '\0':
                    buf++;
                    this->len--;
                    break;
                default:
                    *(pOut++) = *(buf++);
                    ret++;
                    break;
            }
        }
        else {
            *(pOut++) = *(buf++);
        }
    }
    if (this->dataLen != this->len) {
        this->dataLen = this->len;
    }
    *pOut = '\0';
    return ret;
}

inline
ERaDataBuff::operator char* () const {
    char* ptr = reinterpret_cast<char*>(ERA_MALLOC(this->len));
    if (ptr == nullptr) {
        return nullptr;
    }
    memset(ptr, 0, this->len);
    const iterator e = this->end();
    for (iterator it = this->begin(); it < e; ++it) {
        snprintf(ptr + strlen(ptr), this->len - strlen(ptr), it);
    }
    return ptr;
}

inline
ERaDataBuff::iterator ERaDataBuff::operator [] (int index) const {
    return this->at(index);
}

inline
ERaDataBuff::iterator ERaDataBuff::operator [] (const char* key) const {
    return this->at(key);
}

class ERaDataBuffDynamic
    : public ERaDataBuff
{
public:
    ERaDataBuffDynamic()
        : ERaDataBuff(nullptr, 0, 0)
    {}
    ERaDataBuffDynamic(size_t size)
        : ERaDataBuff(ERA_MALLOC(size), 0, size)
    {
        memset(this->buff, 0, size);
    }
    ~ERaDataBuffDynamic() {
        free(this->buff);
    }

    void allocate(size_t size) {
        if (this->buff != nullptr) {
            return;
        }
        this->buff = (char*)ERA_MALLOC(size);
        this->buffSize = size;
        this->clearBuffer();
    }
};

typedef ERaDataBuff ERaDataBuf;
typedef ERaDataBuffDynamic ERaDataBufDynamic;

class ERaDataJson
{
    typedef cJSON* (CJSON_STDCALL* AddBool)(cJSON* const object, const char* const name, const cJSON_bool boolean);
    typedef cJSON* (CJSON_STDCALL* AddNumber)(cJSON* const object, const char* const name, const double number);
    typedef cJSON* (CJSON_STDCALL* AddDouble)(cJSON* const object, const char* const name, const double number, int decimal);
    typedef cJSON* (CJSON_STDCALL* AddString)(cJSON* const object, const char* const name, const char* const string);
    typedef cJSON_bool (CJSON_STDCALL* AddItem)(cJSON* object, const char* name, cJSON* item);
    typedef struct __DataHooks {
        AddBool addBool;
        AddNumber addNumber;
        AddDouble addDouble;
        AddString addString;
        AddItem addItem;
    } DataHooks;

public:
    class iterator
    {
    public:
        iterator()
            : item(nullptr)
        {}
        iterator(cJSON* pItem,
                cJSON* pParent = nullptr)
            : item(pItem)
            , parent(pParent)
        {}
        ~iterator()
        {}

        static iterator invalid() {
            return iterator(nullptr, nullptr);
        }

        bool isBool() const {
            if (!this->isValid()) {
                return false;
            }
            return cJSON_IsBool(this->item);
        }

        bool isNumber() const {
            if (!this->isValid()) {
                return false;
            }
            return cJSON_IsNumber(this->item);
        }

        bool isString() const {
            if (!this->isValid()) {
                return false;
            }
            return cJSON_IsString(this->item);
        }

        bool isNull() const {
            if (!this->isValid()) {
                return false;
            }
            return cJSON_IsNull(this->item);
        }

        bool isObject() const {
            if (!this->isValid()) {
                return false;
            }
            return cJSON_IsObject(this->item);
        }

        bool isArray() const {
            if (!this->isValid()) {
                return false;
            }
            return cJSON_IsArray(this->item);
        }

        const cJSON* getItem() const {
            return this->item;
        }

        ERaInt_t getInt() const {
            if (!this->isValid()) {
                return 0;
            }
            return this->item->valueint;
        }

        float getFloat() const {
            if (!this->isValid()) {
                return 0.0f;
            }
            return (float)this->item->valuedouble;
        }

        double getDouble() const {
            if (!this->isValid()) {
                return 0.0;
            }
            return this->item->valuedouble;
        }

        const char* getString() const {
            if (!this->isValid()) {
                return nullptr;
            }
            return this->item->valuestring;
        }

        const char* getName() const {
            if (!this->isValid()) {
                return nullptr;
            }
            return this->item->string;
        }

        const cJSON* getObject() const {
            return this->item;
        }

#if defined(ERA_USE_LONG_LONG)

    #if defined(ERA_USE_ERA_ATOLL)
        ERaInt_t parseInt() const {
            if (this->isNumber()) {
                return this->item->valueint;
            }
            else if (this->isString()) {
                return ERaAtoll(this->item->valuestring);
            }
            return 0;
        }
    #else
        ERaInt_t parseInt() const {
            if (this->isNumber()) {
                return this->item->valueint;
            }
            else if (this->isString()) {
                return atoll(this->item->valuestring);
            }
            return 0;
        }
    #endif

        ERaInt_t parseInt(int base) const {
            if (this->isNumber()) {
                return this->item->valueint;
            }
            else if (this->isString()) {
                return strtoll(this->item->valuestring, nullptr, base);
            }
            return 0;
        }

#else

        ERaInt_t parseInt() const {
            if (this->isNumber()) {
                return this->item->valueint;
            }
            else if (this->isString()) {
                return atol(this->item->valuestring);
            }
            return 0;
        }

        ERaInt_t parseInt(int base) const {
            if (this->isNumber()) {
                return this->item->valueint;
            }
            else if (this->isString()) {
                return strtol(this->item->valuestring, nullptr, base);
            }
            return 0;
        }

#endif

        float parseFloat() const {
            if (this->isNumber()) {
                return (float)this->item->valuedouble;;
            }
            else if (this->isString()) {
                return (float)atof(this->item->valuestring);
            }
            return 0.0f;
        }

        double parseDouble() const {
            if (this->isNumber()) {
                return this->item->valuedouble;;
            }
            else if (this->isString()) {
                return atof(this->item->valuestring);
            }
            return 0.0;
        }

        ERaDataJson toJSON() const {
            if (!this->isString()) {
                return ERaDataJson();
            }
            return ERaDataJson(this->item->valuestring);
        }

        bool rename(const char* name) const {
            return cJSON_Rename(const_cast<cJSON*>(this->item), name);
        }

        bool isValid() const {
            return (this->item != nullptr);
        }

        bool isEmpty() const {
            return !this->isValid();
        }

        size_t size() const {
            return (size_t)cJSON_GetArraySize(this->item);
        }

        size_t length() const {
            if (!this->isString()) {
                return 0;
            }
            return strlen(this->item->valuestring);
        }

        bool keyEqual(const char* name) const {
            if (!this->isValid()) {
                return false;
            }
            if (name == nullptr) {
                return false;
            }
            if (this->item->string == nullptr) {
                return false;
            }
            return !strcmp(this->item->string, name);
        }

        operator const cJSON* () const {
            return this->getItem();
        }

        operator bool () const {
            return this->isValid();
        }

        iterator operator [] (int index) {
            if (!this->isArray()) {
                cJSON* array = cJSON_CreateArray();
                cJSON_ReplaceItem(this->parent, this->item, array);
                this->item = array;
            }
            cJSON* subItem = cJSON_GetArrayIndex(this->item, index);
            return iterator(subItem, this->item);
        }

        iterator operator [] (const char* key) {
            if (!this->isObject()) {
                cJSON* object = cJSON_CreateObject();
                cJSON_ReplaceItem(this->parent, this->item, object);
                this->item = object;
            }
            cJSON* subItem = cJSON_GetObjectItem(this->item, key);
            if (subItem == nullptr) {
                subItem = cJSON_AddNullToObject(this->item, key);
            }
            return iterator(subItem, this->item);
        }

        template <typename T>
        iterator& operator = (T value) {
            cJSON_SetNumber(this->parent, this->item, value);
            return (*this);
        }

        iterator& operator = (bool value) {
            cJSON_SetBool(this->parent, this->item, value);
            return (*this);
        }

        iterator& operator = (float value) {
            cJSON_SetNumberWithDecimal(this->parent, this->item, value, 2);
            return (*this);
        }

        iterator& operator = (double value) {
            cJSON_SetNumberWithDecimal(this->parent, this->item, value, 5);
            return (*this);
        }

        iterator& operator = (char* value) {
            cJSON_SetString(this->parent, this->item, value);
            return (*this);
        }

        iterator& operator = (const char* value) {
            cJSON_SetString(this->parent, this->item, value);
            return (*this);
        }

        iterator& operator = (nullptr_t) {
            cJSON_SetNull(this->parent, this->item);
            return (*this);
        }

        iterator& operator = (const iterator& it) {
            this->item = it.item;
            return (*this);
        }

        template <typename T>
        bool operator == (T value) const {
            if (!this->isNumber()) {
                return false;
            }
            return (this->item->valueint == (ERaInt_t)value);
        }

        bool operator == (bool value) const {
            if (!this->isBool() && !this->isNumber()) {
                return false;
            }
            return (this->item->valueint == value);
        }

        bool operator == (float value) const {
            return this->FloatCompare(value);
        }

        bool operator == (double value) const {
            return operator == ((float)value);
        }

        bool operator == (char* value) const {
            return operator == ((const char*)value);
        }

        bool operator == (const char* value) const {
            if (!this->isString()) {
                return false;
            }
            if (value == nullptr) {
                return false;
            }
            if (this->item->valuestring == nullptr) {
                return false;
            }
            return !strcmp(this->item->valuestring, value);
        }

        bool operator == (nullptr_t) const {
            return this->isNull();
        }

        bool operator != (const iterator& it) const {
            return (this->item != it.item);
        }

        template <typename T>
        bool operator != (T value) const {
            return !(operator == <T> (value));
        }

        bool operator != (bool value) const {
            return !(operator == (value));
        }

        bool operator != (float value) const {
            return !(operator == (value));
        }

        bool operator != (double value) const {
            return !(operator == (value));
        }

        bool operator != (char* value) const {
            return !(operator == (value));
        }

        bool operator != (const char* value) const {
            return !(operator == (value));
        }

        bool operator != (nullptr_t) const {
            return !this->isNull();
        }

        iterator& operator ++ () {
            if (this->isValid()) {
                this->item = this->item->next;
            }
            return (*this);
        }

    private:
        bool FloatCompare(float value) const {
            float maxVal = (fabs(this->item->valuedouble) > fabs(value)) ? fabs(this->item->valuedouble) : fabs(value);
            return (fabs(this->item->valuedouble - value) <= (maxVal * FLT_EPSILON));
        }

        cJSON* item;
        cJSON* parent;
    };

    ERaDataJson(cJSON* json = nullptr)
        : ptr(nullptr)
        , root(json)
    {}
    ERaDataJson(const char* str)
        : ERaDataJson(cJSON_Parse(str))
    {}
    ERaDataJson(const ERaDataJson& json)
        : ptr(nullptr)
        , root(nullptr)
    {
        (*this) = json;
    }
    ~ERaDataJson()
    {
        this->clear();
        this->clearObject();
    }

    const cJSON* getObject() const {
        return this->root;
    }

    cJSON* detachObject() {
        cJSON* it = this->root;
        this->root = nullptr;
        return it;
    }

    void setObject(cJSON* const it) {
        this->root = it;
    }

    void parse(const char* str) {
        this->clear();
        this->clearObject();
        this->root = cJSON_Parse(str);
    }

    const char* c_str() {
        return this->getString();
    }

    const char* getString() {
        this->clear();
        this->ptr = cJSON_PrintUnformatted(this->root);
        return ((this->ptr != nullptr) ? this->ptr : "");
    }

    bool isObject() const {
        if (!this->isValid()) {
            return false;
        }
        return cJSON_IsObject(this->root);
    }

    bool isArray() const {
        if (!this->isValid()) {
            return false;
        }
        return cJSON_IsArray(this->root);
    }

    bool isValid() const {
        return (this->root != nullptr);
    }

    bool isEmpty() const {
        if (!this->isValid()) {
            return true;
        }
        return (this->root->child == nullptr);
    }

    size_t size() const {
        return (size_t)cJSON_GetArraySize(this->root);
    }

    void clear() {
        if (this->ptr == nullptr) {
            return;
        }
        free(this->ptr);
        this->ptr = nullptr;
    }

    void clearObject() {
        if (this->root == nullptr) {
            return;
        }
        cJSON_Delete(this->root);
        this->root = nullptr;
    }

    template <typename T>
    void set(const char* name, T value);
    void set(const char* name, bool value);
    void set(const char* name, float value);
    void set(const char* name, double value);
    void set(const char* name, char* value);
    void set(const char* name, const char* value);

    template <typename T>
    void add(const char* name, T value);
    void add(const char* name, bool value);
    void add(const char* name, float value);
    void add(const char* name, double value);
    void add(const char* name, char* value);
    void add(const char* name, const char* value);
    void add(const char* name, cJSON* value);
    void add(const char* name, ERaDataJson& value);
    void add(const char* name, ERaParam& value);

#if defined(ERA_HAS_PROGMEM)
    void add(const char* name, const __FlashStringHelper* value);
#endif

    template <typename T>
    void add_multi(const T value) {
        (void)value;
    }

    template <typename T> 
    void add_multi(const char* name, const T last) {
        this->add(name, last);
    }

    template <typename T, typename... Args> 
    void add_multi(const char* name, const T head, Args... tail) {
        this->add(name, head);
        this->add_multi(tail...);
    }

    template <typename T> 
    void add_multi(ERaInt_t id, const T last) {
        char name[2 + 8 * sizeof(ERaInt_t)] {0};
        snprintf(name, sizeof(name), ERA_INT_FORMAT, id);
        this->add_multi(name, last);
    }

    template <typename T, typename... Args> 
    void add_multi(ERaInt_t id, const T head, Args... tail) {
        char name[2 + 8 * sizeof(ERaInt_t)] {0};
        snprintf(name, sizeof(name), ERA_INT_FORMAT, id);
        this->add_multi(name, head, tail...);
    }

    cJSON* detach(size_t index);
    cJSON* detach(const char* key);
    cJSON* detach(cJSON* const it);
    cJSON* detach(const iterator& it);

    void remove(size_t index);
    void remove(const char* key);
    void remove(cJSON* const it);
    void remove(const iterator& it);

    iterator begin() const {
        if (this->root == nullptr) {
            return iterator(nullptr);
        }
        return iterator(this->root->child, this->root);
    }

    iterator end() const {
        return iterator(nullptr);
    }

    iterator at(int index);
    iterator at(const char* key);

    operator const char* () {
        return this->getString();
    }

    iterator operator [] (int index);
    iterator operator [] (const char* key);
    bool operator == (ERaDataJson& value) const;
    bool operator == (const ERaDataJson& value) const;
    bool operator == (nullptr_t) const;
    bool operator != (ERaDataJson& value) const;
    bool operator != (const ERaDataJson& value) const;
    bool operator != (nullptr_t) const;

    ERaDataJson& operator = (const char* value);
    ERaDataJson& operator = (const ERaDataJson& value);
    ERaDataJson& operator = (nullptr_t);

protected:
    void create() {
        if (cJSON_IsObject(this->root)) {
            return;
        }
        cJSON_Delete(this->root);
        this->root = cJSON_CreateObject();
    }

    void createArray() {
        if (cJSON_IsArray(this->root)) {
            return;
        }
        cJSON_Delete(this->root);
        this->root = cJSON_CreateArray();
    }

    cJSON* setBool(cJSON* const object, const char* const name, const cJSON_bool boolean) {
        return cJSON_SetBoolToObject(object, name, boolean);
    }

    cJSON* setNumber(cJSON* const object, const char* const name, const double number) {
        return cJSON_SetNumberToObject(object, name, number);
    }

    cJSON* setDouble(cJSON* const object, const char* const name, const double number, int decimal) {
        return cJSON_SetNumberWithDecimalToObject(object, name, number, decimal);
    }

    cJSON* setString(cJSON* const object, const char* const name, const char* const string) {
        return cJSON_SetStringToObject(object, name, string);
    }

    cJSON* addBool(cJSON* const object, const char* const name, const cJSON_bool boolean) {
        return cJSON_AddBoolToObject(object, name, boolean);
    }

    cJSON* addNumber(cJSON* const object, const char* const name, const double number) {
        return cJSON_AddNumberToObject(object, name, number);
    }

    cJSON* addDouble(cJSON* const object, const char* const name, const double number, int decimal) {
        return cJSON_AddNumberWithDecimalToObject(object, name, number, decimal);
    }

    cJSON* addString(cJSON* const object, const char* const name, const char* const string) {
        return cJSON_AddStringToObject(object, name, string);
    }

    cJSON_bool addItem(cJSON* object, const char* name, cJSON* item) {
        return cJSON_AddItemToObject(object, name, item);
    }

    char* ptr;
    cJSON* root;
};

template <typename T>
inline
void ERaDataJson::set(const char* name, T value) {
    this->create();
    this->setNumber(this->root, name, value);
}

inline
void ERaDataJson::set(const char* name, bool value) {
    this->create();
    this->setBool(this->root, name, value);
}

inline
void ERaDataJson::set(const char* name, float value) {
    this->create();
    this->setDouble(this->root, name, value, 2);
}

inline
void ERaDataJson::set(const char* name, double value) {
    this->create();
    this->setDouble(this->root, name, value, 5);
}

inline
void ERaDataJson::set(const char* name, char* value) {
    this->create();
    this->setString(this->root, name, value);
}

inline
void ERaDataJson::set(const char* name, const char* value) {
    this->create();
    this->setString(this->root, name, value);
}

template <typename T>
inline
void ERaDataJson::add(const char* name, T value) {
    this->create();
    this->addNumber(this->root, name, value);
}

inline
void ERaDataJson::add(const char* name, bool value) {
    this->create();
    this->addBool(this->root, name, value);
}

inline
void ERaDataJson::add(const char* name, float value) {
    this->create();
    this->addDouble(this->root, name, value, 2);
}

inline
void ERaDataJson::add(const char* name, double value) {
    this->create();
    this->addDouble(this->root, name, value, 5);
}

inline
void ERaDataJson::add(const char* name, char* value) {
    this->create();
    this->addString(this->root, name, value);
}

inline
void ERaDataJson::add(const char* name, const char* value) {
    this->create();
    this->addString(this->root, name, value);
}

inline
void ERaDataJson::add(const char* name, cJSON* value) {
    this->create();
    this->addItem(this->root, name, value);
}

inline
void ERaDataJson::add(const char* name, ERaDataJson& value) {
    this->add(name, value.detachObject());
}

#if defined(ERA_HAS_PROGMEM)

    inline
    void ERaDataJson::add(const char* name, const __FlashStringHelper* value) {
        if (value == nullptr) {
            return;
        }
        PGM_P p = reinterpret_cast<PGM_P>(value);
        size_t size = strlen_P(p);
        char str[size + 1] {0};
        memcpy_P(str, p, size);
        this->add(name, str);
    }

#endif

inline
cJSON* ERaDataJson::detach(size_t index) {
    const iterator e = this->end();
    for (iterator it = this->begin(); it != e; ++it) {
        if (!index--) {
            return cJSON_DetachItemFromObject(this->root, it.getItem()->string);
        }
    }
}

inline
cJSON* ERaDataJson::detach(const char* key) {
    return cJSON_DetachItemFromObject(this->root, key);
}

inline
cJSON* ERaDataJson::detach(cJSON* const it) {
    return cJSON_DetachItemViaPointer(this->root, it);
}

inline
cJSON* ERaDataJson::detach(const iterator& it) {
    return cJSON_DetachItemViaPointer(this->root, const_cast<cJSON*>(it.getItem()));
}

inline
void ERaDataJson::remove(size_t index) {
    const iterator e = this->end();
    for (iterator it = this->begin(); it != e; ++it) {
        if (!index--) {
            cJSON_DeleteItemFromObject(this->root, it.getItem()->string);
            break;
        }
    }
}

inline
void ERaDataJson::remove(const char* key) {
    cJSON_DeleteItemFromObject(this->root, key);
}

inline
void ERaDataJson::remove(cJSON* const it) {
    cJSON_DeleteItemViaPointer(this->root, it);
}

inline
void ERaDataJson::remove(const ERaDataJson::iterator& it) {
    cJSON_DeleteItemViaPointer(this->root, const_cast<cJSON*>(it.getItem()));
}

inline
ERaDataJson::iterator ERaDataJson::at(int index) {
    const iterator e = this->end();
    for (iterator it = this->begin(); it != e; ++it) {
        if (!index--) {
            return it;
        }
    }
    if (!this->isValid()) {
        this->createArray();
    }
    cJSON* item = cJSON_GetArrayIndex(this->root, index);
    return iterator(item, this->root);
}

inline
ERaDataJson::iterator ERaDataJson::at(const char* key) {
    const iterator e = this->end();
    for (iterator it = this->begin(); it != e; ++it) {
        if (it.keyEqual(key)) {
            return it;
        }
    }
    this->create();
    cJSON* item = cJSON_AddNullToObject(this->root, key);
    return iterator(item, this->root);
}

inline
ERaDataJson::iterator ERaDataJson::operator [] (int index) {
    return this->at(index);
}

inline
ERaDataJson::iterator ERaDataJson::operator [] (const char* key) {
    return this->at(key);
}

inline
bool ERaDataJson::operator == (ERaDataJson& value) const {
    if (this == &value) {
        return true;
    }
    return cJSON_Compare(this->root, value.getObject(), true);
}

inline
bool ERaDataJson::operator == (const ERaDataJson& value) const {
    if (this == &value) {
        return true;
    }
    return cJSON_Compare(this->root, value.getObject(), true);
}

inline
bool ERaDataJson::operator == (nullptr_t) const {
    return this->isEmpty();
}

inline
bool ERaDataJson::operator != (ERaDataJson& value) const {
    return !(operator == (value));
}

inline
bool ERaDataJson::operator != (const ERaDataJson& value) const {
    return !(operator == (value));
}

inline
bool ERaDataJson::operator != (nullptr_t) const {
    return !this->isEmpty();
}

inline
ERaDataJson& ERaDataJson::operator = (const char* value) {
    this->clear();
    this->clearObject();
    this->root = cJSON_Parse(value);
    return (*this);
}

inline
ERaDataJson& ERaDataJson::operator = (const ERaDataJson& value) {
    if (this == &value) {
        return (*this);
    }
    this->clear();
    this->clearObject();
    this->root = cJSON_Duplicate(value.getObject(), true);
    return (*this);
}

inline
ERaDataJson& ERaDataJson::operator = (nullptr_t) {
    this->clear();
    this->clearObject();
    return (*this);
}

inline
ERaDataJson ERaDataBuff::iterator::toJSON() const {
    if (!this->isValid()) {
        return ERaDataJson();
    }
    return ERaDataJson(this->ptr);
}

using DataEntry = ERaDataBuff::iterator;
using JsonEntry = ERaDataJson::iterator;

typedef ERaDataJson ERaJson;
typedef ERaDataBuff ERaBuff;
typedef ERaDataBuffDynamic ERaBuffDynamic;

#endif /* INC_ERA_DATA_HPP_ */
