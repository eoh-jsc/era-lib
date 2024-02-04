#ifndef INC_ERA_MODBUS_CONFIG_HPP_
#define INC_ERA_MODBUS_CONFIG_HPP_

#ifndef MODBUS_BAUDRATE
    #define MODBUS_BAUDRATE             9600
#endif

#ifndef MODBUS_RXD_Pin
    #if defined(ARDUINO_ESP32C2_DEV)
        #define MODBUS_RXD_Pin          10
    #elif defined(ARDUINO_ESP32C3_DEV)
        #define MODBUS_RXD_Pin          6
    #elif defined(ARDUINO_ESP32C6_DEV)
        #define MODBUS_RXD_Pin          6
    #elif defined(ARDUINO_ESP32S2_DEV)
        #define MODBUS_RXD_Pin          21
    #elif defined(ARDUINO_ESP32S3_DEV)
        #define MODBUS_RXD_Pin          18
    #elif defined(ARDUINO_ESP32_S3_BOX)
        #define MODBUS_RXD_Pin          18
    #elif defined(ARDUINO_ESP32H2_DEV)
        #define MODBUS_RXD_Pin          0
    #elif defined(ARDUINO_ARCH_ESP32)
        #if defined(BOARD_HAS_PSRAM)
            #define MODBUS_RXD_Pin      15
        #else
            #define MODBUS_RXD_Pin      16
        #endif
    #else
        #define MODBUS_RXD_Pin          16
    #endif
#endif

#ifndef MODBUS_TXD_Pin
    #if defined(ARDUINO_ESP32C2_DEV)
        #define MODBUS_TXD_Pin          18
    #elif defined(ARDUINO_ESP32C3_DEV)
        #define MODBUS_TXD_Pin          7
    #elif defined(ARDUINO_ESP32C6_DEV)
        #define MODBUS_TXD_Pin          7
    #elif defined(ARDUINO_ESP32S2_DEV)
        #define MODBUS_TXD_Pin          17
    #elif defined(ARDUINO_ESP32S3_DEV)
        #define MODBUS_TXD_Pin          17
    #elif defined(ARDUINO_ESP32_S3_BOX)
        #define MODBUS_TXD_Pin          17
    #elif defined(ARDUINO_ESP32H2_DEV)
        #define MODBUS_TXD_Pin          1
    #elif defined(ARDUINO_ARCH_ESP32)
        #if defined(BOARD_HAS_PSRAM)
            #define MODBUS_TXD_Pin      13
        #else
            #define MODBUS_TXD_Pin      17
        #endif
    #else
        #define MODBUS_TXD_Pin          17
    #endif
#endif

#if !defined(MODBUS_STREAM_TIMEOUT)
    #define MODBUS_STREAM_TIMEOUT       5000
#endif

#define MODBUS_BUFFER_SIZE              256

#if !defined(DEFAULT_TIMEOUT_MODBUS)
    #define DEFAULT_TIMEOUT_MODBUS      1500
#endif

#if !defined(MODBUS_DATA_BUFFER_SIZE)
    #define MODBUS_DATA_BUFFER_SIZE     1024
#endif

#if !defined(MODBUS_MAX_ACTION)
    #define MODBUS_MAX_ACTION           10
#endif

#if !defined(ERA_MODBUS_YIELD)
    #if !defined(ERA_MODBUS_YIELD_MS)
        #if defined(ERA_NO_RTOS)
            #define ERA_MODBUS_YIELD_MS 1
        #else
            #define ERA_MODBUS_YIELD_MS 10
        #endif
    #endif
    #if !defined(ERA_NO_YIELD)
        #define ERA_MODBUS_YIELD()      { ERaDelay(ERA_MODBUS_YIELD_MS); }
    #else
        #define ERA_MODBUS_YIELD()      {}
    #endif
#endif

#if !defined(ERA_MODBUS_EXECUTE_MS)
    #define ERA_MODBUS_EXECUTE_MS       0UL
#endif

#if !defined(ERA_MODBUS_DELAYS_MS)
    #define ERA_MODBUS_DELAYS_MS        10UL
#endif

#if !defined(ERA_MODBUS_MUTEX_MS)
    #define ERA_MODBUS_MUTEX_MS         100UL
#endif

#if !defined(ERA_DISABLE_PNP_MODBUS)
    #define ERA_PNP_MODBUS
#endif

#endif /* INC_ERA_MODBUS_CONFIG_HPP_ */
