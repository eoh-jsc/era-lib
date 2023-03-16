#ifndef INC_ERA_PWM_WIRING_PI_HPP_
#define INC_ERA_PWM_WIRING_PI_HPP_

#include <math.h>
#include <stddef.h>
#include <wiringPi.h>
#include <softPwm.h>

#define PWM_RANGE           100
#define NUMBER_HARD_PIN_PI  4

template <class S, typename... Args>
class HardPin {
public:
    static S* pin(Args... tail) {
        static S _pin[] {tail...};
        return _pin;
    }
};

#define hardPin HardPin<int, int, int, int, int>::pin(12, 13, 18, 19)

static inline
bool isHardPin(int pin) {
    static int* _hardPin = hardPin;
    for (size_t i = 0; i < NUMBER_HARD_PIN_PI; ++i) {
        if (_hardPin[i] == pin) {
            return true;
        }
    }
    return false;
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
