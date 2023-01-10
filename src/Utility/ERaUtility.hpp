#ifndef INC_ERA_UTILITY_HPP_
#define INC_ERA_UTILITY_HPP_

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <ERa/ERaDefine.hpp>

#if defined(ARDUINO_ARCH_ARC32)
    typedef uint64_t MillisTime_t;
#else
    typedef uint32_t MillisTime_t;
#endif

typedef void* ERaMutex_t;

void ERaDelay(MillisTime_t ms);
MillisTime_t ERaMillis();
uint32_t ERaRandomNumber(uint32_t min, uint32_t max);
size_t ERaFreeRam();
void ERaRestart(bool async);

void ERaGuardLock(ERaMutex_t& mutex);
void ERaGuardUnlock(ERaMutex_t& mutex);

char* ERaStrdup(const char* str);
MillisTime_t ERaRemainingTime(MillisTime_t startMillis, MillisTime_t timeout);

template<typename T>
inline
const T& ERaMin(const T& a, const T& b) {
    return (b < a) ? b : a;
}

template<typename T>
inline
const T& ERaMax(const T& a, const T& b) {
    return (b < a) ? a : b;
}

template<typename T>
inline
T ERaMapNumberRange(T value, T fromLow, T fromHigh, T toLow, T toHigh) {
	return (toLow + (ERaMin(value, fromHigh) - ERaMin(value, fromLow)) * (toHigh - toLow) / (fromHigh - fromLow));
}

#if defined(ARDUINO) && defined(ESP32)
    #include "driver/uart.h"
    #include <Utility/ERaOs.hpp>
#elif defined(ARDUINO) &&           \
    (defined(ARDUINO_ARCH_STM32) || \
    defined(ARDUINO_ARCH_RP2040))
    #include <Utility/ERaOs.hpp>
#endif

#if defined(ARDUINO)
    #include <Arduino.h>

    template <typename T, int size>
    inline
    void CopyToArray(const String& src, T(&dst)[size]) {
        src.toCharArray(dst, size);
    }
#endif

template <typename T, int len, int size>
inline
void CopyToArray(const char(&src)[len], T(&dst)[size]) {
    memcpy(dst, src, ERaMin(len, size));
}

template <typename T, int size>
inline
void CopyToArray(const uint8_t& src, T(&dst)[size]) {
    memcpy(dst, &src, size);
}

template <typename T, int size>
inline
void CopyToArray(const uint8_t& src, T(&dst)[size], int len) {
    memcpy(dst, &src, ERaMin(len, size));
}

template <typename T>
inline
void CopyToStruct(const uint8_t& src, T& dst, int size) {
    memcpy(&dst, &src, size);
}

template <typename T>
inline
void ClearStruct(T& arr, int size) {
    memset(&arr, 0, size);
}

template <typename T, int size>
inline
void ClearArray(T(&arr)[size]) {
    memset(arr, 0, sizeof(T) * size);
}

template <typename... Args>
inline
void FormatString(char* buf, size_t len, Args... tail) {
    if (buf == nullptr) {
        return;
    }
    snprintf(buf + strlen(buf), len - strlen(buf), tail...);
}

template <int size>
inline
void FormatString(char(&buf)[size], const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(buf + strlen(buf), size - strlen(buf), format, args);
    va_end(args);
}

bool ERaStrCmp(const char* str, const char* str2);

template <int size>
inline
bool ERaStrNCmp(const char* str, const char(&str2)[size]) {
    return !strncmp(str, str2, size - 1);
}

#if !_GLIBCXX_USE_C99_STDLIB
    #include <sstream>

    template<typename T>
    inline
    std::string to_string(const T& n)
    {
        std::ostringstream ss;
        ss << n;
        return ss.str();
    }
#endif

#include <ERa/ERaDebug.hpp>

#endif /* INC_ERA_UTILITY_HPP_ */
