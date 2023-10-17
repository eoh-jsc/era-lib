#ifndef INC_ERA_ZIGBEE_DETECT_HPP_
#define INC_ERA_ZIGBEE_DETECT_HPP_

#if defined(ESP32)
    #include <Zigbee/ERaZigbeeEsp32.hpp>
#elif defined(ARDUINO_ARCH_STM32)
    #include <Zigbee/ERaZigbeeStm32.hpp>
#elif defined(LINUX)
    #include <Zigbee/ERaZigbeeLinux.hpp>
#else
    #include <Zigbee/ERaZigbeeArduino.hpp>
#endif

#endif /* INC_ERA_ZIGBEE_DETECT_HPP_ */
