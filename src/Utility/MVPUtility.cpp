#include <Utility/MVPUtility.hpp>

#if defined(ARDUINO) && defined(__AVR__)

    uint32_t MVPRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    size_t MVPFreeRam() {
        extern int __heap_start, *__brkval;
        int v;
        return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    }

    void MVPRestart(bool async) {
        void(*resetMCU)(void) = 0;
        resetMCU();
        while (1) {}
    }

    #define MVP_USE_DEFAULT_DELAY
    #define MVP_USE_DEFAULT_MILLIS

#elif defined(ARDUINO) && defined(ESP32)

    void MVPDelay(MillisTime_t ms) {
        osDelay(ms);
    }

    MillisTime_t MVPMillis() {
        return millis();
    }

    uint32_t MVPRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        uint32_t numRand = esp_random();
        return (numRand % (max - min) + min);
    }

    size_t MVPFreeRam() {
        return ESP.getFreeHeap();
    }

    void MVPRestart(bool async) {
        ESP.restart();
        while (1) {}
    }

    void MVPGuardLock(MVPMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (MVPMutex_t)xSemaphoreCreateMutex();
        }
        osSemaphoreAcquire((SemaphoreHandle_t)mutex, osWaitForever);
    }

    void MVPGuardUnlock(MVPMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (MVPMutex_t)xSemaphoreCreateMutex();
        }
        osSemaphoreRelease((SemaphoreHandle_t)mutex);
    }

#elif defined(ARDUINO) && defined(ESP8266)

    void MVPDelay(MillisTime_t ms) {
        delay(ms);
    }

    MillisTime_t MVPMillis() {
        return millis();
    }

    uint32_t MVPRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    size_t MVPFreeRam() {
        return ESP.getFreeHeap();
    }

    void MVPRestart(bool async) {
        ESP.restart();
        while (1) {}
    }

#elif defined(ARDUINO) && (defined(__STM32F1__) || defined(__STM32F3__))

    #include <libmaple/nvic.h>

    uint32_t MVPRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    void MVPRestart(bool async) {
        nvic_sys_reset();
        while (1) {}
    }

    #define MVP_USE_DEFAULT_DELAY
    #define MVP_USE_DEFAULT_MILLIS
    #define MVP_USE_DEFAULT_FREE_RAM

#elif defined(ARDUINO) && (defined(STM32F4xx) || defined(STM32F7xx))

    extern "C" char *sbrk(int i);

    uint32_t MVPRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    size_t MVPFreeRam() {
        char stack_dummy = 0;
        return &stack_dummy - sbrk(0);
    }

    void MVPRestart(bool async) {
        NVIC_SystemReset();
        while (1) {}
    }

    #define MVP_USE_DEFAULT_DELAY
    #define MVP_USE_DEFAULT_MILLIS

#elif defined(LINUX) && defined(RASPBERRY)

    #include <stdlib.h>
    #include <wiringPi.h>

    MVP_CONSTRUCTOR
    static void MVPSystemInit() {
        wiringPiSetupGpio();
    }

    void MVPRestart(bool async) {
        exit(1);
        while (1) {}
    }

    void MVPGuardLock(MVPMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new pthread_mutex_t(0);
            pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
        }
        pthread_mutex_lock((pthread_mutex_t*)mutex);
    }

    void MVPGuardUnlock(MVPMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new pthread_mutex_t(0);
            pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
        }
        pthread_mutex_unlock((pthread_mutex_t*)mutex);
    }

    #define MVP_USE_DEFAULT_DELAY
    #define MVP_USE_DEFAULT_MILLIS
    #define MVP_USE_DEFAULT_RANDOM
    #define MVP_USE_DEFAULT_FREE_RAM

#elif defined(LINUX)

    #define _POSIX_C_SOURCE 200809L
    #include <stdlib.h>
    #include <time.h>
    #include <unistd.h>

    static millis_time_t startupTime {0};

    MVP_CONSTRUCTOR
    static void MVPSystemInit() {
        startupTime = MVPMillis();
    }

    void MVPDelay(MillisTime_t ms) {
        usleep(ms * 1000);
    }

    MillisTime_t MVPMillis() {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000L) - startupTime;
    }

    void MVPRestart(bool async) {
        exit(1);
        while (1) {}
    }

    void MVPGuardLock(MVPMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new pthread_mutex_t(0);
            pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
        }
        pthread_mutex_lock((pthread_mutex_t*)mutex);
    }

    void MVPGuardUnlock(MVPMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new pthread_mutex_t(0);
            pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
        }
        pthread_mutex_unlock((pthread_mutex_t*)mutex);
    }

    #define MVP_USE_DEFAULT_RANDOM
    #define MVP_USE_DEFAULT_FREE_RAM

#else

    #define MVP_USE_DEFAULT_DELAY
    #define MVP_USE_DEFAULT_MILLIS
    #define MVP_USE_DEFAULT_RANDOM
    #define MVP_USE_DEFAULT_FREE_RAM
    #define MVP_USE_DEFAULT_RESET

#endif

#if defined(MVP_USE_DEFAULT_DELAY)
    void MVPDelay(MillisTime_t ms) {
        delay(ms);
    }
#endif

#if defined(MVP_USE_DEFAULT_MILLIS)
    MillisTime_t MVPMillis() {
        return millis();
    }
#endif

#if defined(MVP_USE_DEFAULT_RANDOM)
    #include <time.h>

    uint32_t MVPRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        srand(time(NULL));
        uint32_t numRand = rand();
        return (numRand % (max - min) + min);
    }
#endif

#if defined(MVP_USE_DEFAULT_FREE_RAM)
    size_t MVPFreeRam() {
        return 0;
    }
#endif

#if defined(MVP_USE_DEFAULT_RESET)
    void MVPRestart(bool async) {
        while (1) {}
    }
#endif

char* MVPStrdup(const char* str) {
    if (str == nullptr) {
        return nullptr;
    }

    size_t length {0};
    char* copy = nullptr;

    length = strlen(str) + sizeof("");
    copy = (char*)MVP_MALLOC(length);
    if (copy == nullptr) {
        return nullptr;
    }
    memcpy(copy, str, length);

    return copy;
}

MillisTime_t MVPRemainingTime(MillisTime_t prevMillis, MillisTime_t timeout) {
    int32_t remainingTime = prevMillis + timeout - MVPMillis();
    return (remainingTime > 0 ? remainingTime : 0);
}

bool MVPStrCmp(const char* str, const char* str2) {
    return !strcmp(str, str2);
}
