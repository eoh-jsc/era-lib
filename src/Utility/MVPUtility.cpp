#include <Utility/MVPUtility.hpp>

#if defined(ARDUINO) && defined(ESP32)
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
#elif defined(LINUX)
    void MVPDelay(MillisTime_t ms) {
        usleep(ms * 1000);
    }

    MillisTime_t MVPMillis() {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000L);
    }

    uint32_t MVPRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        srand(time(NULL));
        uint32_t numRand = rand();
        return (numRand % (max - min) + min);
    }

    size_t MVPFreeRam() {
        return 0;
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
