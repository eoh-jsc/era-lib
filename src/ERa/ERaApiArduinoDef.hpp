#ifndef INC_ERA_API_ARDUINO_DEFINE_HPP_
#define INC_ERA_API_ARDUINO_DEFINE_HPP_

#if defined(ESP8266)
    #define INPUT_PULLDOWN    INPUT_PULLDOWN_16
#elif defined(ARDUINO_ARCH_STM32)
    #define ANALOG            INPUT_ANALOG
#elif defined(__AVR__) ||           \
    defined(ARDUINO_ARCH_SAM) ||    \
    defined(ARDUINO_ARCH_ARC32)
    #define INPUT_PULLDOWN    INPUT
    #define OUTPUT_OPEN_DRAIN OUTPUT
#elif defined(ARDUINO_ARCH_SAMD)
    #define OUTPUT_OPEN_DRAIN OUTPUT
#elif defined(ARDUINO_ARCH_ARM)
    #define OUTPUT_OPEN_DRAIN OUTPUT
#elif !defined(__MBED__) &&         \
    defined(ARDUINO_ARCH_RP2040)
    #define OUTPUT_OPEN_DRAIN OUTPUT
#elif !defined(__MBED__) &&         \
    defined(ARDUINO_ARCH_NRF5)
    #define OUTPUT_OPEN_DRAIN OUTPUT
#elif defined(RTL8722DM) ||         \
    defined(ARDUINO_AMEBA)
    #define OUTPUT_OPEN_DRAIN OUTPUT_OPENDRAIN
#elif defined(ARDUINO_ARCH_RENESAS)
    #define OUTPUT_OPEN_DRAIN OUTPUT_OPENDRAIN
#elif defined(__MBED__)
    #define OUTPUT_OPEN_DRAIN (PinMode)OUTPUT_OPENDRAIN
#endif

#if !defined(ANALOG)
    #define ANALOG            0xC0
#endif

#endif /* INC_ERA_API_ARDUINO_DEFINE_HPP_ */
