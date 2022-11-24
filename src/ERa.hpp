#ifndef INC_ERA_HPP_
#define INC_ERA_HPP_

#if defined(ARDUINO) && defined(ESP32)
    #include <ERaSimpleMBEsp32.hpp>
#elif defined(ARDUINO) && defined(ESP8266)
    #include <ERaSimpleMBEsp8266.hpp>
#elif defined(ARDUINO) && defined(STM32F4xx)
    #define ERA_NO_RTOS
    #include <ERa/ERaDetect.hpp>
    #define TINY_GSM_RX_BUFFER ERA_MQTT_BUFFER_SIZE
    #include <TinyGsmClient.h>
    #if defined(TINY_GSM_MODEM_HAS_WIFI)
        #include <ERaSimpleMBStm32WiFi.hpp>
    #else
        #include <ERaSimpleMBStm32Gsm.hpp>
    #endif
#else
    #warning "Please include a board-specific header file!"
#endif

#endif /* INC_ERA_HPP_ */