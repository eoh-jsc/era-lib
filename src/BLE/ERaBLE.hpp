#ifndef INC_ERA_BLE_HPP_
#define INC_ERA_BLE_HPP_

#if defined(ARDUINO) && defined(ESP32)
    #define ERA_BT_MULTI_CONNECT

    #if defined(ERA_BLE_ESP32_ORIGIN)
        /* OK, use the specified value */
    #elif defined(__has_include)
        #if __has_include(<NimBLEDevice.h>)
            #include <BLE/ERaNimBLETranspEsp32.hpp>
        #endif
    #endif

    #if !defined(INC_ERA_NIM_BLE_TRANSP_HPP_)
        #include <BLE/ERaBLETranspEsp32.hpp>
    #endif
#elif defined(ARDUINO) &&   \
    (defined(RTL8722DM) || defined(ARDUINO_AMEBA))
    #include <BLE/ERaBLETranspRealtek.hpp>
#elif defined(ARDUINO) && defined(ARDUINO_ARCH_SAMD)
    #include <BLE/ERaBLETranspArduino.hpp>
#else
    #error "This platform is not supported."
#endif

#endif /* INC_ERA_BLE_HPP_ */
