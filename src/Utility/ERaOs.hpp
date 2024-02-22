#ifndef INC_ERA_OS_HPP_
#define INC_ERA_OS_HPP_

#if defined(ARDUINO_ARCH_RENESAS) &&    \
    (defined(ARDUINO_MINIMA) ||         \
    defined(ARDUINO_UNOWIFIR4) ||       \
    defined(ARDUINO_PORTENTA_C33))
    #define ARDUINO_RTOS_RENESAS
#elif defined(__has_include) &&         \
    __has_include(<FreeRTOS.h>) &&      \
    defined(ARDUINO_ARCH_STM32)
    #define ARDUINO_RTOS_STM32
#endif

#if defined(ARDUINO) &&                 \
    !defined(__MBED__) &&               \
    (defined(ESP32) ||                  \
    defined(RTL8722DM) ||               \
    defined(ARDUINO_AMEBA) ||           \
    defined(ARDUINO_RTOS_STM32) ||      \
    defined(ARDUINO_RTOS_RENESAS) ||    \
    defined(ARDUINO_ARCH_RP2040))

#define ERA_RTOS

#include <ERa/ERaDetect.hpp>

#if defined(ESP32)
    #include "freertos/FreeRTOS.h"
    #include "freertos/semphr.h"
    #include "freertos/task.h"
    #include "freertos/queue.h"
    #include "freertos/event_groups.h"
#elif defined(ARDUINO_RTOS_RENESAS)
    #include <Arduino_FreeRTOS.h>

    #ifndef IRAM_ATTR
        #define IRAM_ATTR
    #endif
#else
    #include "STM32FreeRTOSConfig_extra.h"
    #include <FreeRTOS.h>
    #include <semphr.h>
    #include <task.h>
    #include <queue.h>
    #include <event_groups.h>

    #ifndef IRAM_ATTR
        #define IRAM_ATTR
    #endif
#endif

#ifndef configSUPPORT_STATIC_ALLOCATION
    #define configSUPPORT_STATIC_ALLOCATION         0
#endif

#ifndef portEND_SWITCHING_ISR
    #define portEND_SWITCHING_ISR(xSwitchRequired)  if (xSwitchRequired != pdFALSE) portYIELD()
#endif
#ifndef osPortYIELD_FROM_ISR
    #define osPortYIELD_FROM_ISR(x)                 portEND_SWITCHING_ISR(x)
#endif

/// Timeout value.
#define osWaitForever         0xFFFFFFFFU ///< Wait forever timeout value.
#define osFlagsWaitAny        0x00000000U ///< Wait for any flag (default).
#define osFlagsWaitAll        0x00000001U ///< Wait for all flags.
#define osFlagsNoClear        0x00000002U ///< Do not clear flags which have been specified to wait for.

typedef enum {
    osOK                      =  0,         ///< Operation completed successfully.
    osError                   = -1,         ///< Unspecified RTOS error: run-time error but no other error message fits.
    osErrorTimeout            = -2,         ///< Operation not completed within the timeout period.
    osErrorResource           = -3,         ///< Resource not available.
    osErrorParameter          = -4,         ///< Parameter error.
    osErrorNoMemory           = -5,         ///< System is out of memory: it was impossible to allocate or reserve memory for the operation.
    osErrorISR                = -6,         ///< Not allowed in ISR context: the function cannot be called from interrupt service routines.
    osStatusReserved          = 0x7FFFFFFF  ///< Prevents enum down-size compiler optimization.
} osStatus_t;

/// Timer type.
typedef enum {
    osTimerOnce               = 0,          ///< One-shot timer.
    osTimerPeriodic           = 1           ///< Repeating timer.
} osTimerType_t;

namespace ERaOs {

    void osStartsScheduler();
    void osDelay(uint32_t ticks);
    bool osStarted();
    uint32_t osFreeHeapSize();
    TaskHandle_t osThreadNew(void (*task)(void *args), const char* name, uint32_t size,
                            void* args, unsigned int priority, unsigned int core = 0);
    void osThreadDelete(TaskHandle_t thread_id);
    uint32_t osThreadGetStackSpace(TaskHandle_t thread_id);
    osStatus_t osMessageQueueGet(QueueHandle_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout);
    osStatus_t osMessageQueueGetIRQ(QueueHandle_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout);
    osStatus_t osMessageQueuePut(QueueHandle_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout);
    osStatus_t osMessageQueuePutIRQ(QueueHandle_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout);
    uint32_t osMessageQueueGetCount(QueueHandle_t mq_id);
    uint32_t osMessageQueueGetCountIRQ(QueueHandle_t mq_id);
    uint32_t osMessageQueueGetSpace(QueueHandle_t mq_id);
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    uint32_t osMessageQueueGetSpaceIRQ(QueueHandle_t mq_id);
#endif
    void waitMessageQueueSpace(QueueHandle_t mq_id, uint32_t timeout);
    osStatus_t osMessageQueueReset(QueueHandle_t mq_id);
    SemaphoreHandle_t osSemaphoreNew();
    osStatus_t osSemaphoreRelease(SemaphoreHandle_t semaphore_id);
    osStatus_t osSemaphoreReleaseIRQ(SemaphoreHandle_t semaphore_id);
    osStatus_t osSemaphoreAcquire(SemaphoreHandle_t semaphore_id, uint32_t timeout);
    osStatus_t osSemaphoreAcquireIRQ(SemaphoreHandle_t semaphore_id, uint32_t timeout);
    uint32_t osEventFlagsSet(EventGroupHandle_t ef_id, uint32_t flags);
    uint32_t osEventFlagsClear(EventGroupHandle_t ef_id, uint32_t flags);
    uint32_t osEventFlagsWait(EventGroupHandle_t ef_id, uint32_t flags, uint32_t options, uint32_t timeout);
    TimerHandle_t osTimerNew(const char *name, osTimerType_t type, void *argument, TimerCallbackFunction_t callback);
    osStatus_t osTimerStart(TimerHandle_t timer_id, uint32_t ticks);
    osStatus_t osTimerStop(TimerHandle_t timer_id);
    uint32_t osTimerIsRunning(TimerHandle_t timer_id);
    osStatus_t osTimerDelete(TimerHandle_t timer_id);

};

#endif

#endif /* INC_ERA_OS_HPP_ */
