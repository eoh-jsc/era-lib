#ifndef INC_ERA_UDP_ESP32_HPP_
#define INC_ERA_UDP_ESP32_HPP_

#include <Network/ERaUdp.hpp>

template <class Udp>
template <int size>
void ERaUdp<Udp>::getWiFiName(char(&ptr)[size], bool withPrefix) {
    const uint64_t chipId = ESP.getEfuseMac();
    uint64_t unique {0};
    for (int i = 0; i < 41; i = i + 8) {
        unique |= ((chipId >> (40 - i)) & 0xff) << i;
    }
    ClearArray(ptr);
    if (withPrefix) {
        FormatString(ptr, "%s.%s.%04x%08x", this->pORG, this->pModel,
                                            static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    }
    else {
        FormatString(ptr, "%s.%04x%08x", this->pORG, static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    }
    ERaToLowerCase(ptr);
}

template <class Udp>
template <int size>
void ERaUdp<Udp>::getImeiChip(char(&ptr)[size]) {
    const uint64_t chipId = ESP.getEfuseMac();
    uint64_t unique {0};
    for (int i = 0; i < 41; i = i + 8) {
        unique |= ((chipId >> (40 - i)) & 0xff) << i;
    }
    ClearArray(ptr);
#if defined(ERA_AUTH_TOKEN)
    FormatString(ptr, ERA_AUTH_TOKEN);
#else
    if ((this->authToken != nullptr) && strlen(this->authToken)) {
        FormatString(ptr, this->authToken);
    }
    else {
        FormatString(ptr, "ERA-%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    }
#endif
}

#endif /* INC_ERA_UDP_ESP32_HPP_ */
