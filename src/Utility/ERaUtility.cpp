#include <Utility/ERaUtility.hpp>

#if defined(ARDUINO) && defined(__AVR__)

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    size_t ERaFreeRam() {
        extern int __heap_start, *__brkval;
        int v;
        return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    }

    void ERaRestart(bool async) {
        void(*resetMCU)(void) = 0;
        resetMCU();
        while (1) {}
    }

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_GUARD

#elif defined(ARDUINO) && defined(ESP32)

    void ERaDelay(MillisTime_t ms) {
        osDelay(ms);
    }

    MillisTime_t ERaMillis() {
        return millis();
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

    void ERaRestart(bool async) {
        ESP.restart();
        while (1) {}
    }

    void ERaGuardLock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)xSemaphoreCreateMutex();
        }
        osSemaphoreAcquire((SemaphoreHandle_t)mutex, osWaitForever);
    }

    void ERaGuardUnlock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)xSemaphoreCreateMutex();
        }
        osSemaphoreRelease((SemaphoreHandle_t)mutex);
    }

#elif defined(ARDUINO) && defined(ESP8266)

    void ERaDelay(MillisTime_t ms) {
        delay(ms);
    }

    MillisTime_t ERaMillis() {
        return millis();
    }

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    size_t ERaFreeRam() {
        return ESP.getFreeHeap();
    }

    void ERaRestart(bool async) {
        ESP.restart();
        while (1) {}
    }

    #define ERA_USE_DEFAULT_GUARD

#elif defined(ARDUINO) && (defined(__STM32F1__) || defined(__STM32F3__))

    #include <libmaple/nvic.h>

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    void ERaRestart(bool async) {
        nvic_sys_reset();
        while (1) {}
    }

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_FREE_RAM
    #define ERA_USE_DEFAULT_GUARD

#elif defined(ARDUINO) && (defined(STM32F4xx) || defined(STM32F7xx))

    extern "C" char *sbrk(int i);

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    size_t ERaFreeRam() {
        char stack_dummy = 0;
        return &stack_dummy - sbrk(0);
    }

    void ERaRestart(bool async) {
        NVIC_SystemReset();
        while (1) {}
    }

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_GUARD

#elif defined(LINUX) && defined(RASPBERRY)

    #include <stdlib.h>
    #include <wiringPi.h>

    ERA_CONSTRUCTOR
    static void ERaSystemInit() {
        wiringPiSetupGpio();
    }

    void ERaRestart(bool async) {
        exit(1);
        while (1) {}
    }

    void ERaGuardLock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new pthread_mutex_t;
            pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
        }
        pthread_mutex_lock((pthread_mutex_t*)mutex);
    }

    void ERaGuardUnlock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new pthread_mutex_t;
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

    static millis_time_t startupTime {0};

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

    void ERaRestart(bool async) {
        exit(1);
        while (1) {}
    }

    void ERaGuardLock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new pthread_mutex_t;
            pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
        }
        pthread_mutex_lock((pthread_mutex_t*)mutex);
    }

    void ERaGuardUnlock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new pthread_mutex_t;
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
    void ERaRestart(bool async) {
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

bool ERaStrCmp(const char* str, const char* str2) {
    return !strcmp(str, str2);
}
