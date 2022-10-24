#ifndef INC_MVP_MODBUS_CONFIG_HPP_
#define INC_MVP_MODBUS_CONFIG_HPP_

#ifndef MODBUS_RXD_Pin
    #define MODBUS_RXD_Pin      16
#endif

#ifndef MODBUS_TXD_Pin
    #define MODBUS_TXD_Pin      17
#endif

#define MODBUS_BUFFER_SIZE      256
#define MAX_TIMEOUT_MODBUS      1500

#define MODBUS_DATA_BUFFER_SIZE 1024

#endif /* INC_MVP_MODBUS_CONFIG_HPP_ */
