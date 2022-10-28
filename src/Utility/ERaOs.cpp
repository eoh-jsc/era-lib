#include <Arduino.h>
#include <Utility/ERaOs.hpp>

#ifdef INC_FREERTOS_H

void osDelay(uint32_t ticks) {
    if (ticks != 0U) {
        vTaskDelay(ticks / portTICK_PERIOD_MS);
    }
}

uint32_t osThreadGetStackSpace(TaskHandle_t thread_id) {
	uint32_t sz;

	sz = (uint32_t)(uxTaskGetStackHighWaterMark(thread_id) * sizeof(StackType_t));

	return (sz);
}

osStatus_t osMessageQueueGet(QueueHandle_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout) {
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

osStatus_t IRAM_ATTR osMessageQueueGetIRQ(QueueHandle_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout) {
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

osStatus_t osMessageQueuePut(QueueHandle_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout) {
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

osStatus_t IRAM_ATTR osMessageQueuePutIRQ(QueueHandle_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout) {
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

uint32_t osMessageQueueGetCount(QueueHandle_t mq_id) {
    UBaseType_t count;

    if (mq_id == NULL) {
        count = 0U;
    }
    else {
        count = uxQueueMessagesWaiting(mq_id);
    }

    return ((uint32_t)count);
}

uint32_t IRAM_ATTR osMessageQueueGetCountIRQ(QueueHandle_t mq_id) {
    UBaseType_t count;

    if (mq_id == NULL) {
        count = 0U;
    }
    else {
        count = uxQueueMessagesWaitingFromISR(mq_id);
    }

    return ((uint32_t)count);
}

uint32_t osMessageQueueGetSpace(QueueHandle_t mq_id) {
	StaticQueue_t *mq = (StaticQueue_t *)mq_id;
	uint32_t space;

	if (mq == NULL) {
		space = 0U;
	}
	else {
		space = (uint32_t)uxQueueSpacesAvailable((QueueHandle_t)mq);
	}

	return (space);
}

uint32_t IRAM_ATTR osMessageQueueGetSpaceIRQ(QueueHandle_t mq_id) {
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

void waitMessageQueueSpace(QueueHandle_t mq_id, uint32_t timeout) {
	unsigned long tick = millis();
	while (!osMessageQueueGetSpace(mq_id) && (millis() - tick < timeout)) {
		osDelay(100);
	}
}

osStatus_t osMessageQueueReset(QueueHandle_t mq_id) {
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

osStatus_t osSemaphoreRelease(SemaphoreHandle_t semaphore_id) {
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

osStatus_t IRAM_ATTR osSemaphoreReleaseIRQ(SemaphoreHandle_t semaphore_id) {
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

osStatus_t osSemaphoreAcquire(SemaphoreHandle_t semaphore_id, uint32_t timeout) {
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

osStatus_t IRAM_ATTR osSemaphoreAcquireIRQ(SemaphoreHandle_t semaphore_id, uint32_t timeout) {
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

    return (stat);
}

uint32_t osEventFlagsSet(EventGroupHandle_t ef_id, uint32_t flags) {
	uint32_t rflags;

	if (ef_id == NULL) {
		rflags = (uint32_t)osErrorParameter;
	}
	else {
		rflags = xEventGroupSetBits(ef_id, (EventBits_t)flags);
	}

	return (rflags);
}

uint32_t osEventFlagsClear(EventGroupHandle_t ef_id, uint32_t flags) {
	uint32_t rflags;

	if (ef_id == NULL) {
		rflags = (uint32_t)osErrorParameter;
	}
	else {
		rflags = xEventGroupClearBits(ef_id, (EventBits_t)flags);
	}

	return (rflags);
}

uint32_t osEventFlagsWait(EventGroupHandle_t ef_id, uint32_t flags, uint32_t options, uint32_t timeout) {
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

TimerHandle_t osTimerNew(const char *name, osTimerType_t type, void *argument, TimerCallbackFunction_t callback) {
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

osStatus_t osTimerStart(TimerHandle_t timer_id, uint32_t ticks) {
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

osStatus_t osTimerStop(TimerHandle_t timer_id) {
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

uint32_t osTimerIsRunning(TimerHandle_t timer_id) {
	uint32_t running;

	if (timer_id == NULL) {
		running = 0U;
	} else {
		running = (uint32_t)xTimerIsTimerActive(timer_id);
	}

	return (running);
}

osStatus_t osTimerDelete(TimerHandle_t timer_id) {
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
