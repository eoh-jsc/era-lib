#include <new>
#include <math.h>
#include <float.h>
#include <Utility/ERaUtility.hpp>

#if defined(ARDUINO) && defined(__AVR__)

    #include <avr/wdt.h>

    static bool isWatchdogEnable {false};

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

    ERA_WEAK
    void ERaWatchdogEnable(unsigned long timeout) {
        if (isWatchdogEnable) {
            return;
        }
        wdt_enable(WDTO_8S);
        isWatchdogEnable = true;
        ERA_FORCE_UNUSED(timeout);
    }

    ERA_WEAK
    void ERaWatchdogDisable() {
        if (!isWatchdogEnable) {
            return;
        }
        wdt_disable();
        isWatchdogEnable = false;
    }

    ERA_WEAK
    void ERaWatchdogFeed() {
        if (!isWatchdogEnable) {
            return;
        }
        wdt_reset();
    }

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_GUARD

#elif defined(ARDUINO) && defined(ESP32)

    #include <esp_task_wdt.h>

    static bool isWatchdogEnable {false};

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

    void ERaGuardLockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreAcquire((SemaphoreHandle_t)mutex, osWaitForever);
    }

    void ERaGuardUnlockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreRelease((SemaphoreHandle_t)mutex);
    }

#if (ESP_IDF_VERSION_MAJOR > 4)
    ERA_WEAK
    void ERaWatchdogEnable(unsigned long timeout) {
        if (isWatchdogEnable) {
            return;
        }
        esp_task_wdt_config_t twdt_config = {
            .timeout_ms = timeout,
            .idle_core_mask = ((1 << portNUM_PROCESSORS) - 1),
            .trigger_panic = true
        };
#if !CONFIG_ESP_TASK_WDT_INIT
        esp_task_wdt_init(&twdt_config);
#else
        esp_task_wdt_reconfigure(&twdt_config);
#endif
        esp_task_wdt_add(NULL);
        esp_task_wdt_status(NULL);
        isWatchdogEnable = true;
    }
#else
    ERA_WEAK
    void ERaWatchdogEnable(unsigned long timeout) {
        if (isWatchdogEnable) {
            return;
        }
        esp_task_wdt_init(timeout / 1000UL, true);
        esp_task_wdt_add(NULL);
        esp_task_wdt_status(NULL);
        isWatchdogEnable = true;
    }
#endif

    ERA_WEAK
    void ERaWatchdogDisable() {
        if (!isWatchdogEnable) {
            return;
        }
        esp_task_wdt_delete(NULL);
        isWatchdogEnable = false;
    }

    ERA_WEAK
    void ERaWatchdogFeed() {
        if (!isWatchdogEnable) {
            return;
        }
        esp_task_wdt_reset();
    }

    #define ERA_USE_DEFAULT_MILLIS

#elif defined(ARDUINO) && defined(ESP8266)

#if defined(ESP8266) && defined(ARDUINO_ESP8266_MAJOR) &&               \
    defined(ARDUINO_ESP8266_MINOR) && defined(ARDUINO_ESP8266_REVISION)
    #define ESP8266_VERSION_NUMBER ((ARDUINO_ESP8266_MAJOR * 10000) +   \
                                    (ARDUINO_ESP8266_MINOR * 100) +     \
                                    (ARDUINO_ESP8266_REVISION))
    #if (ESP8266_VERSION_NUMBER >= 30101)
        #include <coredecls.h>
        #define ESP8266_YIELD_FIX(ms) { if (!ms) { esp_yield(); return; } }
    #endif
#endif

#if !defined(ESP8266_YIELD_FIX)
    #define ESP8266_YIELD_FIX(ms)
