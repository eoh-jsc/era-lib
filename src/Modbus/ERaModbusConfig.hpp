#ifndef INC_ERA_MODBUS_CONFIG_HPP_
#define INC_ERA_MODBUS_CONFIG_HPP_

#ifndef MODBUS_BAUDRATE
    #define MODBUS_BAUDRATE         9600
#endif

#ifndef MODBUS_RXD_Pin
    #if defined(ARDUINO_ESP32C3_DEV)
        #define MODBUS_RXD_Pin      6
    #elif defined(ARDUINO_ESP32S2_DEV)
        #define MODBUS_RXD_Pin      21
    #elif defined(ARDUINO_ESP32S3_DEV)
        #define MODBUS_RXD_Pin      18
    #elif defined(ARDUINO_ESP32_S3_BOX)
        #define MODBUS_RXD_Pin      18
    #elif defined(ARDUINO_ARCH_ESP32)
        #define MODBUS_RXD_Pin      16
    #else
        #define MODBUS_RXD_Pin      16
    #endif
#endif

#ifndef MODBUS_TXD_Pin
    #if defined(ARDUINO_ESP32C3_DEV)
        #define MODBUS_TXD_Pin      7
    #elif defined(ARDUINO_ESP32S2_DEV)
        #define MODBUS_TXD_Pin      17
    #elif defined(ARDUINO_ESP32S3_DEV)
        #define MODBUS_TXD_Pin      17
    #elif defined(ARDUINO_ESP32_S3_BOX)
        #define MODBUS_TXD_Pin      17
    #elif defined(ARDUINO_ARCH_ESP32)
        #define MODBUS_TXD_Pin      17
    #else
        #define MODBUS_TXD_Pin      17
    #endif
#endif

#define MODBUS_BUFFER_SIZE          256
#define MAX_TIMEOUT_MODBUS          1500

#define MODBUS_DATA_BUFFER_SIZE     1024

#if !defined(ERA_MODBUS_YIELD)
    #if !defined(ERA_MODBUS_YIELD_MS)
        #define ERA_MODBUS_YIELD_MS 10
    #endif
    #if !defined(ERA_NO_YIELD)
        #define ERA_MODBUS_YIELD() { ERaDelay(ERA_MODBUS_YIELD_MS); }
    #else
        #define ERA_MODBUS_YIELD() {}
    #endif
#endif

#if !defined(ERA_MODBUS_EXECUTE_MS)
    #define ERA_MODBUS_EXECUTE_MS  0
#endif

#endif /* INC_ERA_MODBUS_CONFIG_HPP_ */
