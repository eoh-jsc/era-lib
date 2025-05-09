#ifndef INC_ERA_FLASH_DEFINE_HPP_
#define INC_ERA_FLASH_DEFINE_HPP_

#if defined(ARDUINO) &&                     \
    (defined(ESP32) || defined(ESP8266) ||  \
    (defined(ARDUINO_ARCH_RP2040) && !defined(__MBED__)))
    #define FILENAME_AUTO_CONFIG            "/auto/config.txt"
    #define FILENAME_BT_CONFIG              "/bt/config.txt"
    #define FILENAME_PIN_CONFIG             "/pin/config.txt"
    #define FILENAME_MODBUS_CONFIG          "/modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL         "/modbus/control.txt"
    #define FILENAME_ZIGBEE_CONFIG          "/zigbee/config.txt"
    #define FILENAME_ZIGBEE_DEVICES         "/zigbee/devices.txt"
    #define FILENAME_ZIGBEE_NETWORK         "/zigbee/network.txt"
    #define FILENAME_ZIGBEE_MANAGER_TABLE   "/zigbee/manager_table.txt"
    #define FILENAME_ZIGBEE_OPTIONS         "/zigbee/options.txt"
#elif defined(ARDUINO_ARCH_STM32)
    #define FILENAME_AUTO_CONFIG            "auto/config.txt"
    #define FILENAME_BT_CONFIG              "bt/config.txt"
    #define FILENAME_PIN_CONFIG             "pin/config.txt"
    #define FILENAME_MODBUS_CONFIG          "modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL         "modbus/control.txt"
    #define FILENAME_ZIGBEE_CONFIG          "zigbee/config.txt"
    #define FILENAME_ZIGBEE_DEVICES         "zigbee/devices.txt"
    #define FILENAME_ZIGBEE_NETWORK         "zigbee/network.txt"
    #define FILENAME_ZIGBEE_MANAGER_TABLE   "zigbee/manager_table.txt"
    #define FILENAME_ZIGBEE_OPTIONS         "zigbee/options.txt"
#elif defined(__MBED__) || defined(ARDUINO_ARCH_ARM) || \
    defined(ARDUINO_ARCH_OPENCPU)
    #define FILENAME_AUTO_CONFIG            "/fs/auto/config.txt"
    #define FILENAME_BT_CONFIG              "/fs/bt/config.txt"
    #define FILENAME_PIN_CONFIG             "/fs/pin/config.txt"
    #define FILENAME_MODBUS_CONFIG          "/fs/modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL         "/fs/modbus/control.txt"
    #define FILENAME_ZIGBEE_CONFIG          "/fs/zigbee/config.txt"
    #define FILENAME_ZIGBEE_DEVICES         "/fs/zigbee/devices.txt"
    #define FILENAME_ZIGBEE_NETWORK         "/fs/zigbee/network.txt"
    #define FILENAME_ZIGBEE_MANAGER_TABLE   "/fs/zigbee/manager_table.txt"
    #define FILENAME_ZIGBEE_OPTIONS         "/fs/zigbee/options.txt"
#elif defined(LINUX)
    #define FILENAME_AUTO_CONFIG            "database/auto/config.txt"
    #define FILENAME_BT_CONFIG              "database/bt/config.txt"
    #define FILENAME_PIN_CONFIG             "database/pin/config.txt"
    #define FILENAME_MODBUS_CONFIG          "database/modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL         "database/modbus/control.txt"
    #define FILENAME_ZIGBEE_CONFIG          "database/zigbee/config.txt"
    #define FILENAME_ZIGBEE_DEVICES         "database/zigbee/devices.txt"
    #define FILENAME_ZIGBEE_NETWORK         "database/zigbee/network.txt"
    #define FILENAME_ZIGBEE_MANAGER_TABLE   "database/zigbee/manager_table.txt"
    #define FILENAME_ZIGBEE_OPTIONS         "database/zigbee/options.txt"
#else
    #define FILENAME_AUTO_CONFIG            "auto/config.txt"
    #define FILENAME_BT_CONFIG              "bt/config.txt"
    #define FILENAME_PIN_CONFIG             "pin/config.txt"
    #define FILENAME_MODBUS_CONFIG          "modbus/config.txt"
    #define FILENAME_MODBUS_CONTROL         "modbus/control.txt"
    #define FILENAME_ZIGBEE_CONFIG          "zigbee/config.txt"
    #define FILENAME_ZIGBEE_DEVICES         "zigbee/devices.txt"
    #define FILENAME_ZIGBEE_NETWORK         "zigbee/network.txt"
    #define FILENAME_ZIGBEE_MANAGER_TABLE   "zigbee/manager_table.txt"
    #define FILENAME_ZIGBEE_OPTIONS         "zigbee/options.txt"
#endif

#endif /* INC_ERA_FLASH_DEFINE_HPP_ */
