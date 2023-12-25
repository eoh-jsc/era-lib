#include <math.h>
#include <ctype.h>
#include <Utility/ERacJSON.hpp>
#include <Utility/ERaUtility.hpp>

using namespace std;

/* Utility for array list handling. */
static void suffix_object(cJSON* prev, cJSON* item) {
    prev->next = item;
    item->prev = prev;
}

/* Compare the next path element of two JSON pointers, two NULL pointers are considered unequal: */
static cJSON_bool compare_pointers(const unsigned char* name, const unsigned char* pointer, const cJSON_bool case_sensitive) {
    if ((name == NULL) || (pointer == NULL)) {
        return false;
    }

    for (; (*name != '\0') && (*pointer != '\0') && (*pointer != '/'); (void)name++, pointer++) { /* compare until next '/' */
        if (*pointer == '~') {
            /* check for escaped '~' (~0) and '/' (~1) */
            if (((pointer[1] != '0') || (*name != '~')) && ((pointer[1] != '1') || (*name != '/'))) {
                /* invalid escape sequence or wrong character in *name */
                return false;
            }
            else {
                pointer++;
            }
        }
        else if ((!case_sensitive && (tolower(*name) != tolower(*pointer))) || (case_sensitive && (*name != *pointer))) {
            return false;
        }
    }
    if (((*pointer != 0) && (*pointer != '/')) != (*name != 0)) {
        /* one string has ended, the other not */
        return false;;
    }

    return true;
}

/* non broken version of cJSON_GetArrayItem */
static cJSON* get_array_item(const cJSON* array, size_t item) {
    cJSON* child = array ? array->child : NULL;
    while ((child != NULL) && (item > 0)) {
        item--;
        child = child->next;
    }

    return child;
}

static cJSON_bool decode_array_index_from_pointer(const unsigned char* const pointer, size_t* const index) {
    size_t parsed_index = 0;
    size_t position = 0;

    if ((pointer[0] == '0') && ((pointer[1] != '\0') && (pointer[1] != '/'))) {
        /* leading zeroes are not permitted */
        return 0;
    }

    for (position = 0; (pointer[position] >= '0') && (pointer[0] <= '9'); position++) {
        parsed_index = (10 * parsed_index) + (size_t)(pointer[position] - '0');
    }

    if ((pointer[position] != '\0') && (pointer[position] != '/')) {
        return 0;
    }

    *index = parsed_index;

    return 1;
}

static cJSON* get_item_from_pointer(cJSON* const object, const char* pointer, const cJSON_bool case_sensitive) {
    cJSON* current_element = object;

    if (pointer == NULL) {
        return NULL;
    }

    /* follow path of the pointer */
    while ((pointer[0] == '/') && (current_element != NULL)) {
        pointer++;
        if (cJSON_IsArray(current_element)) {
            size_t index = 0;
            if (!decode_array_index_from_pointer((const unsigned char*)pointer, &index)) {
                return NULL;
            }

            current_element = get_array_item(current_element, index);
        }
        else if (cJSON_IsObject(current_element)) {
            current_element = current_element->child;
            /* GetObjectItem. */
            while ((current_element != NULL) && !compare_pointers((unsigned char*)current_element->string, (const unsigned char*)pointer, case_sensitive)) {
                current_element = current_element->next;
            }
        }
        else {
            return NULL;
        }

        /* skip to the next path token or end of string */
        while ((pointer[0] != '\0') && (pointer[0] != '/')) {
            pointer++;
        }
    }

    return current_element;
}

CJSON_PUBLIC(cJSON*) cJSONUtils_GetPointer(cJSON* const object, const char* pointer) {
    return get_item_from_pointer(object, pointer, false);
}

CJSON_PUBLIC(cJSON*) cJSONUtils_GetPointerCaseSensitive(cJSON* const object, const char* pointer) {
    return get_item_from_pointer(object, pointer, true);
}

