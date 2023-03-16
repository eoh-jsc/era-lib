#include <new>
#include <math.h>
#include <float.h>
#include <Utility/ERaUtility.hpp>

#if defined(ARDUINO) && defined(__AVR__)

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    size_t ERaFreeRam() {
        extern int __heap_start, *__brkval;
        int v;
        return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    }

    void ERaRestart(bool ERA_UNUSED async) {
        void(*resetMCU)(void) = 0;
        resetMCU();
        while (1) {}
    }

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_GUARD

#elif defined(ARDUINO) && defined(ESP32)

    void ERaDelay(MillisTime_t ms) {
        ERaOs::osDelay(ms);
    }

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        uint32_t numRand = esp_random();
        return (numRand % (max - min) + min);
    }

    size_t ERaFreeRam() {
        return ESP.getFreeHeap();
    }

    void ERaRestart(bool ERA_UNUSED async) {
        ESP.restart();
        while (1) {}
    }

    void ERaGuardLock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreAcquire((SemaphoreHandle_t)mutex, osWaitForever);
    }

    void ERaGuardUnlock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreRelease((SemaphoreHandle_t)mutex);
    }

    #define ERA_USE_DEFAULT_MILLIS

#elif defined(ARDUINO) && defined(ESP8266)

    void ERaDelay(MillisTime_t ms) {
        delay(ms);
    }

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    size_t ERaFreeRam() {
        return ESP.getFreeHeap();
    }

    void ERaRestart(bool ERA_UNUSED async) {
        ESP.restart();
        while (1) {}
    }

    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_GUARD

#elif defined(ARDUINO) && (defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD))

    extern "C" char *sbrk(int i);

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    size_t ERaFreeRam() {
        char stack_dummy = 0;
        return &stack_dummy - sbrk(0);
    }

    void ERaRestart(bool ERA_UNUSED async) {
        NVIC_SystemReset();
        while (1) {}
    }

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_GUARD

#elif defined(ARDUINO) && defined(ARDUINO_ARCH_ARC32)

    MillisTime_t ERaMillis() {
        noInterrupts();
        uint64_t t = millis();
        interrupts();
        return t;
    }

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_FREE_RAM
    #define ERA_USE_DEFAULT_RESET
    #define ERA_USE_DEFAULT_GUARD

#elif defined(ARDUINO) && (defined(__STM32F1__) || defined(__STM32F3__))

    #include <libmaple/nvic.h>

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    void ERaRestart(bool ERA_UNUSED async) {
        nvic_sys_reset();
        while (1) {}
    }

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_FREE_RAM
    #define ERA_USE_DEFAULT_GUARD

#elif defined(ARDUINO) && defined(ARDUINO_ARCH_STM32)

    extern "C" char *sbrk(int i);

    void ERaDelay(MillisTime_t ms) {
        ERaOs::osDelay(ms);
    }

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

#if defined(ERA_NO_RTOS)

    size_t ERaFreeRam() {
        char stack_dummy = 0;
        return &stack_dummy - sbrk(0);
    }

#else

    size_t ERaFreeRam() {
        if (ERaOs::osStarted()) {
            return ERaOs::osFreeHeapSize();
        }
        else {
            char stack_dummy = 0;
            return &stack_dummy - sbrk(0);
        }
    }

#endif

    void ERaRestart(bool ERA_UNUSED async) {
        NVIC_SystemReset();
        while (1) {}
    }

    void ERaGuardLock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreAcquire((SemaphoreHandle_t)mutex, osWaitForever);
    }

    void ERaGuardUnlock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreRelease((SemaphoreHandle_t)mutex);
    }

    #define ERA_USE_DEFAULT_MILLIS

#elif defined(ARDUINO) && !defined(__MBED__) && defined(ARDUINO_ARCH_RP2040)

    extern "C" char *sbrk(int i);

    void ERaDelay(MillisTime_t ms) {
        ERaOs::osDelay(ms);
    }

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

#if defined(ERA_NO_RTOS)

    size_t ERaFreeRam() {
        char stack_dummy = 0;
        return &stack_dummy - sbrk(0);
    }

#else

    size_t ERaFreeRam() {
        if (ERaOs::osStarted()) {
            return ERaOs::osFreeHeapSize();
        }
        else {
            char stack_dummy = 0;
            return &stack_dummy - sbrk(0);
        }
    }

#endif

    void ERaRestart(bool ERA_UNUSED async) {
        NVIC_SystemReset();
        while (1) {}
    }

    void ERaGuardLock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreAcquire((SemaphoreHandle_t)mutex, osWaitForever);
    }

    void ERaGuardUnlock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreRelease((SemaphoreHandle_t)mutex);
    }

    #define ERA_USE_DEFAULT_MILLIS

