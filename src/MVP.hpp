#ifndef INC_MVP_HPP_
#define INC_MVP_HPP_

#if defined(ARDUINO) && defined(ESP32)
    #include <MVPSimpleMBEsp32.hpp>
#elif defined(ARDUINO) && defined(ESP8266)
    #include <MVPSimpleEsp8266.hpp>
#elif defined(ARDUINO) && defined(STM32F4xx)
    #include <MVPSimpleStm32Gsm.hpp>
#else
    #warning "Please include a board-specific header file!"
#endif

#endif /* INC_MVP_HPP_ */