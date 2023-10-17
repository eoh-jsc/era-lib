#ifndef INC_ERA_LOGGER_DEFINE_HPP_
#define INC_ERA_LOGGER_DEFINE_HPP_

#if defined(ARDUINO) && \
    (defined(ESP32) || defined(ESP8266) || \
    (defined(ARDUINO_ARCH_RP2040) && !defined(__MBED__)))
    #define DIRECTORY_LOGGER  "/logger"
#elif defined(__MBED__)
    #define DIRECTORY_LOGGER  "/fs/logger"
#elif defined(LINUX)
    #define DIRECTORY_LOGGER  "database/logger"
#elif defined(ARDUINO_ARCH_STM32)
    #define DIRECTORY_LOGGER  "logger"
#else
    #define DIRECTORY_LOGGER  "logger"
#endif

#define LOGGER_MAX_PATH_LENGTH  100
#define LOGGER_LOG_INTERVAL     10 * 60 * 1000UL

#endif /* INC_ERA_LOGGER_DEFINE_HPP_ */