#elif defined(ARDUINO) && (defined(RTL8722DM) || defined(ARDUINO_AMEBA))

    extern "C" char *sbrk(int i);

    void ERaDelay(MillisTime_t ms) {
        ERaOs::osDelay(ms);
    }

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

#if defined(ERA_NO_RTOS)

    size_t ERaFreeRam() {
        char stack_dummy = 0;
        return &stack_dummy - sbrk(0);
    }

#else

    size_t ERaFreeRam() {
        if (ERaOs::osStarted()) {
            return ERaOs::osFreeHeapSize();
        }
        else {
            char stack_dummy = 0;
            return &stack_dummy - sbrk(0);
        }
    }

#endif

    void ERaRestart(bool ERA_UNUSED async) {
        NVIC_SystemReset();
        while (1) {}
    }

    void ERaGuardLock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreAcquire((SemaphoreHandle_t)mutex, osWaitForever);
    }

    void ERaGuardUnlock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreRelease((SemaphoreHandle_t)mutex);
    }

    #define ERA_USE_DEFAULT_MILLIS

#elif defined(ARDUINO) && defined(__MBED__)

    #include <mbed.h>
    using namespace mbed;
    using namespace rtos;

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    void ERaRestart(bool ERA_UNUSED async) {
        NVIC_SystemReset();
        while (1) {}
    }

#ifndef NO_RTOS
    void ERaGuardLock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) Semaphore(1);
        }
        ((Semaphore*)mutex)->acquire();
    }

    void ERaGuardUnlock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) Semaphore(1);
        }
        ((Semaphore*)mutex)->release();
    }
#else
    #define ERA_USE_DEFAULT_GUARD
#endif

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_FREE_RAM

#elif defined(PARTICLE) || defined(SPARK)

    #include "application.h"

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    void ERaRestart(bool ERA_UNUSED async) {
        System.reset();
        while (1) {}
    }

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_FREE_RAM
    #define ERA_USE_DEFAULT_GUARD

#elif defined(ARDUINO)

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_FREE_RAM
    #define ERA_USE_DEFAULT_RESET
    #define ERA_USE_DEFAULT_GUARD

#elif defined(__MBED__)

    #include <mbed.h>
    using namespace mbed;
    using namespace rtos;

    static Timer  eraMillisTimer;
    static Ticker eraWaker;

    static void eraWake() {
    }

    ERA_CONSTRUCTOR
    static void ERaSystemInit() {
        eraWaker.attach(&eraWake, 2.0f);
        eraMillisTimer.start();
    }

    void ERaDelay(MillisTime_t ms) {
#ifndef NO_RTOS
        ThisThread::sleep_for(ms);
#else
        wait_us(ms * 1000);
#endif
    }

    MillisTime_t ERaMillis() {
        return eraMillisTimer.read_ms();
    }

#ifndef NO_RTOS
    void ERaGuardLock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) Semaphore(1);
        }
        ((Semaphore*)mutex)->acquire();
    }

    void ERaGuardUnlock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) Semaphore(1);
        }
        ((Semaphore*)mutex)->release();
    }
#else
    #define ERA_USE_DEFAULT_GUARD
#endif

    #define ERA_USE_DEFAULT_RANDOM
    #define ERA_USE_DEFAULT_FREE_RAM
    #define ERA_USE_DEFAULT_RESET

#elif defined(LINUX) && defined(RASPBERRY)

    #include <stdlib.h>
    #include <pthread.h>
    #include <wiringPi.h>

    ERA_CONSTRUCTOR
    static void ERaSystemInit() {
        wiringPiSetupGpio();
    }

    void ERaRestart(bool ERA_UNUSED async) {
        exit(1);
        while (1) {}
    }

    void ERaGuardLock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) pthread_mutex_t;
            pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
        }
        pthread_mutex_lock((pthread_mutex_t*)mutex);
    }

    void ERaGuardUnlock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) pthread_mutex_t;
            pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
        }
        pthread_mutex_unlock((pthread_mutex_t*)mutex);
    }

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_RANDOM
    #define ERA_USE_DEFAULT_FREE_RAM

#elif defined(LINUX)

    #define _POSIX_C_SOURCE 200809L
    #include <stdlib.h>
    #include <time.h>
    #include <unistd.h>
    #include <pthread.h>

    static MillisTime_t startupTime {0};

    ERA_CONSTRUCTOR
    static void ERaSystemInit() {
        startupTime = ERaMillis();
    }

    void ERaDelay(MillisTime_t ms) {
        usleep(ms * 1000);
    }

    MillisTime_t ERaMillis() {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000L) - startupTime;
    }

    void ERaRestart(bool ERA_UNUSED async) {
        exit(1);
        while (1) {}
    }

    void ERaGuardLock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) pthread_mutex_t;
            pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
        }
        pthread_mutex_lock((pthread_mutex_t*)mutex);
    }

    void ERaGuardUnlock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) pthread_mutex_t;
            pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
        }
        pthread_mutex_unlock((pthread_mutex_t*)mutex);
    }

    #define ERA_USE_DEFAULT_RANDOM
    #define ERA_USE_DEFAULT_FREE_RAM

