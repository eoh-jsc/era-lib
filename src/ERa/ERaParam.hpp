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
	~ERaParam()
	{}

	template <typename T>
	void add(T value) {
		addParam(value);
	}

	template <typename T>
	void operator= (T value) {
		add(value);
	}

	operator bool() const { return this->valueint; }
	operator int() const { return this->valueint; }
	operator float() const { return static_cast<float>(this->valuedouble); }
	operator double() const { return this->valuedouble; }
	operator char*() const { return this->valuestring; }

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
};

#endif /* INC_ERA_PARAM_HPP_ */
