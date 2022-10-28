#ifndef INC_ERA_UDP_ESP32_HPP_
#define INC_ERA_UDP_ESP32_HPP_

#include <Network/ERaUdp.hpp>

template <class Udp>
void ERaUdp<Udp>::getWiFiName(char* ptr, size_t size, bool withPrefix) {
    const uint64_t chipId = ESP.getEfuseMac();
    uint64_t unique {0};
    for (int i = 0; i < 41; i = i + 8) {
        unique |= ((chipId >> (40 - i)) & 0xff) << i;
    }
    if (withPrefix) {
        snprintf(ptr, size, "eoh.era.%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    } else {
        snprintf(ptr, size, "era.%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    }
}

template <class Udp>
void ERaUdp<Udp>::getImeiChip(char* ptr, size_t size) {
    const uint64_t chipId = ESP.getEfuseMac();
    uint64_t unique {0};
    for (int i = 0; i < 41; i = i + 8) {
        unique |= ((chipId >> (40 - i)) & 0xff) << i;
    }
#ifdef ERA_AUTH_TOKEN
    snprintf(ptr, size, ERA_AUTH_TOKEN);
#else
    if (this->authToken != nullptr) {
        snprintf(ptr, size, this->authToken);
    }
    else {
        snprintf(ptr, size, "ERA-%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    }
#endif
}

#endif /* INC_ERA_UDP_ESP32_HPP_ */
