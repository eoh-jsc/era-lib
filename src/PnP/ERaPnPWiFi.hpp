#ifndef INC_ERA_PNP_WIFI_HPP_
#define INC_ERA_PNP_WIFI_HPP_

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
#else
    #include <WiFi.h>
#endif

#if defined(ESP32)
    #include <PnP/ERaPnPArduino.hpp>

    template <class Transport>
    void ERaPnP<Transport>::addEncryptionType(uint8_t networkItem, cJSON* const item) {
        switch (WiFi.encryptionType(networkItem)) {
            case wifi_auth_mode_t::WIFI_AUTH_WEP:
                cJSON_AddStringToObject(item, "encryption", "WEP");
                break;
            case wifi_auth_mode_t::WIFI_AUTH_WPA_PSK:
                cJSON_AddStringToObject(item, "encryption", "WPA/PSK");
                break;
            case wifi_auth_mode_t::WIFI_AUTH_WPA2_PSK:
                cJSON_AddStringToObject(item, "encryption", "WPA2/PSK");
                break;
            case wifi_auth_mode_t::WIFI_AUTH_WPA_WPA2_PSK:
                cJSON_AddStringToObject(item, "encryption", "WPA/WPA2/PSK");
                break;
            case wifi_auth_mode_t::WIFI_AUTH_OPEN:
                cJSON_AddStringToObject(item, "encryption", "OPEN");
                break;
            default:
                cJSON_AddStringToObject(item, "encryption", "unknown");
                break;
        }
    }
#elif defined(RTL8722DM) || defined(ARDUINO_AMEBA)
    #include <PnP/ERaPnPRealtek.hpp>
#else
    #define ERA_ARDUINO_WIFI

    #include <PnP/ERaPnPArduino.hpp>

    template <class Transport>
    void ERaPnP<Transport>::addEncryptionType(uint8_t networkItem, cJSON* const item) {
        switch (WiFi.encryptionType(networkItem)) {
            case wl_enc_type::ENC_TYPE_WEP:
                cJSON_AddStringToObject(item, "encryption", "WEP");
                break;
            case wl_enc_type::ENC_TYPE_TKIP:
                cJSON_AddStringToObject(item, "encryption", "WPA/PSK");
                break;
            case wl_enc_type::ENC_TYPE_CCMP:
                cJSON_AddStringToObject(item, "encryption", "WPA2/PSK");
                break;
            case wl_enc_type::ENC_TYPE_AUTO:
                cJSON_AddStringToObject(item, "encryption", "WPA/WPA2/PSK");
                break;
            case wl_enc_type::ENC_TYPE_NONE:
                cJSON_AddStringToObject(item, "encryption", "OPEN");
                break;
            default:
                cJSON_AddStringToObject(item, "encryption", "unknown");
                break;
        }
    }
#endif

#endif /* INC_ERA_PNP_WIFI_HPP_ */