#else

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_RANDOM
    #define ERA_USE_DEFAULT_FREE_RAM
    #define ERA_USE_DEFAULT_RESET
    #define ERA_USE_DEFAULT_GUARD

#endif

#if defined(ERA_USE_DEFAULT_DELAY)
    void ERaDelay(MillisTime_t ms) {
        delay(ms);
    }
#endif

#if defined(ERA_USE_DEFAULT_MILLIS)
    MillisTime_t ERaMillis() {
        return millis();
    }
#endif

#if defined(ERA_USE_DEFAULT_RANDOM)
    #include <time.h>

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        srand(time(NULL));
        uint32_t numRand = rand();
        return (numRand % (max - min) + min);
    }
#endif

#if defined(ERA_USE_DEFAULT_FREE_RAM)
    size_t ERaFreeRam() {
        return 0;
    }
#endif

#if defined(ERA_USE_DEFAULT_RESET)
    void ERaRestart(bool ERA_UNUSED async) {
        while (1) {}
        ERA_FORCE_UNUSED(async);
    }
#endif

#if defined(ERA_USE_DEFAULT_GUARD)
    void ERaGuardLock(ERaMutex_t& mutex) {
        ERA_FORCE_UNUSED(mutex);
    }

    void ERaGuardUnlock(ERaMutex_t& mutex) {
        ERA_FORCE_UNUSED(mutex);
    }
#endif

char* ERaStrdup(const char* str) {
    if (str == nullptr) {
        return nullptr;
    }

    size_t length {0};
    char* copy = nullptr;

    length = strlen(str) + sizeof("");
    copy = (char*)ERA_MALLOC(length);
    if (copy == nullptr) {
        return nullptr;
    }
    memcpy(copy, str, length);

    return copy;
}

MillisTime_t ERaRemainingTime(MillisTime_t startMillis, MillisTime_t timeout) {
    int32_t remainingTime = startMillis + timeout - ERaMillis();
    return (remainingTime > 0 ? remainingTime : 0);
}

uint8_t RSSIToPercentage(int16_t value) {
    uint8_t percentage {0};
    if(value <= -100) {
        percentage = 0;
    }
    else if(value >= -50) {
        percentage = 100;
    }
    else {
        percentage = 2 * (value + 100);
    }
    return percentage;
}

int16_t GSMToRSSI(uint8_t value) {
    int16_t rssi {-255};
    if (value != 99) {
        rssi = (2 * value) - 113;
    }
    return rssi;
}

uint8_t GSMToPercentage(uint8_t value) {
    int16_t rssi = GSMToRSSI(value);
    return RSSIToPercentage(rssi);
}

long long ERaAtoll(const char* str) {
    long long value {0};
    for (; *str; str++) {
        value = ((10 * value) + (*str - '0'));
    }
    return value;
}

char* ERaDtostrf(double number, int decimal, char* str) {
    if (str == nullptr) {
        return nullptr;
    }
    if (isnan(number)) {
        strcpy(str, "nan");
        return str;
    }
    if (isinf(number)) {
        strcpy(str, "inf");
        return str;
    }

    if ((number > 4294967040.0) || (number < -4294967040.0)) {
        strcpy(str, "ovf");
        return str;
    }
    char* out = str;
    if (number < 0.0) {
        *out = '-';
        ++out;
        number = -number;
    }

    double rounding = 0.5;
    for (uint8_t i = 0; i < decimal; ++i) {
        rounding /= 10.0;
    }

    number += rounding;

    unsigned long int_part = (unsigned long) number;
    double remainder = number - (double) int_part;
    out += sprintf(out, "%lu", int_part);

    if (decimal > 0) {
        *out++ = '.';
    }

    while (decimal-- > 0) {
        remainder *= 10.0;
        if ((int)remainder == 0) {
            *out++ = '0';
        }
    }
    if ((int)remainder != 0) {
        sprintf(out, "%d", (int)remainder);
    }
    else {
        *out++ = '\0';
    }

    return str;
}

bool ERaFloatCompare(float a, float b) {
    float maxVal = (fabs(a) > fabs(b)) ? fabs(a) : fabs(b);
    return (fabs(a - b) <= (maxVal * FLT_EPSILON));
}

double ERaDoubleCompare(double a, double b) {
    double maxVal = (fabs(a) > fabs(b)) ? fabs(a) : fabs(b);
    return (fabs(a - b) <= (maxVal * DBL_EPSILON));
}

char* ERaFindStr(const char* str, const char* str2) {
    return (char*)strstr(str, str2);
}

bool ERaStrCmp(const char* str, const char* str2) {
    return !strcmp(str, str2);
}
