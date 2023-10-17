#ifndef INC_ERA_PWM_WIRING_PI_HPP_
#define INC_ERA_PWM_WIRING_PI_HPP_

#include <math.h>
#include <stddef.h>
#include <wiringPi.h>
#include <softPwm.h>

#define PWM_RANGE               100

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
#endif
}

static inline
void pinModePi(int pin, int mode) {
    softPwmStop(pin);
    pinMode(pin, mode);
}

static inline
void pwmModePi(int pin) {
    if (isHardPin(pin)) {
        pinMode(pin, PWM_OUTPUT);
    }
    else {
        softPwmStop(pin);
        softPwmCreate(pin, 0, PWM_RANGE);
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
