#ifndef INC_ERA_FLASH_DEFINE_HPP_
#define INC_ERA_FLASH_DEFINE_HPP_

#if defined(ARDUINO) && \
    (defined(ESP32) || defined(ESP8266) || \
    defined(ARDUINO_ARCH_RP2040))
    #define FILENAME_PIN_CONFIG         "/pin/config.txt"
    #define FILENAME_MODBUS_CONFIG      "/modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL     "/modbus/control.txt"
    #define FILENAME_ZIGBEE_DEVICES     "/zigbee/devices.txt"
#elif defined(__MBED__)
    #define FILENAME_PIN_CONFIG         "/fs/pin/config.txt"
    #define FILENAME_MODBUS_CONFIG      "/fs/modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL     "/fs/modbus/control.txt"
    #define FILENAME_ZIGBEE_DEVICES     "/fs/zigbee/devices.txt"
#elif defined(LINUX)
    #define FILENAME_PIN_CONFIG         "database/pin/config.txt"
    #define FILENAME_MODBUS_CONFIG      "database/modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL     "database/modbus/control.txt"
    #define FILENAME_ZIGBEE_DEVICES     "database/zigbee/devices.txt"
#else
    #define FILENAME_PIN_CONFIG         "pin/config.txt"
    #define FILENAME_MODBUS_CONFIG      "modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL     "modbus/control.txt"
    #define FILENAME_ZIGBEE_DEVICES     "zigbee/devices.txt"
#endif

#endif /* INC_ERA_FLASH_DEFINE_HPP_ */
