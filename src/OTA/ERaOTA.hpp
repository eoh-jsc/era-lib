#ifndef INC_ERA_OTA_HPP_
#define INC_ERA_OTA_HPP_

#if defined(ESP32)
    #if defined(ERA_TINY_GSM) ||                \
        defined(ERA_ETHERNET_SPI)
        #include <OTA/ERaOTAEsp8266.hpp>
    #else
        #include <OTA/ERaOTAEsp32.hpp>
    #endif
#elif defined(ESP8266)
    #include <OTA/ERaOTAEsp8266.hpp>
#elif defined(RTL8722DM) ||                     \
    defined(ARDUINO_AMEBA)
    #include <OTA/ERaOTARealtek.hpp>
#elif defined(ARDUINO_ARCH_ARM)
    #include <OTA/ERaOTALogicrom.hpp>
#elif defined(LINUX)
    #include <OTA/ERaOTALinux.hpp>
#elif defined(ERA_OTA)
    #if !defined(__MBED__) &&                   \
        defined(ARDUINO_ARCH_RP2040)
        #if defined(__has_include) &&           \
            __has_include(<InternalStorageRP2.h>)
            #include <OTA/ERaOTABase.hpp>
        #else
            #include <OTA/ERaOTARp2040.hpp>
        #endif
    #elif defined(ARDUINO_PORTENTA_H7_M4) ||    \
        defined(ARDUINO_PORTENTA_H7_M7) ||      \
        defined(ARDUINO_NICLA_VISION)
        #include <OTA/ERaOTAPortentaH7.hpp>
    #elif defined(ARDUINO_ARCH_STM32) ||        \
        defined(ARDUINO_ARCH_RENESAS_UNO)
        #if defined(__has_include) &&           \
            __has_include(<ArduinoHttpClient.h>)
            #include <OTA/ERaOTAArduino.hpp>
        #else
            #include <OTA/ERaOTABase.hpp>
        #endif
    #else
        #include <OTA/ERaOTAArduino.hpp>
    #endif
#endif

#endif /* INC_ERA_OTA_HPP_ */
