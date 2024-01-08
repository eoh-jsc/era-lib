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
#elif defined(ARDUINO) &&               \
    (defined(RTL8722DM) || defined(ARDUINO_AMEBA))
    #if defined(ERA_USE_SSL)
        #include <ERaRealtekSSLWiFi.hpp>
    #else
        #include <ERaRealtekWiFi.hpp>
    #endif
#elif defined(ARDUINO) && defined(ARDUINO_ARCH_STM32)
    #include <TinyGsmClient.hpp>
    #if defined(TINY_GSM_MODEM_HAS_WIFI_PNP)
        #include <ERaSimpleMBTiny.hpp>
    #elif defined(TINY_GSM_MODEM_HAS_WIFI)
        #include <ERaSimpleMBStm32WiFi.hpp>
    #else
        #include <ERaSimpleMBStm32Gsm.hpp>
    #endif
#elif defined(ARDUINO) &&               \
    !defined(__MBED__) && defined(ARDUINO_ARCH_RP2040)
    #include <TinyGsmClient.hpp>
    #if defined(TINY_GSM_MODEM_HAS_WIFI_PNP)
        #include <ERaSimpleMBTiny.hpp>
    #elif defined(TINY_GSM_MODEM_HAS_WIFI)
        #include <ERaSimpleMBRp2040WiFi.hpp>
    #else
        #include <ERaSimpleMBRp2040Gsm.hpp>
    #endif
#elif defined(PARTICLE) || defined(SPARK)
    #if defined(ERA_USE_SSL)
        #include <ERaSimpleMBParticleSSL.hpp>
    #else
        #include <ERaSimpleMBParticle.hpp>
    #endif
#elif defined(__MBED__)
    #include <TinyGsmClient.hpp>
    #if defined(TINY_GSM_MODEM_HAS_WIFI_PNP)
        #include <ERaSimpleMBTiny.hpp>
    #elif defined(TINY_GSM_MODEM_HAS_WIFI)
        #include <ERaSimpleMBMbedWiFi.hpp>
    #else
        #include <ERaSimpleMBMbedGsm.hpp>
    #endif
#elif defined(LINUX)
    #include <ERaLinux.hpp>
#else
    #warning "Please include a board-specific header file!"
#endif

#endif /* INC_ERA_HPP_ */
