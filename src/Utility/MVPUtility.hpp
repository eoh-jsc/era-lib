#ifndef INC_MVP_UTILITY_HPP_
#define INC_MVP_UTILITY_HPP_

#include <stdint.h>
#include <MVP/MVPDefine.hpp>

#if defined(ARDUINO_ARCH_ARC32)
    typedef uint64_t MillisTime_t;
#else
    typedef uint32_t MillisTime_t;
#endif

typedef void* MVPMutex_t;

void MVPDelay(MillisTime_t ms);
MillisTime_t MVPMillis();
uint32_t MVPRandomNumber(uint32_t min, uint32_t max);
size_t MVPFreeRam();
void MVPRestart(bool async);

void MVPGuardLock(MVPMutex_t& mutex) MVP_WEAK;
void MVPGuardUnlock(MVPMutex_t& mutex) MVP_WEAK;

char* MVPStrdup(const char* str);
MillisTime_t MVPRemainingTime(MillisTime_t prevMillis, MillisTime_t timeout);

template<typename T>
const T& MVPMin(const T& a, const T& b) {
    return (b < a) ? b : a;
}

template<typename T>
const T& MVPMax(const T& a, const T& b) {
    return (b < a) ? a : b;
}

template<typename T>
T MVPMapNumberRange(T value, T fromLow, T fromHigh, T toLow, T toHigh) {
	return (toLow + (MVPMin(value, fromHigh) - MVPMin(value, fromLow)) * (toHigh - toLow) / (fromHigh - fromLow));
}

#if defined(ARDUINO) && defined(ESP32)
    #include "driver/uart.h"
    #include <Utility/MVPos.hpp>
#endif

#if defined(ARDUINO)
    #include <Arduino.h>

    template <typename T, int size>
    void CopyToArray(const String& str, T(&arr)[size]) {
        str.toCharArray(arr, size);
    }
#endif

template <typename T, int len, int size>
void CopyToArray(const char(&ptr)[len], T(&arr)[size]) {
    memcpy(arr, ptr, MVPMin(len, size));
}

template <typename T, int size>
void CopyToArray(const uint8_t& ptr, T(&arr)[size], int len) {
    memcpy(arr, &ptr, MVPMin(len, size));
}

template <typename T>
void CopyToStruct(const uint8_t& ptr, T& arr, int size) {
    memcpy(&arr, &ptr, size);
}

template <typename T>
void ClearStruct(T& arr, int size) {
    memset(&arr, 0, size);
}

template <typename T, int size>
void ClearArray(T(&arr)[size]) {
    memset(arr, 0, size);
}

template <typename... Args>
void FormatString(char* buf, size_t len, Args... tail) {
    if (buf == nullptr) {
        return;
    }
    snprintf(buf + strlen(buf), len - strlen(buf), tail...);
}

template <int size>
void FormatString(char(&buf)[size], const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(buf + strlen(buf), size - strlen(buf), format, args);
    va_end(args);
}

bool MVPStrCmp(const char* str, const char* str2);

template <int size>
bool MVPStrNCmp(const char* str, const char(&str2)[size]) {
    return !strncmp(str, str2, size);
}

#if !_GLIBCXX_USE_C99_STDLIB
    #include <sstream>

    template<typename T>
    std::string to_string(const T& n)
    {
        std::ostringstream ss;
        ss << n;
        return ss.str();
    }
#endif

#include <MVP/MVPDebug.hpp>

#endif /* INC_MVP_UTILITY_HPP_ */