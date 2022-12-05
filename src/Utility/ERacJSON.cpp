#include <math.h>
#include <Utility/ERacJSON.hpp>
#include <Utility/ERaUtility.hpp>

using namespace std;

/* Utility for array list handling. */
static void suffix_object(cJSON *prev, cJSON *item) {
    prev->next = item;
    item->prev = prev;
}

CJSON_PUBLIC(cJSON*) cJSON_ParseWithLimit(const char *value, size_t limit) {
    cJSON* root = nullptr;
    for (size_t i = 0; i < limit; ++i) {
        root = cJSON_Parse(value);
        if (root != nullptr && cJSON_GetErrorPtr() == nullptr) {
            break;
        }
        ERaDelay(1000);
    }
    return root;
}

CJSON_PUBLIC(cJSON*) cJSON_AddNumberWithDecimalToObject(cJSON* const object, const char* const name, const double number, int decimal) {
	if (round(number) == number || decimal <= 0) {
		return cJSON_AddNumberToObject(object, name, number);
    }

	char number_buffer[26] {0};
	char number_format[10] {0};
	unsigned int number_length {0};
	double n = number;
	long long d = (long long)number;

	do {
		++number_length;
		d /= 10;
	} while (d);

	while (abs(n) < 1.f && --decimal) {
		n *= 10;
	}

	snprintf(number_format, sizeof(number_format), "%%.%dg", decimal + number_length);
	snprintf(number_buffer, sizeof(number_buffer), number_format, number);
	return cJSON_AddRawNumberToObject(object, name, number_buffer);
}

CJSON_PUBLIC(cJSON*) cJSON_CreateNumberWithDecimalToObject(const double number, int decimal) {
	if (round(number) == number || decimal <= 0) {
		return cJSON_CreateNumber(number);
    }

	char number_buffer[26] {0};
	char number_format[10] {0};
	unsigned int number_length {0};
	double n = number;
	long long d = (long long)number;

	do {
		++number_length;
		d /= 10;
	} while (d);

	while (abs(n) < 1.f && --decimal) {
		n *= 10;
	}

	snprintf(number_format, sizeof(number_format), "%%.%dg", decimal + number_length);
	snprintf(number_buffer, sizeof(number_buffer), number_format, number);
	return cJSON_CreateRawNumber(number_buffer);
}

/* Create Arrays: */
CJSON_PUBLIC(cJSON*) cJSON_CreateUint8Array(const uint8_t *numbers, int count) {
    size_t i = 0;
    cJSON *n = NULL;
    cJSON *p = NULL;
    cJSON *a = NULL;

    if ((count < 0) || (numbers == NULL)) {
        return NULL;
    }

    a = cJSON_CreateArray();

    for (i = 0; a && (i < (size_t)count); ++i) {
        n = cJSON_CreateNumber(numbers[i]);
        if (!n) {
            cJSON_Delete(a);
            return NULL;
        }
        if (!i) {
            a->child = n;
        }
        else {
            suffix_object(p, n);
        }
        p = n;
    }

    if (a && a->child) {
        a->child->prev = n;
    }

    return a;
}

CJSON_PUBLIC(cJSON*) cJSON_SetNumberWithDecimalToObject(cJSON* const object, const char* const name, const double number, int decimal) {
    cJSON *number_item = cJSON_GetObjectItem(object, name);
    if(number_item == NULL) {
        number_item = cJSON_AddNumberWithDecimalToObject(object, name, number, decimal);
    }
    else {
        number_item = cJSON_CreateNumberWithDecimalToObject(number, decimal);
        cJSON_ReplaceItemInObject(object, name, number_item);
    }
    return number_item;
}

CJSON_PUBLIC(cJSON*) cJSON_SetBoolToObject(cJSON * const object, const char * const name, const cJSON_bool boolean) {
    cJSON *bool_item = cJSON_GetObjectItem(object, name);
    if(bool_item == NULL) {
        return cJSON_AddBoolToObject(object, name, boolean);
    }
    if (cJSON_IsBool(bool_item)) {
        cJSON_SetBoolValue(bool_item, boolean);
    }
    else {
        bool_item = cJSON_CreateBool(boolean);
        cJSON_ReplaceItemInObject(object, name, bool_item);
    }
    return bool_item;
}

CJSON_PUBLIC(cJSON*) cJSON_SetNumberToObject(cJSON * const object, const char * const name, const double number) {
    cJSON *number_item = cJSON_GetObjectItem(object, name);
    if(number_item == NULL) {
        return cJSON_AddNumberToObject(object, name, number);
    }
    if (cJSON_IsNumber(number_item)) {
        cJSON_SetNumberValue(number_item, number);
    }
    else {
        number_item = cJSON_CreateNumber(number);
        cJSON_ReplaceItemInObject(object, name, number_item);
    }
    return number_item;
}

CJSON_PUBLIC(cJSON*) cJSON_SetStringToObject(cJSON * const object, const char * const name, const char * const string) {
    cJSON *string_item = cJSON_GetObjectItem(object, name);
    if(string_item == NULL) {
        return cJSON_AddStringToObject(object, name, string);
    }
    if (cJSON_IsString(string_item)) {
        cJSON_SetValuestring(string_item, string);
    }
    else {
        string_item = cJSON_CreateString(string);
        cJSON_ReplaceItemInObject(object, name, string_item);
    }
    return string_item;
}

CJSON_PUBLIC(cJSON*) cJSON_SetNullToObject(cJSON * const object, const char * const name) {
    cJSON *null_item = cJSON_GetObjectItem(object, name);
    if(null_item == NULL) {
        return cJSON_AddNullToObject(object, name);
    }
    return null_item;
}

CJSON_PUBLIC(cJSON_bool) cJSON_Empty(const cJSON * const object) {
    return (cJSON_IsNull(object) || (cJSON_IsString(object) && !strlen(object->valuestring)));
}
