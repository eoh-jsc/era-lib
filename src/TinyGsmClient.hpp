/**
 * @file       TinyGsmClient.hpp
 */

#ifndef INC_TINYGSM_CLIENT_HPP_
#define INC_TINYGSM_CLIENT_HPP_

#include <ERa/ERaDetect.hpp>

#if !defined(TINY_GSM_RX_BUFFER)
    #if defined(ERA_GSM_RX_BUFFER)
        #define TINY_GSM_RX_BUFFER              ERA_GSM_RX_BUFFER
    #else
        #define TINY_GSM_RX_BUFFER              ERA_MQTT_BUFFER_SIZE
    #endif
#endif

#if !defined(TINY_GSM_MAX_SEND_RETRY)
    #if defined(ERA_GSM_MAX_SEND_RETRY)
        #define TINY_GSM_MAX_SEND_RETRY         ERA_GSM_MAX_SEND_RETRY
    #endif
#endif

#if defined(TINY_GSM_MODEM_ESP32)
    #define TINY_GSM_MODEM_HAS_WIFI
    #include <TinyGsmCommon.h>
    #include <Compat/TinyGsm/TinyGsmClientESP32.hpp>
    typedef TinyGsmESP32                       TinyGsm;
    typedef TinyGsmESP32::GsmClientESP32       TinyGsmClient;
    typedef TinyGsmESP32::GsmClientSecureESP32 TinyGsmClientSecure;
#else
    #include <TinyGsmClient.h>
#endif

#endif  /* INC_TINYGSM_CLIENT_HPP_ */
