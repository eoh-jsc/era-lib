#ifndef INC_MVP_FLASH_DEFINE_HPP_
#define INC_MVP_FLASH_DEFINE_HPP_

#if defined(ARDUINO) && (defined(ESP32) || defined(ESP8266))
    #define FILENAME_PIN_CONFIG         "/pin/config.txt"
    #define FILENAME_MODBUS_CONFIG      "/modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL     "/modbus/control.txt"
#elif defined(LINUX)
    #define FILENAME_PIN_CONFIG         "~/pin/config.txt"
    #define FILENAME_MODBUS_CONFIG      "~/modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL     "~/modbus/control.txt"
#else
    #define FILENAME_PIN_CONFIG         "pin/config.txt"
    #define FILENAME_MODBUS_CONFIG      "modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL     "modbus/control.txt"
#endif

#endif /* INC_MVP_FLASH_DEFINE_HPP_ */