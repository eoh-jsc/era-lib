#ifndef INC_ERA_API_MBED_DEFINE_HPP_
#define INC_ERA_API_MBED_DEFINE_HPP_

#if defined(ARDUINO)
    #define OUTPUT_OPEN_DRAIN   (PinMode)OUTPUT_OPENDRAIN
#else
    #define OUTPUT_OPEN_DRAIN   OUTPUT_OPENDRAIN
#endif

#define ANALOG                  0xC0

#endif /* INC_ERA_API_MBED_DEFINE_HPP_ */