#endif

    #include <Esp.h>

    void ERaDelay(MillisTime_t ms) {
        ESP8266_YIELD_FIX(ms)
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

    ERA_WEAK
    void ERaWatchdogEnable(unsigned long timeout) {
        ESP.wdtDisable();
        *((volatile uint32_t*) 0x60000900) &= ~(1);
        *((volatile uint32_t*) 0x60000904) |= 15;
        *((volatile uint32_t*) 0x60000900) |= 1;
        ERA_FORCE_UNUSED(timeout);
    }

    ERA_WEAK
    void ERaWatchdogDisable() {
        ESP.wdtEnable(0);
    }

    ERA_WEAK
    void ERaWatchdogFeed() {
        ESP.wdtFeed();
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

#if defined(ARDUINO_ARCH_SAMD)

    #include <Adafruit_SleepyDog.h>

    static bool isWatchdogEnable {false};

    ERA_WEAK
    void ERaWatchdogEnable(unsigned long timeout) {
        if (isWatchdogEnable) {
            return;
        }
        Watchdog.enable(timeout);
        isWatchdogEnable = true;
    }

    ERA_WEAK
    void ERaWatchdogDisable() {
    }

    ERA_WEAK
    void ERaWatchdogFeed() {
        if (!isWatchdogEnable) {
            return;
        }
        Watchdog.reset();
    }

    void wifi_nina_feed_watchdog() {
        ERaWatchdogFeed();
    }

    void mkr_nb_feed_watchdog() {
        ERaWatchdogFeed();
    }

    void mkr_gsm_feed_watchdog() {
        ERaWatchdogFeed();
    }
#else
    #define ERA_USE_DEFAULT_WATCHDOG
#endif

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
    #define ERA_USE_DEFAULT_WATCHDOG

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
    #define ERA_USE_DEFAULT_WATCHDOG

#elif defined(ARDUINO) && defined(ARDUINO_ARCH_STM32)

    #include <IWatchdog.h>

    extern "C" char *sbrk(int i);

#if defined(ERA_NO_RTOS) ||     \
    !defined(ARDUINO_RTOS_STM32)
    void ERaDelay(MillisTime_t ms) {
        delay(ms);
    }
#else
    void ERaDelay(MillisTime_t ms) {
        ERaOs::osDelay(ms);
    }
#endif

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

#if defined(ERA_NO_RTOS) ||     \
    !defined(ARDUINO_RTOS_STM32)
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

#if defined(ERA_NO_RTOS) ||     \
    !defined(ARDUINO_RTOS_STM32)
    #define ERA_USE_DEFAULT_GUARD
#else
    void ERaGuardLockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreAcquire((SemaphoreHandle_t)mutex, osWaitForever);
    }

    void ERaGuardUnlockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreRelease((SemaphoreHandle_t)mutex);
    }
#endif

    ERA_WEAK
    void ERaWatchdogEnable(unsigned long timeout) {
        if (IWatchdog.isEnabled()) {
            return;
        }
        if (timeout > IWDG_TIMEOUT_MAX) {
            timeout = IWDG_TIMEOUT_MAX;
        }
        IWatchdog.isReset(true);
        IWatchdog.begin(timeout * 1000UL);
    }

    ERA_WEAK
    void ERaWatchdogDisable() {
    }

    ERA_WEAK
    void ERaWatchdogFeed() {
        if (!IWatchdog.isEnabled()) {
            return;
        }
        IWatchdog.reload();
    }

    #define ERA_USE_DEFAULT_MILLIS

#elif defined(ARDUINO) && !defined(__MBED__) && defined(ARDUINO_ARCH_RP2040)

    #include <RP2040.h>

    static bool isWatchdogEnable {false};

    void ERaDelay(MillisTime_t ms) {
        ERaOs::osDelay(ms);
    }

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        uint32_t numRand = rp2040.hwrand32();
        return (numRand % (max - min) + min);
    }

    size_t ERaFreeRam() {
        return rp2040.getFreeHeap();
    }

    void ERaRestart(bool ERA_UNUSED async) {
        rp2040.reboot();
        while (1) {}
    }

    void ERaGuardLockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreAcquire((SemaphoreHandle_t)mutex, osWaitForever);
    }

    void ERaGuardUnlockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreRelease((SemaphoreHandle_t)mutex);
    }

    ERA_WEAK
    void ERaWatchdogEnable(unsigned long timeout) {
        if (isWatchdogEnable) {
            return;
        }
        rp2040.wdt_begin(timeout);
        isWatchdogEnable = true;
    }

    ERA_WEAK
    void ERaWatchdogDisable() {
    }

    ERA_WEAK
    void ERaWatchdogFeed() {
        if (!isWatchdogEnable) {
            return;
        }
        rp2040.wdt_reset();
    }

    #define ERA_USE_DEFAULT_MILLIS

#elif defined(ARDUINO) && (defined(RTL8722DM) || defined(ARDUINO_AMEBA))

    #include <WDT.h>

    static WDT Watchdog;
    static bool isWatchdogEnable {false};

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

    void ERaGuardLockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreAcquire((SemaphoreHandle_t)mutex, osWaitForever);
    }

    void ERaGuardUnlockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreRelease((SemaphoreHandle_t)mutex);
    }

    ERA_WEAK
    void ERaWatchdogEnable(unsigned long timeout) {
        if (isWatchdogEnable) {
            return;
        }
        Watchdog.InitWatchdog(timeout);
        Watchdog.StartWatchdog();
        isWatchdogEnable = true;
    }

    ERA_WEAK
    void ERaWatchdogDisable() {
        if (!isWatchdogEnable) {
            return;
        }
        Watchdog.StopWatchdog();
        isWatchdogEnable = false;
    }

    ERA_WEAK
    void ERaWatchdogFeed() {
        if (!isWatchdogEnable) {
            return;
        }
        Watchdog.RefreshWatchdog();
    }

    #define ERA_USE_DEFAULT_MILLIS

