#ifndef INC_ERA_HPP_
#define INC_ERA_HPP_

#if defined(ARDUINO) && defined(ESP32)
    #include <ERaSimpleMBEsp32.hpp>
#elif defined(ARDUINO) && defined(ESP8266)
    #include <ERaSimpleEsp8266.hpp>
#elif defined(ARDUINO) && defined(STM32F4xx)
    #include <ERaSimpleStm32Gsm.hpp>
#else
    #warning "Please include a board-specific header file!"
#endif

#endif /* INC_ERA_HPP_ */
