#ifndef INC_MVP_DATA_HPP_
#define INC_MVP_DATA_HPP_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <MVP/MVPDefine.hpp>

class MVPDataBuff
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

	MVPDataBuff(char* _buff, size_t _len)
		: buff(_buff)
		, len(0)
		, buffSize(_len)
	{}
	MVPDataBuff(const char* _buff, size_t _len)
		: buff((char*)_buff)
		, len(0)
		, buffSize(_len)
	{}
	~MVPDataBuff() {}

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
    void add(float value);
    void add(double value);

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

void MVPDataBuff::add(const void* ptr, size_t size) {
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

void MVPDataBuff::add_hex(uint8_t value) {
	if (this->len + 2 > this->buffSize) {
		return;
	}
	this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%02x", value);
	this->buff[this->len++] = '\0';
}

void MVPDataBuff::add_hex_array(const uint8_t* ptr, size_t size) {
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

void MVPDataBuff::add(int value) {
    this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%i", value);
    this->buff[this->len++] = '\0';
}

void MVPDataBuff::add(unsigned int value) {
    this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%u", value);
    this->buff[this->len++] = '\0';
}

void MVPDataBuff::add(float value) {
    this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%.2f", value);
    this->buff[this->len++] = '\0';
}

void MVPDataBuff::add(double value) {
    this->len += snprintf(this->buff + this->len, this->buffSize - this->len, "%.5f", value);
    this->buff[this->len++] = '\0';
}

bool MVPDataBuff::next() {
	char* ptr = this->buff + this->len;
	if (*ptr != '\0') {
		this->len += strlen(ptr);
		this->buff[this->len++] = '\0';
		return true;
	}
	return false;
}

void MVPDataBuff::remove(size_t index) {
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

void MVPDataBuff::remove(const char* key) {
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

MVPDataBuff::operator char* () const {
	char* ptr = reinterpret_cast<char*>(MVP_MALLOC(this->len));
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

MVPDataBuff::iterator MVPDataBuff::operator [] (int index) const {
	const iterator e = this->end();
	for (iterator it = this->begin(); it < e; ++it) {
		if (!index--) {
			return it;
		}
	}
	return iterator::invalid();
}

MVPDataBuff::iterator MVPDataBuff::operator [] (const char* key) const {
	const iterator e = this->end();
	for (iterator it = this->begin(); it < e; ++it) {
		if (it == key) {
			return ++it;
		}
	}
	return iterator::invalid();
}

class MVPDataBuffDynamic : public MVPDataBuff
{
public:
	MVPDataBuffDynamic()
		: MVPDataBuff((char*)nullptr, 0)
	{}
	MVPDataBuffDynamic(size_t size)
		: MVPDataBuff((char*)MVP_MALLOC(size), size)
	{
		memset(this->buff, 0, size);
	}
	~MVPDataBuffDynamic() {
		free(this->buff);
	}

	void allocate(size_t size) {
		if (this->buff != nullptr) {
			return;
		}
		this->buff = (char*)MVP_MALLOC(size);
		this->buffSize = size;
		this->clearBuffer();
	}
};

#endif /* INC_MVP_DATA_HPP_ */