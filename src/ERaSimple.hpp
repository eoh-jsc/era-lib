#ifndef INC_ERA_SIMPLE_HPP_
#define INC_ERA_SIMPLE_HPP_

#if defined(ARDUINO) && defined(ESP32)
    #if defined(ERA_USE_SSL)
        #include <ERaSimpleEsp32SSL.hpp>
    #else
        #include <ERaSimpleEsp32.hpp>
    #endif
#elif defined(ARDUINO) && defined(ESP8266)
    #if defined(ERA_USE_SSL)
        #include <ERaSimpleEsp8266SSL.hpp>
    #else
        #include <ERaSimpleEsp8266.hpp>
    #endif
#elif defined(ARDUINO) &&               \
    (defined(RTL8722DM) || defined(ARDUINO_AMEBA))
    #if defined(ERA_USE_SSL)
        #include <ERaSimpleRealtekSSLWiFi.hpp>
    #else
        #include <ERaSimpleRealtekWiFi.hpp>
    #endif
#elif defined(ARDUINO) && defined(ARDUINO_ARCH_STM32)
    #include <TinyGsmClient.hpp>
    #if defined(TINY_GSM_MODEM_HAS_WIFI_PNP)
        #include <ERaSimpleTiny.hpp>
    #elif defined(TINY_GSM_MODEM_HAS_WIFI)
        #include <ERaSimpleStm32WiFi.hpp>
    #else
        #include <ERaSimpleStm32Gsm.hpp>
    #endif
#elif defined(ARDUINO) &&               \
    !defined(__MBED__) && defined(ARDUINO_ARCH_RP2040)
    #include <TinyGsmClient.hpp>
    #if defined(TINY_GSM_MODEM_HAS_WIFI_PNP)
        #include <ERaSimpleTiny.hpp>
    #elif defined(TINY_GSM_MODEM_HAS_WIFI)
        #include <ERaSimpleRp2040WiFi.hpp>
    #else
        #include <ERaSimpleRp2040Gsm.hpp>
    #endif
#elif defined(PARTICLE) || defined(SPARK)
    #if defined(ERA_USE_SSL)
        #include <ERaSimpleParticleSSL.hpp>
    #else
        #include <ERaSimpleParticle.hpp>
    #endif
#elif defined(__MBED__)
    #include <TinyGsmClient.hpp>
    #if defined(TINY_GSM_MODEM_HAS_WIFI_PNP)
        #include <ERaSimpleTiny.hpp>
    #elif defined(TINY_GSM_MODEM_HAS_WIFI)
        #include <ERaSimpleMbedWiFi.hpp>
    #else
        #include <ERaSimpleMbedGsm.hpp>
    #endif
#else
    #warning "Please include a board-specific header file!"
#endif

#endif /* INC_ERA_SIMPLE_HPP_ */
