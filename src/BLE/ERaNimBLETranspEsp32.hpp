#ifndef INC_ERA_NIM_BLE_TRANSP_ESP32_HPP_
#define INC_ERA_NIM_BLE_TRANSP_ESP32_HPP_

#if defined(__has_include)
    #if __has_include(<NimBLESecurity.h>)
        #include <BLE/ERaNimBLETransp.hpp>
    #endif
#endif

#if !defined(INC_ERA_NIM_BLE_TRANSP_HPP_)
    #include <BLE/ERaNimBLETransp2.hpp>
#endif

#endif /* INC_ERA_NIM_BLE_TRANSP_ESP32_HPP_ */
