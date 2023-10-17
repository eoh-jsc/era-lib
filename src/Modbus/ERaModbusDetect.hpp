#ifndef INC_ERA_MODBUS_DETECT_HPP_
#define INC_ERA_MODBUS_DETECT_HPP_

#if defined(ESP32)
    #include <Modbus/ERaModbusEsp32.hpp>
#elif defined(ARDUINO_ARCH_STM32)
    #include <Modbus/ERaModbusStm32.hpp>
#elif defined(LINUX)
    #include <Modbus/ERaModbusLinux.hpp>
#else
    #include <Modbus/ERaModbusArduino.hpp>
#endif

#endif /* INC_ERA_MODBUS_DETECT_HPP_ */
