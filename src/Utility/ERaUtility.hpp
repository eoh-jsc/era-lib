#ifndef INC_ERA_UTILITY_HPP_
#define INC_ERA_UTILITY_HPP_

#include <ctype.h>
#include <stdio.h>
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
MillisTime_t ERaSeconds();
uint32_t ERaRandomNumber(uint32_t min, uint32_t max);
size_t ERaFreeRam();
void ERaRestart(bool async) ERA_NORETURN;
void ERaFatality() ERA_NORETURN;

#if !defined(ERA_NO_RTOS)
    #define ERaGuardAuto(mutex)     const ERaGuard ERA_CONCAT(guard, __LINE__)(mutex)
    #define ERaGuardLock(mutex)     ERaGuardLockFn(mutex)
    #define ERaGuardUnlock(mutex)   ERaGuardUnlockFn(mutex)
#else
    #define ERaGuardAuto(mutex)
    #define ERaGuardLock(mutex)
    #define ERaGuardUnlock(mutex)
#endif

void ERaGuardLockFn(ERaMutex_t& mutex);
void ERaGuardUnlockFn(ERaMutex_t& mutex);

#if !defined(ERA_NO_RTOS)
    #include <Utility/ERaGuard.hpp>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void ERaWatchdogEnable(unsigned long timeout);
void ERaWatchdogDisable();
void ERaWatchdogFeed();

#ifdef __cplusplus
}
#endif

char* ERaStrdup(const char* str);
MillisTime_t ERaRemainingTime(MillisTime_t startMillis, MillisTime_t timeout);

uint8_t RSSIToPercentage(int16_t value);
int16_t GSMToRSSI(uint8_t value);
uint8_t GSMToPercentage(uint8_t value);
uint8_t SignalToPercentage(int16_t value);

bool ERaIsNaN(double value);
bool ERaIsInf(double value);
bool ERaIsSpN(double value);
long long ERaAtoll(const char* str);
char* ERaDtostrf(double number, int decimal, char* str);

template <typename T>
inline
const T& ERaMin(const T& a, const T& b) {
    return (b < a) ? b : a;
}

template <typename T>
inline
const T& ERaMax(const T& a, const T& b) {
    return (b < a) ? a : b;
}

template <class T, class T2>
inline
T ERaMathClamp(T val, T2 low, T2 high) {
    return (val < low) ? low : ((val > high) ? high : val);
}

template <typename T>
inline
T ERaMapNumberRange(T value, T fromLow, T fromHigh, T toLow, T toHigh) {
    return (toLow + (ERaMin(value, fromHigh) - ERaMin(value, fromLow)) * (toHigh - toLow) / (fromHigh - fromLow));
}

#if defined(ARDUINO) ||                 \
    defined(PARTICLE) || defined(SPARK)
    #if defined(ARDUINO)
        #include <Arduino.h>
    #else
        #include "application.h"
    #endif

    template <typename T, int size>
    inline
    void CopyToArray(const String& src, T(&dst)[size]) {
        src.toCharArray(dst, size);
    }
#endif

#if defined(ARDUINO) && defined(ESP32)
    #if (ESP_IDF_VERSION_MAJOR > 4)
        #include "esp_random.h"
        #include "driver/gpio.h"
    #endif
    #include "driver/uart.h"
    #include <Utility/ERaOs.hpp>
#elif defined(ARDUINO) &&               \
    !defined(__MBED__) &&               \
    (defined(RTL8722DM) ||              \
    defined(ARDUINO_AMEBA) ||           \
    defined(ARDUINO_ARCH_STM32) ||      \
    defined(ARDUINO_ARCH_RENESAS) ||    \
    defined(ARDUINO_ARCH_RP2040))
    #include <Utility/ERaOs.hpp>
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

template <int size>
inline
void CopyToString(const char* src, char(&dst)[size]) {
    snprintf(dst, size, "%s", src);
}

template <typename T>
inline
void ClearStruct(T& arr) {
    memset(&arr, 0, sizeof(T));
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

bool IsHexString(const char* buf);

bool ERaFloatCompare(float a, float b);
double ERaDoubleCompare(double a, double b);

char* ERaFindStr(const char* str, const char* str2);
bool ERaStrCmp(const char* str, const char* str2);
void ERaStrConcat(char* str, const char* str2);

template <int size>
inline
bool ERaStrNCmp(const char* str, const char(&str2)[size]) {
    return !strncmp(str, str2, size - 1);
}

template <int size>
inline
void ERaToLowerCase(char(&buf)[size]) {
    for (int i = 0; i < size; ++i) {
        buf[i] = ::tolower(buf[i]);
    }
}

template <int size>
inline
void ERaToUpperCase(char(&buf)[size]) {
    for (int i = 0; i < size; ++i) {
        buf[i] = ::toupper(buf[i]);
    }
}

#if defined(ESP32) && !_GLIBCXX_USE_C99_STDLIB
    #include <sstream>

    template <typename T>
    inline
    std::string to_string(const T& n)
    {
        std::ostringstream ss;
        ss << n;
        return ss.str();
    }
#endif

#include <ERa/ERaDebug.hpp>
#include <Utility/ERaStruct.hpp>

#endif /* INC_ERA_UTILITY_HPP_ */
