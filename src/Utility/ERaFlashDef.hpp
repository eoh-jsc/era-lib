#ifndef INC_ERA_FLASH_DEFINE_HPP_
#define INC_ERA_FLASH_DEFINE_HPP_

#if defined(ARDUINO) && (defined(ESP32) || defined(ESP8266))
    #define FILENAME_PIN_CONFIG         "/pin/config.txt"
    #define FILENAME_MODBUS_CONFIG      "/modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL     "/modbus/control.txt"
#elif defined(LINUX)
    #define FILENAME_PIN_CONFIG         "database/pin/config.txt"
    #define FILENAME_MODBUS_CONFIG      "database/modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL     "database/modbus/control.txt"
#else
    #define FILENAME_PIN_CONFIG         "pin/config.txt"
    #define FILENAME_MODBUS_CONFIG      "modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL     "modbus/control.txt"
#endif

#endif /* INC_ERA_FLASH_DEFINE_HPP_ */
