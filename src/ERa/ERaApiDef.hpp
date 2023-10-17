#ifndef INC_ERA_API_DEFINE_HPP_
#define INC_ERA_API_DEFINE_HPP_

#include <stdint.h>
#include <ERa/ERaParam.hpp>

#if !defined(ERA_MAX_EVENTS)
	#if defined(ERA_NO_RTOS)
		#define ERA_MAX_EVENTS			0
    #elif defined(ERA_ZIGBEE) ||	\
		defined(ERA_SPECIFIC)
        #define ERA_MAX_EVENTS			20
    #elif defined(ERA_MODBUS)
        #define ERA_MAX_EVENTS			5
    #else
        #define ERA_MAX_EVENTS			0
    #endif
#endif

#if defined(analogInputToDigitalPin)
	#define ERA_DECODE_PIN(pin)			analogInputToDigitalPin(pin)
#else
	#define ERA_DECODE_PIN(pin)			pin
#endif

#define ERA_INVALID_PIN					0xFF

#if defined(digitalPinIsValid)
	#define ERA_CHECK_PIN(pin)			if (!digitalPinIsValid(pin)) { continue; }
	#define ERA_CHECK_PIN_RETURN(pin)	if (!digitalPinIsValid(pin)) { return; }
#else
	#define ERA_CHECK_PIN(pin)			if (pin == ERA_INVALID_PIN) { continue; }
	#define ERA_CHECK_PIN_RETURN(pin)	if (pin == ERA_INVALID_PIN) { return; }
#endif

#define ERA_DECODE_PIN_NUMBER(pin)		pin
#define ERA_DECODE_PIN_NAME(pin) 		(((pin[0] == 'a') || (pin[0] == 'A')) ? 	\
										ERA_DECODE_PIN(atoi(pin + 1)) : ERA_DECODE_PIN_NUMBER(atoi(pin)))

enum ERaTypeWriteT {
	ERA_WRITE_VIRTUAL_PIN = 0,
	ERA_WRITE_VIRTUAL_PIN_MULTI = 1,
	ERA_WRITE_DIGITAL_PIN = 2,
	ERA_WRITE_ANALOG_PIN = 3,
	ERA_WRITE_PWM_PIN = 4,
	ERA_WRITE_PIN = 5,
	ERA_WRITE_CONFIG_ID = 6,
	ERA_WRITE_CONFIG_ID_MULTI = 7,
	ERA_WRITE_MODBUS_DATA = 8,
	ERA_WRITE_ZIGBEE_DATA = 9,
	ERA_WRITE_SPECIFIC_DATA = 10,
	ERA_WRITE_MAX = 0xFF
};

typedef struct __ERaRsp_t {
    uint8_t type;
	bool retained;
    ERaParam id;
    ERaParam param;
} ERaRsp_t;

typedef struct __ERaEvent_t {
	uint8_t type;
    bool specific;
    bool retained;
	void* id;
	void* data;
} ERaEvent_t;

#endif /* INC_ERA_API_DEFINE_HPP_ */
