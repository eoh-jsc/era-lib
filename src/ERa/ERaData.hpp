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

		int getInt(int _base = 10) const {
			if (!this->isValid()) {
				return 0;
			}
			return strtol(this->ptr, nullptr, _base);
		}

        unsigned int getUint(int _base = 10) const {
            if (!this->isValid()) {
                return 0;
            }
            return strtoul(this->ptr, nullptr, _base);
        }

        float getFloat() const {
            if (!this->isValid()) {
                return 0.0f;
            }
            return strtof(this->ptr, nullptr);
        }

        double getDouble() const {
            if (!this->isValid()) {
                return 0.0;
            }
            return strtod(this->ptr, nullptr);
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

	ERaDataBuff(char* _buff, size_t _len)
		: buff(_buff)
		, len(0)
		, buffSize(_len)
	{}
	ERaDataBuff(const char* _buff, size_t _len)
		: buff((char*)_buff)
		, len(0)
		, buffSize(_len)
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
	void remove(size_t index);
	void remove(const char* key);

	const char* getBuffer() {
		return this->buff;
	}

	size_t getLen() {
		return this->len;
	}

	iterator begin() const {
		return iterator(this->buff, this->buff + this->len);
	}

	iterator end() const {
		return iterator(this->buff + this->len, this->buff + this->len);
	}

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
	char* buff;
	size_t len;
	size_t buffSize;
};

inline
void ERaDataBuff::add(const void* ptr, size_t size) {
	if (ptr == nullptr) {
		return;
	}
	if (this->len + size > this->buffSize) {
		return;
	}
	memcpy(this->buff + this->len, ptr, size);
	this->len += size;
	this->buff[this->len++] = '\0';
}

inline
void ERaDataBuff::add_hex(uint8_t value) {
	if (this->len + 2 > this->buffSize) {
		return;
	}
	this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%02x", value);
	this->buff[this->len++] = '\0';
}

inline
void ERaDataBuff::add_hex_array(const uint8_t* ptr, size_t size) {
	if (ptr == nullptr || !size) {
		return;
	}
	if (this->len + size * 2 > this->buffSize) {
		return;
	}
	for (size_t i = 0; i < size; ++i) {
		this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%02x", ptr[i]);
	}
	this->buff[this->len++] = '\0';
}

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

#if defined(ERA_HAS_PROGMEM)
	inline
	void ERaDataBuff::add(const __FlashStringHelper* ptr) {
		if (ptr == nullptr) {
			return;
		}
		PGM_P p = reinterpret_cast<PGM_P>(ptr);
		size_t size = strlen_P(p);
		if (this->len + size > this->buffSize) {
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
	const iterator e = this->end();
	for (iterator it = this->begin(); it < e; ++it) {
		if (!index--) {
			return it;
		}
	}
	return iterator::invalid();
}

inline
ERaDataBuff::iterator ERaDataBuff::operator [] (const char* key) const {
	const iterator e = this->end();
	for (iterator it = this->begin(); it < e; ++it) {
		if (it == key) {
			return ++it;
		}
	}
	return iterator::invalid();
}

class ERaDataBuffDynamic : public ERaDataBuff
{
public:
	ERaDataBuffDynamic()
		: ERaDataBuff((char*)nullptr, 0)
	{}
	ERaDataBuffDynamic(size_t size)
		: ERaDataBuff((char*)ERA_MALLOC(size), size)
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

		bool rename(const char* name) const {
			return cJSON_Rename(const_cast<cJSON*>(this->item), name);
		}

		bool isValid() const {
			return this->item != nullptr;
		}

		bool isEmpty() const {
			return !this->isValid();
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

	operator const char* () {
		return this->getString();
	}

	iterator operator [] (int index) const;
	iterator operator [] (const char* key) const;

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
	this->hooks.addDouble(this->root, name, value, 5);
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
ERaDataJson::iterator ERaDataJson::operator [] (int index) const {
	const iterator e = this->end();
	for (iterator it = this->begin(); it != e; ++it) {
		if (!index--) {
			return it;
		}
	}
	return iterator::invalid();
}

inline
ERaDataJson::iterator ERaDataJson::operator [] (const char* key) const {
	const iterator e = this->end();
	for (iterator it = this->begin(); it != e; ++it) {
		if (it == key) {
			return it;
		}
	}
	return iterator::invalid();
}

#endif /* INC_ERA_DATA_HPP_ */
