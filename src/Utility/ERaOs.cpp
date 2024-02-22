#include <Arduino.h>
#include <Utility/ERaOs.hpp>

#if defined(ARDUINO) &&                 \
    !defined(__MBED__) &&               \
    (defined(ESP32) ||                  \
    defined(RTL8722DM) ||               \
    defined(ARDUINO_AMEBA) ||           \
    defined(ARDUINO_RTOS_STM32) ||      \
    defined(ARDUINO_RTOS_RENESAS) ||    \
    defined(ARDUINO_ARCH_RP2040))

#if defined(ARDUINO_RTOS_STM32)
    static volatile bool _osStarted {false};
#else
    static volatile bool _osStarted {true};
#endif

void ERaOs::osStartsScheduler() {
    if (_osStarted) {
        return;
    }
    _osStarted = true;
#if defined(ARDUINO_RTOS_STM32)
    vTaskStartScheduler();
    while (1) { delay(1000); }
#endif
}

void ERaOs::osDelay(uint32_t ticks) {
    if (ticks != 0U) {
        if (_osStarted) {
            vTaskDelay(ticks / portTICK_PERIOD_MS);
        }
        else {
            delay(ticks);
        }
    }
}

bool ERaOs::osStarted() {
    return _osStarted;
}

uint32_t ERaOs::osFreeHeapSize() {
    return xPortGetFreeHeapSize();
}

TaskHandle_t ERaOs::osThreadNew(void (*task)(void *args), const char* name, uint32_t size,
                                void* args, unsigned int priority, unsigned int core) {
    TaskHandle_t hTask = NULL;

#if defined(ESP32)
    #if !defined(CONFIG_FREERTOS_UNICORE)
        if (core < 2) {
            xTaskCreatePinnedToCore(task, name, size, args, priority, &hTask, core);
        }
        else {
    #endif
            xTaskCreate(task, name, size, args, priority, &hTask);
            (void)core;
    #if !defined(CONFIG_FREERTOS_UNICORE)
        }
    #endif
#else
    xTaskCreate(task, name, size, args, priority, &hTask);
    (void)core;
#endif

    return hTask;
}

void ERaOs::osThreadDelete(TaskHandle_t thread_id) {
    vTaskDelete(thread_id);
}

uint32_t ERaOs::osThreadGetStackSpace(TaskHandle_t thread_id) {
    uint32_t sz;

    sz = (uint32_t)(uxTaskGetStackHighWaterMark(thread_id) * sizeof(StackType_t));

    return (sz);
}

osStatus_t ERaOs::osMessageQueueGet(QueueHandle_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout) {
    osStatus_t stat;

    (void)msg_prio; /* Message priority is ignored */

    stat = osOK;

    if ((mq_id == NULL) || (msg_ptr == NULL)) {
        stat = osErrorParameter;
    }
    else {
        if (xQueueReceive(mq_id, msg_ptr, (TickType_t)timeout) != pdPASS) {
            if (timeout != 0U) {
                stat = osErrorTimeout;
            } else {
                stat = osErrorResource;
            }
        }
    }

    return (stat);
}

IRAM_ATTR
osStatus_t ERaOs::osMessageQueueGetIRQ(QueueHandle_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout) {
    osStatus_t stat;
    BaseType_t yield;

    (void)msg_prio; /* Message priority is ignored */

    stat = osOK;

    if ((mq_id == NULL) || (msg_ptr == NULL) || (timeout != 0U)) {
        stat = osErrorParameter;
    }
    else {
        yield = pdFALSE;

        if (xQueueReceiveFromISR(mq_id, msg_ptr, &yield) != pdPASS) {
            stat = osErrorResource;
        } else {
            osPortYIELD_FROM_ISR(yield);
        }
    }

    return (stat);
}

osStatus_t ERaOs::osMessageQueuePut(QueueHandle_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout) {
    osStatus_t stat;

    (void)msg_prio; /* Message priority is ignored */

    stat = osOK;

    if ((mq_id == NULL) || (msg_ptr == NULL)) {
        stat = osErrorParameter;
    }
    else {
        if (xQueueSendToBack(mq_id, msg_ptr, (TickType_t)timeout) != pdPASS) {
            if (timeout != 0U) {
                stat = osErrorTimeout;
            } else {
                stat = osErrorResource;
            }
        }
    }

    return (stat);
}

