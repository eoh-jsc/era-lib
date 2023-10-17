#ifndef INC_ERA_DATA_HPP_
#define INC_ERA_DATA_HPP_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ERa/ERaDefine.hpp>

class ERaParam;

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
		iterator(char* _ptr, char* _limit)
			: ptr(_ptr)
			, limit(_limit)
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
			return atoi(ptr);
		}

		int getInt(int _base) const {
			return (int)this->getLong(_base);
		}

		long getLong() const {
			if (!this->isValid()) {
				return 0;
			}
			return atol(ptr);
		}

		long getLong(int _base) const {
			if (!this->isValid()) {
				return 0;
			}
			return strtol(this->ptr, nullptr, _base);
		}

#if defined(ERA_USE_ERA_ATOLL)
		long long getLongLong() const {
			if (!this->isValid()) {
				return 0;
			}
			return ERaAtoll(ptr);
		}
#else
		long long getLongLong() const {
			if (!this->isValid()) {
				return 0;
			}
			return atoll(ptr);
		}

		long long getLongLong(int _base) const {
			if (!this->isValid()) {
				return 0;
			}
			return strtoll(this->ptr, nullptr, _base);
		}
#endif

        float getFloat() const {
            if (!this->isValid()) {
                return 0.0f;
            }
            return (float)atof(ptr);
        }

        double getDouble() const {
            if (!this->isValid()) {
                return 0.0;
            }
            return atof(ptr);
        }

		bool isValid() const {
			return ((this->ptr != nullptr) && (this->ptr < this->limit));
		}

		bool isEmpty() const {
			if (!this->isValid()) {
				return true;
			}
			return *this->ptr == '\0';
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

		bool operator == (const char* _ptr) {
			if (!this->isValid()) {
				return false;
			}
			return !strcmp(this->ptr, _ptr);
		}

		iterator& operator ++ () {
			if (this->isValid()) {
				this->ptr += strlen(this->ptr) + 1;
			}
			return *this;
		}

	private:
		const char* ptr;
		const char* limit;
	};

	ERaDataBuff(const void* _buff, size_t _len)
		: buff((char*)_buff)
		, len(_len)
		, buffSize(_len)
		, dataLen(_len)
		, changed(false)
	{}
	ERaDataBuff(void* _buff, size_t _len, size_t _size)
		: buff((char*)_buff)
		, len(_len)
		, buffSize(_size)
		, dataLen(_len)
		, changed(false)
	{}
	~ERaDataBuff()
	{}

	const char* getString() const {
		return this->buff;
	}

	bool isValid() const {
		return this->buff != nullptr;
	}

	bool isEmpty() const {
		return *this->buff == '\0';
	}

	bool isChange() {
		bool _changed = this->changed;
		this->changed = false;
		return _changed;
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

	template <typename T, typename... Args> 
	void add_multi(const T last) {
		add(last);
	}

	template <typename T, typename... Args> 
	void add_multi(const T head, Args... tail) {
		add(head);
		add_multi(tail...);
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
		char str[2 + 8 * sizeof(value)] {0};
		itoa(value, str, 10);
		add(str);
	}

	inline
	void ERaDataBuff::add(unsigned int value) {
        char str[1 + 8 * sizeof(value)] {0};
        utoa(value, str, 10);
        add(str);
	}

	inline
	void ERaDataBuff::add(long value) {
        char str[2 + 8 * sizeof(value)] {0};
        ltoa(value, str, 10);
        add(str);
	}

	inline
	void ERaDataBuff::add(unsigned long value) {
        char str[1 + 8 * sizeof(value)] {0};
        ultoa(value, str, 10);
        add(str);
	}

	inline
	void ERaDataBuff::add(long long value) {
        char str[2 + 8 * sizeof(value)] {0};
        ltoa(value, str, 10);
        add(str);
	}

	inline
	void ERaDataBuff::add(unsigned long long value) {
        char str[1 + 8 * sizeof(value)] {0};
        ultoa(value, str, 10);
        add(str);
	}

	inline
	void ERaDataBuff::add(float value) {
        char str[33] {0};
        dtostrf(value, 5, 2, str);
        add(str);
	}

	inline
	void ERaDataBuff::add(double value) {
        char str[33] {0};
        dtostrf(value, 5, 5, str);
        add(str);
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
        add(str);
	}

	inline
	void ERaDataBuff::add(double value) {
        char str[33] {0};
        ERaDtostrf(value, 5, str);
        add(str);
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
	size_t _size {0};
	const iterator e = this->end();
	for (iterator it = this->begin(); it < e; ++it) {
		++_size;
	}
	return _size;
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
		iterator(cJSON* _item)
			: item(_item)
		{}
		~iterator()
		{}

        static iterator invalid() {
            return iterator(nullptr);
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

		bool isObject() const {
			if (!this->isValid()) {
				return false;
			}
			return cJSON_IsObject(this->item);
		}

		const cJSON* getItem() const {
			return this->item;
		}

		int getInt() const {
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

		bool rename(const char* name) const {
			return cJSON_Rename(const_cast<cJSON*>(this->item), name);
		}

		bool isValid() const {
			return this->item != nullptr;
		}

		bool isEmpty() const {
			return !this->isValid();
		}

		size_t size() const {
			return (size_t)cJSON_GetArraySize(this->item);
		}

		operator const cJSON* () const {
			return this->getItem();
		}

		operator bool () const {
			return this->isValid();
		}

		iterator& operator = (const iterator& it) {
			this->item = it.item;
			return *this;
		}

		bool operator != (const iterator& it) {
			return this->item != it.item;
		}

		bool operator == (const char* name) {
			if (!this->isValid()) {
				return false;
			}
			return !strcmp(this->item->string, name);
		}

		iterator& operator ++ () {
			if (this->isValid()) {
				this->item = this->item->next;
			}
			return *this;
		}

	private:
		const cJSON* item;
	};

	ERaDataJson(cJSON* json = nullptr,
				AddBool addBool = cJSON_AddBoolToObject,
				AddNumber addNumber = cJSON_AddNumberToObject,
				AddDouble addDouble = cJSON_AddNumberWithDecimalToObject,
				AddString addString = cJSON_AddStringToObject,
				AddItem addItem = cJSON_AddItemToObject)
		: ptr(nullptr)
		, root(json)
		, hooks {
			.addBool = addBool,
			.addNumber = addNumber,
			.addDouble = addDouble,
			.addString = addString,
			.addItem = addItem
		}
	{}
	ERaDataJson(const char* str)
		: ERaDataJson(cJSON_Parse(str))
	{}
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
		if (this->root != nullptr) {
			cJSON_Delete(this->root);
		}
		this->root = cJSON_Parse(str);
	}

	const char* getString() {
		this->clear();
		this->ptr = cJSON_PrintUnformatted(this->root);
		return this->ptr;
	}

	bool isValid() const {
		return cJSON_IsObject(this->root);
	}

	bool isEmpty() const {
		if (!this->isValid()) {
			return true;
		}
		return this->root->child == nullptr;
	}

	size_t size() const {
		return (size_t)cJSON_GetArraySize(this->root);
	}

	void clear() {
		if (this->ptr != nullptr) {
			free(this->ptr);
		}
		this->ptr = nullptr;
	}

	void clearObject() {
		cJSON_Delete(this->root);
		this->root = nullptr;
	}

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

	template <typename T, typename... Args> 
	void add_multi(const char* name, const T last) {
		add(name, last);
	}

	template <typename T, typename... Args> 
	void add_multi(const char* name, const T head, Args... tail) {
		add(name, head);
		add_multi(tail...);
	}

	template <typename T, typename... Args> 
	void add_multi(int id, const T last) {
		char name[2 + 8 * sizeof(id)] {0};
		snprintf(name, sizeof(name), "%i", id);
		add_multi(name, last);
	}

	template <typename T, typename... Args> 
	void add_multi(int id, const T head, Args... tail) {
		char name[2 + 8 * sizeof(id)] {0};
		snprintf(name, sizeof(name), "%i", id);
		add_multi(name, head, tail...);
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
		return iterator(this->root->child);
	}

	iterator end() const {
		return iterator(nullptr);
	}

	iterator at(int index) const;
	iterator at(const char* key) const;

	operator const char* () {
		return this->getString();
	}

	iterator operator [] (int index) const;
	iterator operator [] (const char* key) const;
	bool operator == (ERaDataJson& value) const;
	bool operator == (const ERaDataJson& value) const;

protected:
	void create() {
		if (this->root != nullptr) {
			return;
		}
		this->root = cJSON_CreateObject();
	}

	char* ptr;
	cJSON* root;
	DataHooks hooks;
};

template <typename T>
inline
void ERaDataJson::add(const char* name, T value) {
	this->create();
	this->hooks.addNumber(this->root, name, value);
}

inline
void ERaDataJson::add(const char* name, bool value) {
	this->create();
	this->hooks.addBool(this->root, name, value);
}

inline
void ERaDataJson::add(const char* name, float value) {
	this->create();
	this->hooks.addDouble(this->root, name, value, 2);
}

inline
void ERaDataJson::add(const char* name, double value) {
	this->create();
	this->hooks.addDouble(this->root, name, value, 5);
}

inline
void ERaDataJson::add(const char* name, char* value) {
	this->create();
	this->hooks.addString(this->root, name, value);
}

inline
void ERaDataJson::add(const char* name, const char* value) {
	this->create();
	this->hooks.addString(this->root, name, value);
}

inline
void ERaDataJson::add(const char* name, cJSON* value) {
	this->create();
	this->hooks.addItem(this->root, name, value);
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
ERaDataJson::iterator ERaDataJson::at(int index) const {
	const iterator e = this->end();
	for (iterator it = this->begin(); it != e; ++it) {
		if (!index--) {
			return it;
		}
	}
	return iterator::invalid();
}

inline
ERaDataJson::iterator ERaDataJson::at(const char* key) const {
	const iterator e = this->end();
	for (iterator it = this->begin(); it != e; ++it) {
		if (it == key) {
			return it;
		}
	}
	return iterator::invalid();
}

inline
ERaDataJson::iterator ERaDataJson::operator [] (int index) const {
	return this->at(index);
}

inline
ERaDataJson::iterator ERaDataJson::operator [] (const char* key) const {
	return this->at(key);
}

inline
bool ERaDataJson::operator == (ERaDataJson& value) const {
	return cJSON_Compare(this->root, value.getObject(), true);
}

inline
bool ERaDataJson::operator == (const ERaDataJson& value) const {
	return cJSON_Compare(this->root, value.getObject(), true);
}

using DataEntry = ERaDataBuff::iterator;
using JsonEntry = ERaDataJson::iterator;

#endif /* INC_ERA_DATA_HPP_ */
