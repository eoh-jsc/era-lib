#ifndef INC_ERA_PARAM_HPP_
#define INC_ERA_PARAM_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ERa/ERaDefine.hpp>
#include <Utility/ERacJSON.hpp>
#include <Utility/ERaUtility.hpp>

class ERaParam
{
public:
	ERaParam()
		: valueint(0)
		, valuedouble(0)
		, valuestring(nullptr)
	{}
	template <typename T>
	ERaParam(T value)
		: valueint(0)
		, valuedouble(0)
		, valuestring(nullptr)
	{
		add(value);
	}
	~ERaParam()
	{
		this->valueint = 0;
		this->valuedouble = 0;
		this->valuestring = nullptr;
	}

	template <typename T>
	void add(T value) {
		addParam(value);
	}

	void add_static(char* value) {
		this->valuestring = value;
	}

	void add_static(const char* value) {
		this->valuestring = const_cast<char*>(value);
	}

	template <typename T>
	void operator= (T value) {
		add(value);
	}

	template <typename T>
	operator T() const { return static_cast<T>(this->valueint); }

	operator float() const { return static_cast<float>(this->valuedouble); }
	operator double() const { return this->valuedouble; }
	operator char*() const { return this->valuestring; }
	operator const char*() const { return this->valuestring; }

protected:
private:
	int valueint;
	double valuedouble;
	char* valuestring;

	template <typename T>
	void addParam(T value) {
		this->valueint = (int)value;
		this->valuedouble = value;
	}

	void addParam(char* value) {
		this->valuestring = ERaStrdup(value);
	}

	void addParam(const char* value) {
		this->valuestring = ERaStrdup(value);
	}

	void addParam(cJSON* value) {
		this->valuestring = cJSON_PrintUnformatted(value);
	}

	void addParam(const cJSON* value) {
		this->valuestring = cJSON_PrintUnformatted(value);
	}
};

#endif /* INC_ERA_PARAM_HPP_ */
