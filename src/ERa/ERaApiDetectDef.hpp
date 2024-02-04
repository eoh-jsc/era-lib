#ifndef INC_ERA_API_DETECT_DEFINE_HPP_
#define INC_ERA_API_DETECT_DEFINE_HPP_

#if defined(ESP32)
#elif defined(ARDUINO_ARCH_STM32)
    #include <ERa/ERaApiStm32Def.hpp>
#elif defined(PARTICLE) ||  \
    defined(SPARK)
    #include <ERa/ERaApiParticleDef.hpp>
#elif defined(__MBED__)
    #include <ERa/ERaApiMbedDef.hpp>
#elif defined(LINUX) &&      \
    (defined(RASPBERRY) ||   \
    defined(TINKER_BOARD) || \
    defined(ORANGE_PI))
    #include <ERaApiWiringPiDef.hpp>
#elif defined(LINUX)
    #include <ERaApiLinuxDef.hpp>
#else
    #include <ERa/ERaApiArduinoDef.hpp>
#endif

#endif /* INC_ERA_API_DETECT_DEFINE_HPP_ */
