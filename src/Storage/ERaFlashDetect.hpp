#ifndef INC_ERA_FLASH_DETECT_HPP_
#define INC_ERA_FLASH_DETECT_HPP_

#if defined(ESP32) ||                   \
    defined(ERA_FLASH_ESP32)
    #include <Storage/ERaFlashEsp32.hpp>
#elif defined(ESP8266) ||               \
    defined(ERA_FLASH_ESP8266)
    #include <Storage/ERaFlashEsp8266.hpp>
#elif defined(ARDUINO_ARCH_STM32) ||    \
    defined(ERA_FLASH_STM32)
    #include <Storage/ERaFlashStm32.hpp>
#elif !defined(__MBED__) &&             \
    (defined(ARDUINO_ARCH_RP2040) ||    \
    defined(ERA_FLASH_LITTLEFS))
    #include <Storage/ERaFlashLittleFS.hpp>
#elif defined(RTL8722DM) ||             \
    defined(ARDUINO_AMEBA)
    #include <Storage/ERaFlashRealtek.hpp>
#elif defined(ARDUINO_ARCH_SAMD)
    #include <Storage/ERaFlashSAMD.hpp>
#elif defined(ARDUINO_ARCH_ARM)
    #include <Storage/ERaFlashLogicrom.hpp>
#elif defined(__MBED__) ||              \
    defined(ERA_FLASH_MBED)
    #include <Storage/ERaFlashMbed.hpp>
#elif defined(LINUX)
    #include <Storage/ERaFlashLinux.hpp>
#else
    #include <Storage/ERaFlashArduino.hpp>
#endif

#endif /* INC_ERA_FLASH_DETECT_HPP_ */
