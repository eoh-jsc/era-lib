#ifndef INC_ERA_API_ARDUINO_DEFINE_HPP_
#define INC_ERA_API_ARDUINO_DEFINE_HPP_

#if defined(ESP8266)
    #define INPUT_PULLDOWN    INPUT_PULLDOWN_16
#elif defined(ARDUINO_ARCH_STM32)
    #define ANALOG            INPUT_ANALOG
#elif defined(__MBED__)
    #define OUTPUT_OPEN_DRAIN OUTPUT_OPENDRAIN
#elif defined(__AVR__)
    #define INPUT_PULLDOWN    INPUT
#elif defined(ARDUINO_ARCH_RP2040)
    #define OUTPUT_OPEN_DRAIN OUTPUT
#endif

#if !defined(ANALOG)
    #define ANALOG            0xC0
#endif

#endif /* INC_ERA_API_ARDUINO_DEFINE_HPP_ */
