#ifndef INC_ERA_API_ARDUINO_DEFINE_HPP_
#define INC_ERA_API_ARDUINO_DEFINE_HPP_

#if defined(ESP8266)
    #define INPUT_PULLDOWN    INPUT_PULLDOWN_16
#elif defined(__AVR__)
    #define INPUT_PULLDOWN    INPUT
#endif

#if !defined(ANALOG)
    #if defined(INPUT_ANALOG)
        #define ANALOG        INPUT_ANALOG
    #else
        #define ANALOG        0xC0
    #endif
#endif

#endif /* INC_ERA_API_ARDUINO_DEFINE_HPP_ */
