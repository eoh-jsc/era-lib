#ifndef INC_ERA_PNP_REALTEK_HPP_
#define INC_ERA_PNP_REALTEK_HPP_

#include <WiFi.h>
#include <PnP/ERaPnPArduino.hpp>

template <class Transport>
void ERaPnP<Transport>::addEncryptionType(uint8_t networkItem, cJSON* const item) {
    switch (WiFi.encryptionType(networkItem)) {
        case wl_enc_type::ENC_TYPE_WEP:
            cJSON_AddStringToObject(item, "encryption", "WEP");
            break;
        case wl_enc_type::ENC_TYPE_WPA:
            cJSON_AddStringToObject(item, "encryption", "WPA");
            break;
        case wl_enc_type::ENC_TYPE_WPA2:
            cJSON_AddStringToObject(item, "encryption", "WPA2");
            break;
        case wl_enc_type::ENC_TYPE_WPA3:
            cJSON_AddStringToObject(item, "encryption", "WPA3");
            break;
        case wl_enc_type::ENC_TYPE_NONE:
            cJSON_AddStringToObject(item, "encryption", "OPEN");
            break;
        default:
            cJSON_AddStringToObject(item, "encryption", "unknown");
            break;
    }
}

#endif /* INC_ERA_PNP_REALTEK_HPP_ */
