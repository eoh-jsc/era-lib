#ifndef INC_ERA_PARAM_HPP_
#define INC_ERA_PARAM_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ERa/ERaDefine.hpp>
#include <Utility/ERacJSON.hpp>
#include <Utility/ERaUtility.hpp>
#include <ERa/ERaData.hpp>

class ERaParam
{
	enum ERaParamTypeT {
		ERA_PARAM_TYPE_INVALID = 0,
		ERA_PARAM_TYPE_NUMBER = 1,
		ERA_PARAM_TYPE_STRING = 2,
		ERA_PARAM_TYPE_STATIC_STRING = 4,
		ERA_PARAM_TYPE_OBJECT = 8
	};

public:
	ERaParam()
		: type(0)
		, valueint(0)
		, valuedouble(0)
		, valuestring(nullptr)
		, valueobject(nullptr)
	{}
	template <typename T>
	ERaParam(T value)
		: type(0)
		, valueint(0)
		, valuedouble(0)
		, valuestring(nullptr)
		, valueobject(nullptr)
	{
		add(value);
	}
	ERaParam(ERaDataJson& value)
		: type(0)
		, valueint(0)
		, valuedouble(0)
		, valuestring(nullptr)
		, valueobject(nullptr)
	{
		add(value);
	}
	ERaParam(const ERaParam& value)
		: type(0)
		, valueint(0)
		, valuedouble(0)
		, valuestring(nullptr)
		, valueobject(nullptr)
	{
		add(value);
	}
	~ERaParam()
	{
		this->free();
		this->type = 0;
		this->valueint = 0;
		this->valuedouble = 0;
		this->valueobject = nullptr;
	}

	bool isNumber() const {
		return this->getType(ERaParamTypeT::ERA_PARAM_TYPE_NUMBER);
	}

	bool isString() const {
		return (this->getType(ERaParamTypeT::ERA_PARAM_TYPE_STRING) ||
				this->getType(ERaParamTypeT::ERA_PARAM_TYPE_STATIC_STRING));
	}

	bool isObject() const {
		return this->getType(ERaParamTypeT::ERA_PARAM_TYPE_OBJECT);
	}

	int getInt() const {
		return this->valueint;
	}

	float getFloat() const {
		return static_cast<float>(this->valuedouble);
	}

	double getDouble() const {
		return this->valuedouble;
	}

	char* getString() const {
		return this->valuestring;
	}

	ERaDataJson* getObject() const {
		return this->valueobject;
	}

	template <typename T>
	void add(T value) {
		addParam(value);
	}

	void add(ERaDataJson& value) {
		addParam(value);
	}

	void add(const ERaParam& value) {
		addParam(value);
	}

	void add_static(char* value) {
		this->free();
		this->valuestring = value;
		this->setType(ERaParamTypeT::ERA_PARAM_TYPE_STATIC_STRING, true);
	}

	void add_static(const char* value) {
		this->free();
		this->valuestring = const_cast<char*>(value);
		this->setType(ERaParamTypeT::ERA_PARAM_TYPE_STATIC_STRING, true);
	}

	template <typename T>
	ERaParam& operator = (T value) {
		add(value);
		return *this;
	}

	ERaParam& operator = (ERaDataJson& value) {
		add(value);
		return *this;
	}

	ERaParam& operator = (const ERaParam& value) {
		add(value);
		return *this;
	}

	template <typename T>
	bool operator == (T value) const {
		return this->valueint == (int)value;
	}

	bool operator == (float value) const {
		return ERaFloatCompare((float)this->valuedouble, value);
	}

	bool operator == (double value) const {
		return ERaDoubleCompare(this->valuedouble, value);
	}

	bool operator == (char* value) const {
		if (value == nullptr) {
			return false;
		}
		if (this->valuestring == nullptr) {
			return false;
		}
		return ERaStrCmp(this->valuestring, value);
	}

	bool operator == (const char* value) const {
		if (value == nullptr) {
			return false;
		}
		if (this->valuestring == nullptr) {
			return false;
		}
		return ERaStrCmp(this->valuestring, value);
	}

	bool operator == (ERaDataJson* value) const {
		if (value == nullptr) {
			return false;
		}
		if (this->valueobject == nullptr) {
			return false;
		}
		return (*this->valueobject == *value);
	}

	bool operator == (const ERaDataJson* value) const {
		if (value == nullptr) {
			return false;
		}
		if (this->valueobject == nullptr) {
			return false;
		}
		return (*this->valueobject == *value);
	}

