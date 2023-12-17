#ifndef INC_ERA_CJSON_HPP_
#define INC_ERA_CJSON_HPP_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <Utility/cJSON.hpp>

#define MAX_CJSON_PARSE     10

#define ERaSetBool(...)     cJSON_SetBoolToObject(__VA_ARGS__)
#define ERaSetNumber(...)   cJSON_SetNumberToObject(__VA_ARGS__)
#define ERaSetString(...)   cJSON_SetStringToObject(__VA_ARGS__)
#define ERaDeleteItem(...)  cJSON_DeleteItemFromObject(__VA_ARGS__)

CJSON_PUBLIC(cJSON*) cJSONUtils_GetPointer(cJSON* const object, const char* pointer);
CJSON_PUBLIC(cJSON*) cJSONUtils_GetPointerCaseSensitive(cJSON* const object, const char* pointer);

CJSON_PUBLIC(cJSON*) cJSON_ParseWithLimit(const char* value, size_t limit = MAX_CJSON_PARSE);
CJSON_PUBLIC(cJSON*) cJSON_AddNumberWithDecimalToObject(cJSON* const object, const char* const name, const double number, int decimal);
CJSON_PUBLIC(cJSON*) cJSON_CreateNumberWithDecimalToObject(const double number, int decimal);
CJSON_PUBLIC(cJSON*) cJSON_CreateUint8Array(const uint8_t* numbers, int count);
CJSON_PUBLIC(cJSON_bool) cJSON_InsertItemInObject(cJSON* object, const char* string, cJSON* newitem);
CJSON_PUBLIC(cJSON*) cJSON_SetNumberWithDecimalToObject(cJSON* const object, const char* const name, const double number, int decimal);
CJSON_PUBLIC(cJSON*) cJSON_SetBoolToObject(cJSON* const object, const char* const name, const cJSON_bool boolean);
CJSON_PUBLIC(cJSON*) cJSON_SetNumberToObject(cJSON* const object, const char* const name, const double number);
CJSON_PUBLIC(cJSON*) cJSON_SetStringToObject(cJSON* const object, const char* const name, const char* const string);
CJSON_PUBLIC(cJSON*) cJSON_SetNullToObject(cJSON* const object, const char* const name);
CJSON_PUBLIC(cJSON*) cJSON_SetNumberWithDecimalViaPointer(cJSON* const object, cJSON* const item, const double number, int decimal);
CJSON_PUBLIC(cJSON*) cJSON_SetBoolViaPointer(cJSON* const object, cJSON* const item, const cJSON_bool boolean);
CJSON_PUBLIC(cJSON*) cJSON_SetNumberViaPointer(cJSON* const object, cJSON* const item, const double number);
CJSON_PUBLIC(cJSON*) cJSON_SetStringViaPointer(cJSON* const object, cJSON* const item, const char* const string);
CJSON_PUBLIC(cJSON*) cJSON_SetNullViaPointer(cJSON* const object, cJSON* const item);
CJSON_PUBLIC(cJSON*) cJSON_SetNumberWithDecimal(cJSON* const object, cJSON* const item, const double number, int decimal);
CJSON_PUBLIC(cJSON*) cJSON_SetBool(cJSON* const object, cJSON* const item, const cJSON_bool boolean);
CJSON_PUBLIC(cJSON*) cJSON_SetNumber(cJSON* const object, cJSON* const item, const double number);
CJSON_PUBLIC(cJSON*) cJSON_SetString(cJSON* const object, cJSON* const item, const char* const string);
CJSON_PUBLIC(cJSON*) cJSON_SetNull(cJSON* const object, cJSON* const item);
CJSON_PUBLIC(cJSON*) cJSON_GetArrayIndex(cJSON* const object, int index);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItem(cJSON* const parent, cJSON* const item, cJSON* replacement);
CJSON_PUBLIC(cJSON_bool) cJSON_Empty(const cJSON* const object);

CJSON_PUBLIC(cJSON*) cJSON_AddMultiItemToObject(cJSON* const object, const char* const name, const double number);
CJSON_PUBLIC(cJSON*) cJSON_AddMultiItemToObject(cJSON* const object, const char* const name, const char* const string);

template <typename... Args>
inline
void cJSON_AddMultiItemToObject(cJSON* const object, const char* const name,
                                const double number, const Args... tail) {
    cJSON_AddMultiItemToObject(object, name, number);
    cJSON_AddMultiItemToObject(object, tail...);
}

template <typename... Args>
inline
void cJSON_AddMultiItemToObject(cJSON* const object, const char* const name,
                                const char* const string, const Args... tail) {
    cJSON_AddMultiItemToObject(object, name, string);
    cJSON_AddMultiItemToObject(object, tail...);
}

#endif /* INC_ERA_CJSON_HPP_ */
