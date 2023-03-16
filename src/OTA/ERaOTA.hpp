#ifndef INC_ERA_OTA_HPP_
#define INC_ERA_OTA_HPP_

#if defined(ESP32)
    #if defined(ERA_TINY_GSM)
        #include <OTA/ERaOTAEsp8266.hpp>
    #else
        #include <OTA/ERaOTAEsp32.hpp>
    #endif
#elif defined(ESP8266)
    #include <OTA/ERaOTAEsp8266.hpp>
#elif defined(LINUX)
    #include <OTA/ERaOTALinux.hpp>
#elif defined(ERA_OTA)
    #if !defined(__MBED__) &&                   \
        defined(ARDUINO_ARCH_RP2040)
        #include <OTA/ERaOTABase.hpp>
    #elif defined(ARDUINO_PORTENTA_H7_M4) ||    \
        defined(ARDUINO_PORTENTA_H7_M7) ||      \
        defined(ARDUINO_NICLA_VISION)
        #include <OTA/ERaOTAPortentaH7.hpp>
    #else
        #include <OTA/ERaOTAArduino.hpp>
    #endif
#endif

#endif /* INC_ERA_OTA_HPP_ */
