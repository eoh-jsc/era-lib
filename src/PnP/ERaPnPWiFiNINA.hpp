#ifndef INC_ERA_PNP_WIFI_NINA_HPP_
#define INC_ERA_PNP_WIFI_NINA_HPP_

#define ERA_ARDUINO_WIFI
#define ERA_ARDUINO_WIFI_NINA

#include <WiFiNINA.h>
#include <PnP/ERaPnPArduino.hpp>

template <class Transport>
void ERaPnP<Transport>::addEncryptionType(uint8_t networkItem, cJSON* const item) {
    switch (WiFi.encryptionType(networkItem)) {
        case wl_enc_type::ENC_TYPE_WEP:
            cJSON_AddStringToObject(item, "encryption", "WEP");
            break;
        case wl_enc_type::ENC_TYPE_TKIP:
            cJSON_AddStringToObject(item, "encryption", "WPA/WPA2");
            break;
        case wl_enc_type::ENC_TYPE_CCMP:
            cJSON_AddStringToObject(item, "encryption", "WPA/WPA2");
            break;
        case wl_enc_type::ENC_TYPE_NONE:
            cJSON_AddStringToObject(item, "encryption", "OPEN");
            break;
        default:
            cJSON_AddStringToObject(item, "encryption", "unknown");
            break;
    }
}

#endif /* INC_ERA_PNP_WIFI_NINA_HPP_ */
