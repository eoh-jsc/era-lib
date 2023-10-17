#ifndef INC_ERA_TASK_DETECT_HPP_
#define INC_ERA_TASK_DETECT_HPP_

#if defined(ESP32)
    #include <Task/ERaTaskEsp32.hpp>
#elif defined(ESP8266)
    #include <Task/ERaTaskEsp8266.hpp>
#elif defined(RTL8722DM) || \
    defined(ARDUINO_AMEBA)
    #include <Task/ERaTaskBase.hpp>
#elif defined(ARDUINO_ARCH_STM32)
    #include <Task/ERaTaskStm32.hpp>
#elif !defined(__MBED__) && \
    defined(ARDUINO_ARCH_RP2040)
    #include <Task/ERaTaskRp2040.hpp>
#elif defined(ARDUINO_ARCH_RENESAS)
    #include <Task/ERaTaskBase.hpp>
#elif defined(ARDUINO_ARCH_ARM)
    #include <Task/ERaTaskLogicrom.hpp>
#elif defined(PARTICLE) ||  \
    defined(SPARK)
    #include <Task/ERaTaskParticle.hpp>
#elif defined(__MBED__)
    #include <Task/ERaTaskMbed.hpp>
#else
    #include <Task/ERaTaskArduino.hpp>
#endif

#endif /* INC_ERA_TASK_DETECT_HPP_ */