#elif defined(ARDUINO) && defined(ARDUINO_ARCH_RENESAS)

    #include <WDT.h>

    static bool isWatchdogEnable {false};

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

    void ERaGuardLockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreAcquire((SemaphoreHandle_t)mutex, osWaitForever);
    }

    void ERaGuardUnlockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = (ERaMutex_t)ERaOs::osSemaphoreNew();
        }
        ERaOs::osSemaphoreRelease((SemaphoreHandle_t)mutex);
    }

    ERA_WEAK
    void ERaWatchdogEnable(unsigned long timeout) {
        if (isWatchdogEnable) {
            return;
        }
        if (WDT.begin(timeout)) {
            isWatchdogEnable = true;
        }
    }

    ERA_WEAK
    void ERaWatchdogDisable() {
        if (!isWatchdogEnable) {
            return;
        }
        isWatchdogEnable = false;
    }

    ERA_WEAK
    void ERaWatchdogFeed() {
        if (!isWatchdogEnable) {
            return;
        }
        WDT.refresh();
    }

    #define ERA_USE_DEFAULT_MILLIS

#elif defined(ARDUINO) && defined(ARDUINO_ARCH_ARM)

#if defined(ERA_NO_RTOS)
    void ERaDelay(MillisTime_t ms) {
        delay(ms);
    }
#else
    void ERaDelay(MillisTime_t ms) {
        os_task_sleep(ms);
    }
#endif

    uint32_t ERaRandomNumber(uint32_t min, uint32_t max) {
        if (!max) {
            return 0;
        }
        randomSeed(ERaMillis());
        uint32_t numRand = random(max);
        return (numRand % (max - min) + min);
    }

    void ERaRestart(bool ERA_UNUSED async) {
        sys_reset();
        while (1) {}
    }

#if defined(ERA_NO_RTOS)
    #define ERA_USE_DEFAULT_GUARD
#else
    void ERaGuardLockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new uint32_t;
        }
        os_mutex_take(*((uint32_t*)(mutex)));
    }

    void ERaGuardUnlockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new uint32_t;
        }
        os_mutex_give(*((uint32_t*)(mutex)));
    }
#endif

    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_FREE_RAM
    #define ERA_USE_DEFAULT_WATCHDOG

#elif defined(ARDUINO) && defined(__MBED__)

    #include <mbed.h>
    #include <watchdog_api.h>
    using namespace mbed;
    using namespace rtos;

    static bool isWatchdogEnable {false};

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
    void ERaGuardLockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) Semaphore(1);
        }
        ((Semaphore*)mutex)->acquire();
    }

    void ERaGuardUnlockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) Semaphore(1);
        }
        ((Semaphore*)mutex)->release();
    }
#else
    #define ERA_USE_DEFAULT_GUARD
#endif

#if defined(ARDUINO_PORTENTA_H7_M4) ||      \
    defined(ARDUINO_PORTENTA_H7_M7) ||      \
    defined(ARDUINO_NICLA_VISION) ||        \
    defined(ARDUINO_OPTA) ||                \
    defined(ARDUINO_GIGA)
    #include <WiFi.h>
    #if defined(ARDUINO_PORTENTA_H7_M4) ||  \
        defined(ARDUINO_PORTENTA_H7_M7) ||  \
        defined(ARDUINO_OPTA)
        #include <Ethernet.h>
    #endif
