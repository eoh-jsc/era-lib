#ifndef INC_ERA_OTA_STORAGE_HPP_
#define INC_ERA_OTA_STORAGE_HPP_

#if defined(__MBED__) &&                    \
    defined(ARDUINO_ARCH_RP2040)
    #include "utility/ERaOTAStorageRP2040.hpp"
#elif defined(ARDUINO_ARCH_STM32)
    #include "utility/ERaOTAStorageSTM32.hpp"
#elif defined(ARDUINO_ARCH_SAMD) ||         \
    defined(ARDUINO_ARCH_NRF5)
    #include <InternalStorage.h>
#elif defined(__AVR__) &&                   \
    (FLASHEND >= 0xFFFF)
    #include <InternalStorageAVR.h>
#elif defined(ESP32) || defined(ESP8266)
    #include <InternalStorageESP.h>
#elif !defined(__MBED__) &&                 \
    defined(ARDUINO_ARCH_RP2040)
    #include <InternalStorageRP2.h>
#elif defined(ARDUINO_ARCH_RENESAS_UNO)
    #include <InternalStorageRenesas.h>
#else

    class ERaOTAStorage
    {
    public:
        virtual int open(int) { return false; }
        virtual size_t write(uint8_t) { return 0; }
        virtual void close() {}
        virtual void clear() {}
        virtual void apply() {}
        virtual long maxSize() { return 0; }

        static ERaOTAStorage& instance() {
            static ERaOTAStorage _instance;
            return _instance;
        }
    };

    #define InternalStorage ERaOTAStorage::instance()

    #warning "OTA not support this MCU!"

#endif

#endif /* INC_ERA_OTA_STORAGE_HPP_ */
