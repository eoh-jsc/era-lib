#ifndef INC_MVP_HPP_
#define INC_MVP_HPP_

#if defined(ARDUINO) && defined(ESP32)
    #include <MVPEsp32.hpp>
#elif defined(ARDUINO) && defined(ESP8266)
    #include <MVPEsp8266.hpp>
#else
    #error This board not supported MVP now!
#endif

#endif /* INC_MVP_HPP_ */