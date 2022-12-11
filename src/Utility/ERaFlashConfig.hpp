#ifndef INC_ERA_FLASH_CONFIG_HPP_
#define INC_ERA_FLASH_CONFIG_HPP_

#if defined(ESP32) || \
    defined(ERA_FLASH_ESP32)
    #include <Utility/ERaFlashEsp32.hpp>
#elif defined(ESP8266) || \
    defined(ERA_FLASH_ESP8266)
    #include <Utility/ERaFlashEsp8266.hpp>
#elif defined(STM32F0xx) || defined(STM32F1xx) || \
    defined(STM32F2xx) || defined(STM32F3xx) ||   \
    defined(STM32F4xx) || defined(STM32F7xx) ||   \
    defined(ERA_FLASH_STM32)
    #include <Utility/ERaFlashStm32.hpp>
#elif defined(ARDUINO_ARCH_RP2040) || \
    defined(ERA_FLASH_LITTLEFS)
    #include <Utility/ERaFlashLittleFS.hpp>
#elif defined(__MBED__) || \
    defined(ERA_FLASH_MBED)
    #include <Utility/ERaFlashMbed.hpp>
#else
    #include <Utility/ERaFlashArduino.hpp>
#endif

#endif /* INC_ERA_FLASH_CONFIG_HPP_ */
