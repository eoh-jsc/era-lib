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

#elif defined(ARDUINO) &&                        \
    (defined(STM32F0xx) || defined(STM32F1xx) || \
    defined(STM32F2xx) || defined(STM32F3xx) ||  \
    defined(STM32F4xx) || defined(STM32F7xx))

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

    size_t ERaFreeRam() {
        char stack_dummy = 0;
        return &stack_dummy - sbrk(0);
    }

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

#elif defined(ARDUINO) && defined(ARDUINO_ARCH_RP2040)

    void ERaDelay(MillisTime_t ms) {
        ERaOs::osDelay(ms);
    }

    MillisTime_t ERaMillis() {
        return millis();
    }

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
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

    #define ERA_USE_DEFAULT_FREE_RAM
    #define ERA_USE_DEFAULT_RESET

#elif defined(__MBED__)

    #include <mbed.h>
    using namespace mbed;
    using namespace rtos;

    static Timer  eraMillisTimer;
    static Ticker eraWaker;

    static
    void eraWake() {
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
            mutex = new Semaphore(1);
        }
        ((Semaphore*)mutex)->acquire();
    }

    void ERaGuardUnlock(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new Semaphore(1);
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

bool ERaStrCmp(const char* str, const char* str2) {
    return !strcmp(str, str2);
}
