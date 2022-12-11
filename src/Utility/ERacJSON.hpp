#ifndef INC_ERA_CJSON_HPP_
#define INC_ERA_CJSON_HPP_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <Utility/cJSON.hpp>

#define MAX_CJSON_PARSE    10

CJSON_PUBLIC(cJSON*) cJSON_ParseWithLimit(const char *value, size_t limit = MAX_CJSON_PARSE);
CJSON_PUBLIC(cJSON*) cJSON_AddNumberWithDecimalToObject(cJSON* const object, const char* const name, const double number, int decimal);
CJSON_PUBLIC(cJSON*) cJSON_CreateNumberWithDecimalToObject(const double number, int decimal);
CJSON_PUBLIC(cJSON*) cJSON_CreateUint8Array(const uint8_t *numbers, int count);
CJSON_PUBLIC(cJSON*) cJSON_SetNumberWithDecimalToObject(cJSON* const object, const char* const name, const double number, int decimal);
CJSON_PUBLIC(cJSON*) cJSON_SetBoolToObject(cJSON * const object, const char * const name, const cJSON_bool boolean);
CJSON_PUBLIC(cJSON*) cJSON_SetNumberToObject(cJSON * const object, const char * const name, const double number);
CJSON_PUBLIC(cJSON*) cJSON_SetStringToObject(cJSON * const object, const char * const name, const char * const string);
CJSON_PUBLIC(cJSON*) cJSON_SetNullToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON_bool) cJSON_Empty(const cJSON * const object);

#endif /* INC_ERA_CJSON_HPP_ */
