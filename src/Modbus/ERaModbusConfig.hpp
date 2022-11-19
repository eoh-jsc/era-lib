#ifndef INC_ERA_MODBUS_CONFIG_HPP_
#define INC_ERA_MODBUS_CONFIG_HPP_

#ifndef MODBUS_RXD_Pin
    #define MODBUS_RXD_Pin      16
#endif

#ifndef MODBUS_TXD_Pin
    #define MODBUS_TXD_Pin      17
#endif

#define MODBUS_BUFFER_SIZE      256
#define MAX_TIMEOUT_MODBUS      1500

#define MODBUS_DATA_BUFFER_SIZE 1024

#if !defined(ERA_MODBUS_YIELD)
    #if !defined(ERA_MODBUS_YIELD_MS)
        #define ERA_MODBUS_YIELD_MS 10
    #endif
    #define ERA_MODBUS_YIELD() { ERaDelay(ERA_MODBUS_YIELD_MS); }
#endif

#endif /* INC_ERA_MODBUS_CONFIG_HPP_ */