CJSON_PUBLIC(cJSON*) cJSON_ParseWithLimit(const char* value, size_t limit) {
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
CJSON_PUBLIC(cJSON*) cJSON_CreateUint8Array(const uint8_t* numbers, int count) {
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

/* Insert object items with new ones. */
CJSON_PUBLIC(cJSON_bool) cJSON_InsertItemInObject(cJSON* object, const char* string, cJSON* newitem) {
    cJSON *after_inserted = NULL;

    if (newitem == NULL) {
        return false;
    }

    after_inserted = cJSON_GetObjectItem(object, string);
    if (after_inserted == NULL) {
        return false;
    }

    newitem->next = after_inserted;
    newitem->prev = after_inserted->prev;
    after_inserted->prev = newitem;
    if (after_inserted == object->child) {
        object->child = newitem;
    }
    else {
        newitem->prev->next = newitem;
    }
    return true;
}

CJSON_PUBLIC(cJSON*) cJSON_SetNumberWithDecimalToObject(cJSON* const object, const char* const name, const double number, int decimal) {
    cJSON *number_item = cJSON_GetObjectItem(object, name);
    if (number_item == NULL) {
        number_item = cJSON_AddNumberWithDecimalToObject(object, name, number, decimal);
    }
    else {
        number_item = cJSON_CreateNumberWithDecimalToObject(number, decimal);
        cJSON_ReplaceItemInObject(object, name, number_item);
    }
    return number_item;
}

CJSON_PUBLIC(cJSON*) cJSON_SetBoolToObject(cJSON* const object, const char* const name, const cJSON_bool boolean) {
    cJSON *bool_item = cJSON_GetObjectItem(object, name);
    if (bool_item == NULL) {
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

CJSON_PUBLIC(cJSON*) cJSON_SetNumberToObject(cJSON* const object, const char* const name, const double number) {
    cJSON *number_item = cJSON_GetObjectItem(object, name);
    if (number_item == NULL) {
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

CJSON_PUBLIC(cJSON*) cJSON_SetStringToObject(cJSON* const object, const char* const name, const char* const string) {
    cJSON *string_item = cJSON_GetObjectItem(object, name);
    if (string_item == NULL) {
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

CJSON_PUBLIC(cJSON*) cJSON_SetNullToObject(cJSON* const object, const char* const name) {
    cJSON *null_item = cJSON_GetObjectItem(object, name);
    if (null_item == NULL) {
        return cJSON_AddNullToObject(object, name);
    }
    if (cJSON_IsNull(null_item)) {
    }
    else {
        null_item = cJSON_CreateNull();
        cJSON_ReplaceItemInObject(object, name, null_item);
    }
    return null_item;
}

CJSON_PUBLIC(cJSON*) cJSON_SetNumberWithDecimalViaPointer(cJSON* const object, cJSON* const item, const double number, int decimal) {
    cJSON *number_item = cJSON_CreateNumberWithDecimalToObject(number, decimal);
    cJSON_ReplaceItemViaPointer(object, item, number_item);
    return number_item;
}

CJSON_PUBLIC(cJSON*) cJSON_SetBoolViaPointer(cJSON* const object, cJSON* const item, const cJSON_bool boolean) {
    cJSON *bool_item = item;
    if (cJSON_IsBool(bool_item)) {
        cJSON_SetBoolValue(bool_item, boolean);
    }
    else {
        bool_item = cJSON_CreateBool(boolean);
        cJSON_ReplaceItemViaPointer(object, item, bool_item);
    }
    return bool_item;
}

CJSON_PUBLIC(cJSON*) cJSON_SetNumberViaPointer(cJSON* const object, cJSON* const item, const double number) {
    cJSON *number_item = item;
    if (cJSON_IsNumber(number_item)) {
        cJSON_SetNumberValue(number_item, number);
    }
    else {
        number_item = cJSON_CreateNumber(number);
        cJSON_ReplaceItemViaPointer(object, item, number_item);
    }
    return number_item;
}

CJSON_PUBLIC(cJSON*) cJSON_SetStringViaPointer(cJSON* const object, cJSON* const item, const char* const string) {
    cJSON *string_item = item;
    if (cJSON_IsString(string_item)) {
        cJSON_SetValuestring(string_item, string);
    }
    else {
        string_item = cJSON_CreateString(string);
        cJSON_ReplaceItemViaPointer(object, item, string_item);
    }
    return string_item;
}

CJSON_PUBLIC(cJSON*) cJSON_SetNullViaPointer(cJSON* const object, cJSON* const item) {
    cJSON *null_item = item;
    if (cJSON_IsNull(null_item)) {
    }
    else {
        null_item = cJSON_CreateNull();
        cJSON_ReplaceItemViaPointer(object, item, null_item);
    }
    return null_item;
}

CJSON_PUBLIC(cJSON*) cJSON_SetNumberWithDecimal(cJSON* const object, cJSON* const item, const double number, int decimal) {
    if (cJSON_IsArray(object)) {
        return cJSON_SetNumberWithDecimalViaPointer(object, item, number, decimal);
    }
    else {
        return cJSON_SetNumberWithDecimalToObject(object, item->string, number, decimal);
    }
}

CJSON_PUBLIC(cJSON*) cJSON_SetBool(cJSON* const object, cJSON* const item, const cJSON_bool boolean) {
    if (cJSON_IsArray(object)) {
        return cJSON_SetBoolViaPointer(object, item, boolean);
    }
    else {
        return cJSON_SetBoolToObject(object, item->string, boolean);
    }
}

CJSON_PUBLIC(cJSON*) cJSON_SetNumber(cJSON* const object, cJSON* const item, const double number) {
    if (cJSON_IsArray(object)) {
        return cJSON_SetNumberViaPointer(object, item, number);
    }
    else {
        return cJSON_SetNumberToObject(object, item->string, number);
    }
}

CJSON_PUBLIC(cJSON*) cJSON_SetString(cJSON* const object, cJSON* const item, const char* const string) {
    if (cJSON_IsArray(object)) {
        return cJSON_SetStringViaPointer(object, item, string);
    }
    else {
        return cJSON_SetStringToObject(object, item->string, string);
    }
}

CJSON_PUBLIC(cJSON*) cJSON_SetNull(cJSON* const object, cJSON* const item) {
    if (cJSON_IsArray(object)) {
        return cJSON_SetNullViaPointer(object, item);
    }
    else {
        return cJSON_SetNullToObject(object, item->string);
    }
}

CJSON_PUBLIC(cJSON*) cJSON_GetArrayIndex(cJSON* const object, int index) {
    cJSON* item = cJSON_GetArrayItem(object, index);
    if (item == nullptr) {
        while (index >= cJSON_GetArraySize(object)) {
            item = cJSON_CreateNull();

            cJSON_AddItemToArray(object, item);
        }
    }
    return item;
}

CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItem(cJSON* const parent, cJSON* const item, cJSON* replacement) {
    if (cJSON_IsArray(parent)) {
        return cJSON_ReplaceItemViaPointer(parent, item, replacement);
    }
    else {
        return cJSON_ReplaceItemInObject(parent, item->string, replacement);
    }
}

CJSON_PUBLIC(cJSON_bool) cJSON_Empty(const cJSON * const object) {
    return (cJSON_IsNull(object) || (cJSON_IsString(object) && !strlen(object->valuestring)));
}

CJSON_PUBLIC(cJSON*) cJSON_AddMultiItemToObject(cJSON* const object, const char* const name, const double number) {
    if ((object == NULL) || (name == NULL)) {
        return NULL;
    }
    return cJSON_AddNumberToObject(object, name, number);
}

CJSON_PUBLIC(cJSON*) cJSON_AddMultiItemToObject(cJSON* const object, const char* const name, const char* const string) {
    if ((object == NULL) || (name == NULL) || (string == NULL)) {
        return NULL;
    }
    return cJSON_AddStringToObject(object, name, string);
}