IRAM_ATTR
osStatus_t ERaOs::osMessageQueuePutIRQ(QueueHandle_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout) {
    osStatus_t stat;
    BaseType_t yield;

    (void)msg_prio; /* Message priority is ignored */

    stat = osOK;

    if ((mq_id == NULL) || (msg_ptr == NULL) || (timeout != 0U)) {
        stat = osErrorParameter;
    }
    else {
        yield = pdFALSE;

        if (xQueueSendToBackFromISR(mq_id, msg_ptr, &yield) != pdTRUE) {
            stat = osErrorResource;
        } else {
            osPortYIELD_FROM_ISR(yield);
        }
    }

    return (stat);
}

uint32_t ERaOs::osMessageQueueGetCount(QueueHandle_t mq_id) {
    UBaseType_t count;

    if (mq_id == NULL) {
        count = 0U;
    }
    else {
        count = uxQueueMessagesWaiting(mq_id);
    }

    return ((uint32_t)count);
}

IRAM_ATTR
uint32_t ERaOs::osMessageQueueGetCountIRQ(QueueHandle_t mq_id) {
    UBaseType_t count;

    if (mq_id == NULL) {
        count = 0U;
    }
    else {
        count = uxQueueMessagesWaitingFromISR(mq_id);
    }

    return ((uint32_t)count);
}

uint32_t ERaOs::osMessageQueueGetSpace(QueueHandle_t mq_id) {
    UBaseType_t space;

    if (mq_id == NULL) {
        space = 0U;
    }
    else {
        space = uxQueueSpacesAvailable(mq_id);
    }

    return ((uint32_t)space);
}

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    IRAM_ATTR
    uint32_t ERaOs::osMessageQueueGetSpaceIRQ(QueueHandle_t mq_id) {
        StaticQueue_t *mq = (StaticQueue_t *)mq_id;
        uint32_t space;

        if (mq == NULL) {
            space = 0U;
        }
        else {
            /* space = pxQueue->uxLength - pxQueue->uxMessagesWaiting; */
            space = mq->uxDummy4[1] - mq->uxDummy4[0];
        }

        return (space);
    }
#endif

void ERaOs::waitMessageQueueSpace(QueueHandle_t mq_id, uint32_t timeout) {
    unsigned long tick = millis();
    while (!ERaOs::osMessageQueueGetSpace(mq_id) && (millis() - tick < timeout)) {
        osDelay(100);
    }
}

osStatus_t ERaOs::osMessageQueueReset(QueueHandle_t mq_id) {
    osStatus_t stat;

    if (mq_id == NULL) {
        stat = osErrorParameter;
    }
    else {
        stat = osOK;
        (void)xQueueReset(mq_id);
    }

    return (stat);
}

SemaphoreHandle_t ERaOs::osSemaphoreNew() {
#if defined(ARDUINO_RTOS_STM32)
    if (!_osStarted) {
        return NULL;
    }
#endif
    return xSemaphoreCreateMutex();
}

osStatus_t ERaOs::osSemaphoreRelease(SemaphoreHandle_t semaphore_id) {
#if defined(ARDUINO_RTOS_STM32)
    if (!_osStarted) {
        return osOK;
    }
#endif
    osStatus_t stat;

    stat = osOK;

    if (semaphore_id == NULL) {
        stat = osErrorParameter;
    }
    else {
        if (xSemaphoreGive(semaphore_id) != pdPASS) {
            stat = osErrorResource;
        }
    }

    return (stat);
}

IRAM_ATTR
osStatus_t ERaOs::osSemaphoreReleaseIRQ(SemaphoreHandle_t semaphore_id) {
    osStatus_t stat;
    BaseType_t yield;

    stat = osOK;

    if (semaphore_id == NULL) {
        stat = osErrorParameter;
    }
    else {
        yield = pdFALSE;

        if (xSemaphoreGiveFromISR(semaphore_id, &yield) != pdTRUE) {
            stat = osErrorResource;
        } else {
            osPortYIELD_FROM_ISR(yield);
        }
    }

    return (stat);
}

osStatus_t ERaOs::osSemaphoreAcquire(SemaphoreHandle_t semaphore_id, uint32_t timeout) {
#if defined(ARDUINO_RTOS_STM32)
    if (!_osStarted) {
        return osOK;
    }
#endif
    osStatus_t stat;

    stat = osOK;

    if (semaphore_id == NULL) {
        stat = osErrorParameter;
    }
    else {
        if (xSemaphoreTake(semaphore_id, (TickType_t)timeout) != pdPASS) {
            if (timeout != 0U) {
                stat = osErrorTimeout;
            } else {
                stat = osErrorResource;
            }
        }
    }

    return (stat);
}

