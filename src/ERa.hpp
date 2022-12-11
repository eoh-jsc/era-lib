#ifndef INC_ERA_HPP_
#define INC_ERA_HPP_

#if defined(ARDUINO) && defined(ESP32)
    #if defined(ERA_USE_SSL)
        #include <ERaEsp32SSL.hpp>
    #else
        #include <ERaEsp32.hpp>
    #endif
#elif defined(ARDUINO) && defined(ESP8266)
    #if defined(ERA_USE_SSL)
        #include <ERaEsp8266SSL.hpp>
    #else
        #include <ERaEsp8266.hpp>
    #endif
#elif defined(ARDUINO) &&                        \
    (defined(STM32F0xx) || defined(STM32F1xx) || \
    defined(STM32F2xx) || defined(STM32F3xx) ||  \
    defined(STM32F4xx) || defined(STM32F7xx))
    #define ERA_NO_RTOS
    #include <ERa/ERaDetect.hpp>
    #define TINY_GSM_RX_BUFFER ERA_MQTT_BUFFER_SIZE
    #include <TinyGsmClient.h>
    #if defined(TINY_GSM_MODEM_HAS_WIFI)
        #include <ERaSimpleMBStm32WiFi.hpp>
    #else
        #include <ERaSimpleMBStm32Gsm.hpp>
    #endif
#elif defined(ARDUINO) && defined(ARDUINO_ARCH_RP2040)
    #define ERA_NO_RTOS
    #include <ERa/ERaDetect.hpp>
    #define TINY_GSM_RX_BUFFER ERA_MQTT_BUFFER_SIZE
    #include <TinyGsmClient.h>
    #if defined(TINY_GSM_MODEM_HAS_WIFI)
        #include <ERaSimpleMBRp2040WiFi.hpp>
    #else
        #include <ERaSimpleMBRp2040Gsm.hpp>
    #endif
#elif defined(__MBED__)
    #define ERA_NO_RTOS
    #include <ERa/ERaDetect.hpp>
    #define TINY_GSM_RX_BUFFER ERA_MQTT_BUFFER_SIZE
    #include <TinyGsmClient.h>
    #if defined(TINY_GSM_MODEM_HAS_WIFI)
        #include <ERaSimpleMBMbedWiFi.hpp>
    #else
        #include <ERaSimpleMBMbedGsm.hpp>
    #endif
#else
    #warning "Please include a board-specific header file!"
#endif

#endif /* INC_ERA_HPP_ */
