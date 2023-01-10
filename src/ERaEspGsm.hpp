#ifndef INC_ERA_ESP_GSM_HPP_
#define INC_ERA_ESP_GSM_HPP_

#if defined(ARDUINO) && defined(ESP32)
    #include <ERaSimpleMBEsp32Gsm.hpp>
#elif defined(ARDUINO) && defined(ESP8266)
    #include <ERaSimpleEsp8266Gsm.hpp>
#else
    #warning "Please include a board-specific header file!"
#endif

#endif /* INC_ERA_ESP_GSM_HPP_ */