	bool operator == (ERaParam& value) const {
		if (this == &value) {
			return true;
		}
		if (this->isNumber() &&
			value.isNumber()) {
			return (*this == value.getDouble());
		}
		else if (this->isString() &&
			value.isString()) {
			return (*this == value.getString());
		}
		else if (this->isObject() &&
			value.isObject()) {
			return (*this == value.getObject());
		}
		return false;
	}

	bool operator == (const ERaParam& value) const {
		if (this == &value) {
			return true;
		}
		if (this->isNumber() &&
			value.isNumber()) {
			return (*this == value.getDouble());
		}
		else if (this->isString() &&
			value.isString()) {
			return (*this == value.getString());
		}
		else if (this->isObject() &&
			value.isObject()) {
			return (*this == value.getObject());
		}
		return false;
	}

	ERaDataJson::iterator operator [] (int index) const {
		if (this->valueobject != nullptr) {
			return this->valueobject->at(index);
		}
		return ERaDataJson::iterator::invalid();
	}

	ERaDataJson::iterator operator [] (const char* key) const {
		if (this->valueobject != nullptr) {
			return this->valueobject->at(key);
		}
		return ERaDataJson::iterator::invalid();
	}

	template <typename T>
	operator T() const { return static_cast<T>(this->valueint); }

	operator float() const { return static_cast<float>(this->valuedouble); }
	operator double() const { return this->valuedouble; }
	operator char*() const { return this->valuestring; }
	operator const char*() const { return this->valuestring; }
	operator ERaDataJson*() const { return this->valueobject; }
	operator const ERaDataJson*() const { return this->valueobject; }

protected:
private:
	template <typename T>
	void addParam(T value) {
		this->valueint = (int)value;
		this->valuedouble = (double)value;
		this->setType(ERaParamTypeT::ERA_PARAM_TYPE_NUMBER, true);
	}

	void addParam(const ERaParam& value) {
		if (this == &value) {
			return;
		}
		this->free();
		this->type = value.type;
		this->valueint = value.valueint;
		this->valuedouble = value.valuedouble;
		if (this->getType(ERaParamTypeT::ERA_PARAM_TYPE_STATIC_STRING)) {
			this->valuestring = value.valuestring;
		}
		else if (this->getType(ERaParamTypeT::ERA_PARAM_TYPE_STRING)) {
			this->valuestring = ERaStrdup(value.valuestring);
		}
		this->valueobject = value.valueobject;
	}

	void addParam(char* value) {
		this->free();
		this->valuestring = ERaStrdup(value);
		this->setType(ERaParamTypeT::ERA_PARAM_TYPE_STRING, true);
	}

	void addParam(const char* value) {
		this->free();
		this->valuestring = ERaStrdup(value);
		this->setType(ERaParamTypeT::ERA_PARAM_TYPE_STRING, true);
	}

	void addParam(cJSON* value) {
		this->free();
		this->valuestring = cJSON_PrintUnformatted(value);
		this->setType(ERaParamTypeT::ERA_PARAM_TYPE_STRING, true);
	}

	void addParam(const cJSON* value) {
		this->free();
		this->valuestring = cJSON_PrintUnformatted(value);
		this->setType(ERaParamTypeT::ERA_PARAM_TYPE_STRING, true);
	}

	void addParam(ERaDataJson& value) {
		this->valueobject = &value;
		this->setType(ERaParamTypeT::ERA_PARAM_TYPE_OBJECT, true);
	}

	void free() {
		if (this->getType(ERaParamTypeT::ERA_PARAM_TYPE_STRING) &&
			this->valuestring != nullptr) {
			::free(this->valuestring);
		}
		this->valuestring = nullptr;
		this->setType(ERaParamTypeT::ERA_PARAM_TYPE_STRING, false);
		this->setType(ERaParamTypeT::ERA_PARAM_TYPE_STATIC_STRING, false);
	}

    void setType(uint8_t mask, bool value) {
        if (value) {
            this->type |= mask;
        }
        else {
            this->type &= ~mask;
        }
    }

    bool getType(uint8_t mask) const {
        return (this->type & mask) == mask;
    }

	uint8_t type;
	int valueint;
	double valuedouble;
	char* valuestring;
	ERaDataJson* valueobject;
};

inline
void ERaDataJson::add(const char* name, ERaParam& value) {
	if (value.isString()) {
		this->add(name, value.getString());
	}
	else if (value.isNumber()) {
		this->add(name, value.getDouble());
	}
}

#endif /* INC_ERA_PARAM_HPP_ */
