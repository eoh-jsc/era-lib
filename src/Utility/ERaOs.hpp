#ifndef INC_ERA_OS_HPP_
#define INC_ERA_OS_HPP_

#if defined(ARDUINO) && 					 	        \
	(defined(ESP32) || defined(STM32F0xx) || 	        \
	defined(STM32F1xx) || defined(STM32F2xx) ||         \
	defined(STM32F3xx) || defined(STM32F4xx) ||         \
	defined(STM32F7xx) || defined(ARDUINO_ARCH_RP2040))

#define ERA_RTOS

#include <ERa/ERaDetect.hpp>

#if defined(ESP32)
	#include "freertos/FreeRTOS.h"
	#include "freertos/semphr.h"
	#include "freertos/task.h"
	#include "freertos/queue.h"
	#include "freertos/event_groups.h"
#else
	#include "STM32FreeRTOSConfig_extra.h"
	#include <FreeRTOS.h>
	#include <semphr.h>
	#include <task.h>
	#include <queue.h>
	#include <event_groups.h>

	#define IRAM_ATTR
#endif

#ifndef portEND_SWITCHING_ISR
	#define portEND_SWITCHING_ISR(xSwitchRequired) if(xSwitchRequired != pdFALSE) portYIELD()
#endif
#define osPortYIELD_FROM_ISR(x) portEND_SWITCHING_ISR(x)

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
	TaskHandle_t osThreadNew(void (*task)(void *args), const char* name, uint32_t size,
							void* args, unsigned int priority, unsigned int core = 0);
	void osThreadDelete(TaskHandle_t thread_id);
	uint32_t osThreadGetStackSpace(TaskHandle_t thread_id);
	osStatus_t osMessageQueueGet(QueueHandle_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout);
	osStatus_t IRAM_ATTR osMessageQueueGetIRQ(QueueHandle_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout);
	osStatus_t osMessageQueuePut(QueueHandle_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout);
	osStatus_t IRAM_ATTR osMessageQueuePutIRQ(QueueHandle_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout);
	uint32_t osMessageQueueGetCount(QueueHandle_t mq_id);
	uint32_t IRAM_ATTR osMessageQueueGetCountIRQ(QueueHandle_t mq_id);
	uint32_t osMessageQueueGetSpace(QueueHandle_t mq_id);
	uint32_t IRAM_ATTR osMessageQueueGetSpaceIRQ(QueueHandle_t mq_id);
	void waitMessageQueueSpace(QueueHandle_t mq_id, uint32_t timeout);
	osStatus_t osMessageQueueReset(QueueHandle_t mq_id);
	SemaphoreHandle_t osSemaphoreNew();
	osStatus_t osSemaphoreRelease(SemaphoreHandle_t semaphore_id);
	osStatus_t IRAM_ATTR osSemaphoreReleaseIRQ(SemaphoreHandle_t semaphore_id);
	osStatus_t osSemaphoreAcquire(SemaphoreHandle_t semaphore_id, uint32_t timeout);
	osStatus_t IRAM_ATTR osSemaphoreAcquireIRQ(SemaphoreHandle_t semaphore_id, uint32_t timeout);
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
