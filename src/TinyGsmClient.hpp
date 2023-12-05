/**
 * @file       TinyGsmClient.hpp
 */

#ifndef INC_TINYGSM_CLIENT_HPP_
#define INC_TINYGSM_CLIENT_HPP_

#include <ERa/ERaDetect.hpp>

#define ERA_TINY_GSM

#if !defined(TINY_GSM_RX_BUFFER)
    #if defined(ERA_GSM_RX_BUFFER)
        #define TINY_GSM_RX_BUFFER              ERA_GSM_RX_BUFFER
    #else
        #define TINY_GSM_RX_BUFFER              ERA_MAX_READ_BYTES * 2
    #endif
#endif

#if !defined(TINY_GSM_MAX_SEND_RETRY)
    #if defined(ERA_GSM_MAX_SEND_RETRY)
        #define TINY_GSM_MAX_SEND_RETRY         ERA_GSM_MAX_SEND_RETRY
    #endif
#endif

#include <Compat/TinyGsm/TinyGsmFifo.hpp>

#if defined(TINY_GSM_MODEM_ESP32) ||            \
    defined(TINY_GSM_MODEM_ESP8266_AT_17) ||    \
    defined(TINY_GSM_MODEM_ESP8266_LEGACY)
    #if defined(TINY_GSM_MODEM_ESP8266_AT_17)
        #define TINY_GSM_PASSIVE_MODE_LEGACY
    #elif defined(TINY_GSM_MODEM_ESP8266_LEGACY)
        #define TINY_GSM_NO_MODEM_BUFFER
    #endif
    #define TINY_GSM_MODEM_HAS_WIFI
    #define TINY_GSM_MODEM_HAS_WIFI_PNP
    #include <TinyGsmCommon.h>
    #if defined(ARDUINO_DASH)
        #include <Compat/Arduino/Udp.h>
    #else
        #include <Udp.h>
    #endif
    #include <Compat/TinyGsm/TinyGsmClientESP32.hpp>
    typedef TinyGsmESP32                       TinyGsm;
    typedef TinyGsmESP32::GsmClientESP32       TinyGsmUdp;
    typedef TinyGsmESP32::GsmClientESP32       TinyGsmServer;
    typedef TinyGsmESP32::GsmClientESP32       TinyGsmClient;
    typedef TinyGsmESP32::GsmClientSecureESP32 TinyGsmClientSecure;
#elif defined(TINY_GSM_MODEM_EC20)
    #include <TinyGsmCommon.h>
    #include <Compat/TinyGsm/TinyGsmClientEC20.hpp>
    typedef TinyGsmEC20                        TinyGsm;
    typedef TinyGsmEC20::GsmClientEC20         TinyGsmClient;
#else
    #include <TinyGsmClient.h>
#endif

#endif  /* INC_TINYGSM_CLIENT_HPP_ */
