#ifndef INC_ERA_API_WIRING_PI_DEFINE_HPP_
#define INC_ERA_API_WIRING_PI_DEFINE_HPP_

#define INPUT_PULLUP            0x10
#define INPUT_PULLDOWN          0x11
#define OUTPUT_OPEN_DRAIN       OUTPUT
#define ANALOG                  0xC0
#define PWM                     PWM_OUTPUT

#if defined(RASPBERRY)
    #define MAX_GPIO_WIRING_PI  27
#elif defined(TINKER_BOARD) ||  \
    defined(ORANGE_PI)
    #define MAX_GPIO_WIRING_PI  40
#endif

#endif /* INC_ERA_API_WIRING_PI_DEFINE_HPP_ */