IRAM_ATTR
osStatus_t ERaOs::osSemaphoreAcquireIRQ(SemaphoreHandle_t semaphore_id, uint32_t timeout) {
    osStatus_t stat;
    BaseType_t yield;

    stat = osOK;

    if (semaphore_id == NULL) {
        stat = osErrorParameter;
    }
    else {
        yield = pdFALSE;

        if (xSemaphoreTakeFromISR(semaphore_id, &yield) != pdPASS) {
            stat = osErrorResource;
        } else {
            osPortYIELD_FROM_ISR(yield);
        }
    }

    (void)timeout;
    return (stat);
}

uint32_t ERaOs::osEventFlagsSet(EventGroupHandle_t ef_id, uint32_t flags) {
    uint32_t rflags;

    if (ef_id == NULL) {
        rflags = (uint32_t)osErrorParameter;
    }
    else {
        rflags = xEventGroupSetBits(ef_id, (EventBits_t)flags);
    }

    return (rflags);
}

uint32_t ERaOs::osEventFlagsClear(EventGroupHandle_t ef_id, uint32_t flags) {
    uint32_t rflags;

    if (ef_id == NULL) {
        rflags = (uint32_t)osErrorParameter;
    }
    else {
        rflags = xEventGroupClearBits(ef_id, (EventBits_t)flags);
    }

    return (rflags);
}

uint32_t ERaOs::osEventFlagsWait(EventGroupHandle_t ef_id, uint32_t flags, uint32_t options, uint32_t timeout) {
    BaseType_t wait_all;
    BaseType_t exit_clr;
    uint32_t rflags;

    if (ef_id == NULL) {
        rflags = (uint32_t)osErrorParameter;
    }
    else {
        if (options & osFlagsWaitAll) {
            wait_all = pdTRUE;
        } else {
            wait_all = pdFAIL;
        }

        if (options & osFlagsNoClear) {
            exit_clr = pdFAIL;
        } else {
            exit_clr = pdTRUE;
        }

        rflags = xEventGroupWaitBits(ef_id, (EventBits_t)flags, exit_clr, wait_all, (TickType_t)timeout);
    }

    return (rflags);
}

TimerHandle_t ERaOs::osTimerNew(const char *name, osTimerType_t type, void *argument, TimerCallbackFunction_t callback) {
    TimerHandle_t hTimer;
    UBaseType_t reload;

    if (name != NULL && callback != NULL) {
        if (type == osTimerOnce) {
            reload = pdFALSE;
        } else {
            reload = pdTRUE;
        }

        hTimer = xTimerCreate(name, pdMS_TO_TICKS(1000), reload, argument, callback);
    } else {
        hTimer = NULL;
    }

    return (hTimer);
}

osStatus_t ERaOs::osTimerStart(TimerHandle_t timer_id, uint32_t ticks) {
    osStatus_t stat;

    if (timer_id == NULL) {
        stat = osErrorParameter;
    }
    else {
        if (xTimerChangePeriod(timer_id, ticks, 0) == pdPASS) {
            stat = osOK;
        } else {
            stat = osErrorResource;
        }
    }

    return (stat);
}

osStatus_t ERaOs::osTimerStop(TimerHandle_t timer_id) {
    osStatus_t stat;

    if (timer_id == NULL) {
        stat = osErrorParameter;
    }
    else {
        if (xTimerIsTimerActive(timer_id) == pdFALSE) {
            stat = osErrorResource;
        }
        else {
            if (xTimerStop(timer_id, 0) == pdPASS) {
                stat = osOK;
            } else {
                stat = osError;
            }
        }
    }

    return (stat);
}

uint32_t ERaOs::osTimerIsRunning(TimerHandle_t timer_id) {
    uint32_t running;

    if (timer_id == NULL) {
        running = 0U;
    } else {
        running = (uint32_t)xTimerIsTimerActive(timer_id);
    }

    return (running);
}

osStatus_t ERaOs::osTimerDelete(TimerHandle_t timer_id) {
    osStatus_t stat;

    if (timer_id == NULL) {
        stat = osErrorParameter;
    }
    else {
        if (xTimerDelete(timer_id, 0) == pdPASS) {
            stat = osOK;
        } else {
            stat = osErrorResource;
        }
    }

    return (stat);
}

#endif