#endif

    ERA_WEAK
    void ERaWatchdogEnable(unsigned long timeout) {
        if (isWatchdogEnable) {
            return;
        }
        watchdog_config_t cfg;
        cfg.timeout_ms = timeout;
        if (hal_watchdog_init(&cfg) == WATCHDOG_STATUS_OK) {
            isWatchdogEnable = true;
        }
#if defined(ARDUINO_PORTENTA_H7_M4) ||  \
    defined(ARDUINO_PORTENTA_H7_M7) ||  \
    defined(ARDUINO_NICLA_VISION) ||    \
    defined(ARDUINO_OPTA) ||            \
    defined(ARDUINO_GIGA)
        WiFi.setFeedWatchdogFunc(ERaWatchdogFeed);
    #if defined(ARDUINO_PORTENTA_H7_M4) ||  \
        defined(ARDUINO_PORTENTA_H7_M7) ||  \
        defined(ARDUINO_OPTA)
        Ethernet.setFeedWatchdogFunc(ERaWatchdogFeed);
    #endif
#endif
    }

    ERA_WEAK
    void ERaWatchdogDisable() {
    }

    ERA_WEAK
    void ERaWatchdogFeed() {
        if (!isWatchdogEnable) {
            return;
        }
        hal_watchdog_kick();
    }

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
    #define ERA_USE_DEFAULT_WATCHDOG

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
    #define ERA_USE_DEFAULT_WATCHDOG

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
    void ERaGuardLockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) Semaphore(1);
        }
        ((Semaphore*)mutex)->acquire();
    }

    void ERaGuardUnlockFn(ERaMutex_t& mutex) {
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
    #define ERA_USE_DEFAULT_WATCHDOG

#elif defined(LINUX) && (defined(RASPBERRY) || defined(TINKER_BOARD) || defined(ORANGE_PI))

    #include <stdlib.h>
    #include <pthread.h>
    #include <wiringPi.h>

    ERA_CONSTRUCTOR
    static void ERaSystemInit() {
#if defined(RASPBERRY)
        wiringPiSetupGpio();
#elif defined(TINKER_BOARD) ||  \
        defined(ORANGE_PI)
        wiringPiSetupPhys();
#endif
    }

    void ERaRestart(bool ERA_UNUSED async) {
        exit(1);
        while (1) {}
    }

    void ERaGuardLockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) pthread_mutex_t;
            pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
        }
        pthread_mutex_lock((pthread_mutex_t*)mutex);
    }

    void ERaGuardUnlockFn(ERaMutex_t& mutex) {
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
    #define ERA_USE_DEFAULT_WATCHDOG

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

    void ERaGuardLockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) pthread_mutex_t;
            pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
        }
        pthread_mutex_lock((pthread_mutex_t*)mutex);
    }

    void ERaGuardUnlockFn(ERaMutex_t& mutex) {
        if (mutex == nullptr) {
            mutex = new(std::nothrow) pthread_mutex_t;
            pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
        }
        pthread_mutex_unlock((pthread_mutex_t*)mutex);
    }

    #define ERA_USE_DEFAULT_RANDOM
    #define ERA_USE_DEFAULT_FREE_RAM
    #define ERA_USE_DEFAULT_WATCHDOG

#else

    #define ERA_USE_DEFAULT_DELAY
    #define ERA_USE_DEFAULT_MILLIS
    #define ERA_USE_DEFAULT_RANDOM
    #define ERA_USE_DEFAULT_FREE_RAM
    #define ERA_USE_DEFAULT_RESET
    #define ERA_USE_DEFAULT_GUARD
    #define ERA_USE_DEFAULT_WATCHDOG

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

MillisTime_t ERaSeconds() {
    return (ERaMillis() / 1000UL);
}

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

void ERaFatality() {
    ERaDelay(1000);
    ERaRestart(false);
}

#if defined(ERA_USE_DEFAULT_GUARD)
    void ERaGuardLockFn(ERaMutex_t& mutex) {
        ERA_FORCE_UNUSED(mutex);
    }

    void ERaGuardUnlockFn(ERaMutex_t& mutex) {
        ERA_FORCE_UNUSED(mutex);
    }
#endif

#if defined(ERA_USE_DEFAULT_WATCHDOG)
    ERA_WEAK
    void ERaWatchdogEnable(unsigned long timeout) {
        ERA_FORCE_UNUSED(timeout);
    }

    ERA_WEAK
    void ERaWatchdogDisable() {
    }

    ERA_WEAK
    void ERaWatchdogFeed() {
    }
#endif

#ifndef isinf
    #define isinf(d) (isnan((d - d)) && !isnan(d))
#endif
#ifndef isnan
    #define isnan(d) (d != d)
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
    int32_t remainingTime = (startMillis + timeout - ERaMillis());
    return ((remainingTime > 0) ? remainingTime : 0);
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

uint8_t SignalToPercentage(int16_t value) {
    if (value >= 100) {
        return 100;
    }

    uint8_t percentage {0};
    if (value >= 0) {
        percentage = GSMToPercentage(value);
    }
    else {
        percentage = RSSIToPercentage(value);
    }
    return percentage;
}

bool ERaIsNaN(double value) {
    return isnan(value);
}

bool ERaIsInf(double value) {
    return isinf(value);
}

bool ERaIsSpN(double value) {
    return (isnan(value) || isinf(value));
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

bool IsHexString(const char* buf) {
    if (buf == nullptr) {
        return false;
    }

    size_t len = strlen(buf);
    if (!len) {
        return false;
    }

    for (size_t i = 0; i < len; ++i) {
        if (!isxdigit(buf[i])) {
            return false;
        }
    }
    return true;
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

void ERaStrConcat(char* str, const char* str2) {
    strcat(str, str2);
}
