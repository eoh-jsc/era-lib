#ifndef INC_ERA_PULSE_WIRING_PI_HPP_
#define INC_ERA_PULSE_WIRING_PI_HPP_

#include <stdint.h>
#include <stddef.h>
#include <wiringPi.h>

#define WAIT_FOR_PIN_STATE(state)               \
    while (digitalRead(pin) != (state)) {       \
        if ((micros() - startTime) > timeout) { \
            return 0;                           \
        }                                       \
    }

static inline
unsigned long pulseIn(uint8_t pin, uint8_t state,
                    unsigned long timeout = 1000000UL) {
    const uint32_t startTime = micros();
    WAIT_FOR_PIN_STATE(!state);
    WAIT_FOR_PIN_STATE(state);
    const uint32_t pulseStartTime = micros();
    WAIT_FOR_PIN_STATE(!state);
    return (micros() - pulseStartTime);
}

static inline
unsigned long pulseInLong(uint8_t pin, uint8_t state,
                    unsigned long timeout = 1000000UL) {
    return pulseIn(pin, state, timeout);
}

#endif /* INC_ERA_PULSE_WIRING_PI_HPP_ */
