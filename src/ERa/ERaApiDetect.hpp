#ifndef INC_ERA_API_DETECT_HPP_
#define INC_ERA_API_DETECT_HPP_

#if defined(ESP32)
    #include <ERa/ERaApiEsp32.hpp>
#elif defined(ARDUINO_ARCH_STM32)
    #include <ERa/ERaApiStm32.hpp>
#elif defined(PARTICLE) ||   \
    defined(SPARK)
    #include <ERa/ERaApiParticle.hpp>
#elif defined(__MBED__)
    #include <ERa/ERaApiMbed.hpp>
#elif defined(LINUX) &&      \
    (defined(RASPBERRY) ||   \
    defined(TINKER_BOARD) || \
    defined(ORANGE_PI))
    #include <ERaApiWiringPi.hpp>
#elif defined(LINUX)
    #include <ERaApiLinux.hpp>
#else
    #include <ERa/ERaApiArduino.hpp>
#endif

#endif /* INC_ERA_API_DETECT_HPP_ */
