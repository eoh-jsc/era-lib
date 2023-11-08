#ifndef INC_ERA_PWM_WIRING_PI_HPP_
#define INC_ERA_PWM_WIRING_PI_HPP_

#include <math.h>
#include <stddef.h>
#include <wiringPi.h>
#include <softPwm.h>

#define PWM_SOFT_RANGE          100
#define PWM_HARD_RANGE          1024

#if defined(RASPBERRY)
    #define NUMBER_HARD_PIN_PI  4
#elif defined(TINKER_BOARD)
    #define NUMBER_HARD_PIN_PI  2
#else
    #define NUMBER_HARD_PIN_PI  0
#endif

template <class S, typename... Args>
class HardPin {
public:
    static S* pin(Args... tail) {
        static S _pin[] {tail...};
        return _pin;
    }
};

#if defined(RASPBERRY)
    #define hardPin HardPin<int, int, int, int, int>::pin(12, 13, 18, 19)
#elif defined(TINKER_BOARD)
    #define hardPin HardPin<int, int, int>::pin(32, 33)
#endif

static inline
bool isHardPin(int pin) {
#if NUMBER_HARD_PIN_PI
    static int* _hardPin = hardPin;
    for (size_t i = 0; i < NUMBER_HARD_PIN_PI; ++i) {
        if (_hardPin[i] == pin) {
            return true;
        }
    }
    return false;
#else
    (void)pin;
    return false;
#endif
}

static inline
void pinModePi(int pin, int mode) {
    softPwmWrite(pin, 0);
#if defined(ORANGE_PI)
    softPwmStop(pin);
#endif
    pinMode(pin, mode);
}

static inline
void pwmSetRangePi(unsigned int range) {
#if NUMBER_HARD_PIN_PI
    /* Default range of 1024 */
    pwmSetRange(range);
#else
    (void)range;
#endif
}

static inline
void pwmModePi(int pin) {
    if (isHardPin(pin)) {
        pinMode(pin, PWM_OUTPUT);
    }
    else {
        softPwmWrite(pin, 0);
#if defined(ORANGE_PI)
        softPwmStop(pin);
        softPwmCreate(pin, 0, PWM_SOFT_RANGE);
#else
        pinMode(pin, SOFT_PWM_OUTPUT);
#endif
    }
}

static inline
void pwmWritePi(int pin, int value) {
    if (isHardPin(pin)) {
        value = round(value * 10.24f);
        pwmWrite(pin, value);
    }
    else {
        softPwmWrite(pin, value);
    }
}

#endif /* INC_ERA_PWM_WIRING_PI_HPP_ */
