#ifndef INC_ERA_OTA_STORAGE_HPP_
#define INC_ERA_OTA_STORAGE_HPP_

#if defined(__MBED__) &&                    \
    defined(ARDUINO_ARCH_RP2040)
    #include "utility/ERaOTAStorageRP2040.hpp"
#elif defined(ARDUINO_ARCH_SAMD) ||         \
    defined(ARDUINO_ARCH_NRF5)
    #include <InternalStorage.h>
#elif defined(__AVR__)
    #include <InternalStorageAVR.h>
#elif !defined(__MBED__) &&                 \
    defined(ARDUINO_ARCH_RP2040)
    #include <InternalStorageRP2.h>
#elif defined(ARDUINO_ARCH_STM32)
    #include <InternalStorageSTM32.h>
#else
    #include <ArduinoOTA.h>
#endif

#endif /* INC_ERA_OTA_STORAGE_HPP_ */
