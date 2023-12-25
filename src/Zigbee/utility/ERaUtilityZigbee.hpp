#ifndef INC_ERA_UTILITY_ZIGBEE_HPP_
#define INC_ERA_UTILITY_ZIGBEE_HPP_

#include <Zigbee/ERaZigbee.hpp>

#define MAX_TOPIC_LENGTH 100

template <int size>
inline
bool CompareArray(const uint8_t* arr, const uint8_t(&arr2)[size]) {
    return !memcmp(arr, arr2, size);
}

inline
bool CompareString(const char* str, const char* str2) {
    return !strcmp(str, str2);
}

inline
bool CompareNString(const char* str, const char* str2, int size) {
    return !strncmp(str, str2, size);
}

template <int size>
inline
bool CompareNString(const char* str, const char(&str2)[size]) {
    return !strncmp(str, str2, size - 1);
}

template <int len, int size>
inline
void CopyArray(const uint8_t(&src)[len], uint8_t(&dst)[size]) {
    memcpy(dst, src, std::min(len, size));
}

template <int size>
inline
void CopyString(const char* src, char(&dst)[size]) {
    snprintf(dst, size, "%s", src);
}

template <typename... Args>
inline
void StringPrint(char* buf, size_t len, Args... tail) {
    if (buf == nullptr) {
        return;
    }
    snprintf(buf + strlen(buf), len - strlen(buf), tail...);
}

template <int size, typename... Args>
inline
void StringPrint(char(&buf)[size], Args... tail) {
    snprintf(buf + strlen(buf), size - strlen(buf), tail...);
}

inline
void ClearMem(void* ptr, size_t size) {
    memset(ptr, 0, size);
}

template <typename T>
inline
void ClearMem(T& it) {
    memset(&it, 0, sizeof(T));
}

template <typename T, int size>
inline
void ClearMem(T(&arr)[size]) {
    memset(arr, 0, sizeof(T) * size);
}

template <typename T, int size>
inline
bool IsZeroArray(const T(&data)[size]) {
    for (size_t i = 0; i < size; ++i) {
        if (data[i]) {
            return false;
        }
    }
    return true;
}

inline
bool IsDigit(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    size_t position {0};
    if (str.at(0) == '-' || str.at(0) == '+') {
        position = 1;
        if (str.size() < 2) {
            return false;
        }
    }
    for (auto i = str.begin() + position; i != str.end(); ++i) {
        if (!isdigit(*i)) {
            return false;
        }
    }
    return true;
}

inline
bool IsHexDigit(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    for (auto const& var : str) {
        if (!isxdigit(var)) {
            return false;
        }
    }
    return true;
}

template <typename T, int size>
inline
std::string IEEEToString(const T(&arr)[size]) {
    char str[20] {0};
    strcat(str, "0x");
    for (int i = 0; i < size; ++i) {
        snprintf(&str[strlen(str)], sizeof(str) - 2 * i - 2, "%02x", arr[size - i - 1]);
    }
    return str;
}

template <typename T, int size>
inline
bool StringToIEEE(const char* str, T(&ieee)[size]) {
    if (str == nullptr) {
        return false;
    }
    if (strlen(str) != 18 || !CompareNString(str, "0x")) {
        return false;
    }
    if (size != LENGTH_EXTADDR_IEEE) {
        return false;
    }
    std::string hex;
    for(int i = 0; i < size; ++i) {
        hex.assign(str, 2 * i + 2, 2);
        if(IsHexDigit(hex)) {
            ieee[size - i - 1] = strtol(hex.c_str(), nullptr, 16);
        }
    }
    return true;
}

#endif /* INC_ERA_UTILITY_ZIGBEE_HPP_ */
