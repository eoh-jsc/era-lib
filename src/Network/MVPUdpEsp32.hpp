#ifndef INC_MVP_UDP_ESP32_HPP_
#define INC_MVP_UDP_ESP32_HPP_

#include <Network/MVPUdp.hpp>

template <class Udp>
void MVPUdp<Udp>::getWiFiName(char* ptr, size_t size, bool withPrefix) {
    const uint64_t chipId = ESP.getEfuseMac();
    uint64_t unique {0};
    for (int i = 0; i < 41; i = i + 8) {
        unique |= ((chipId >> (40 - i)) & 0xff) << i;
    }
    if (withPrefix) {
        snprintf(ptr, size, "eoh.mvp.%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    } else {
        snprintf(ptr, size, "mvp.%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    }
}

template <class Udp>
void MVPUdp<Udp>::getImeiChip(char* ptr, size_t size) {
    const uint64_t chipId = ESP.getEfuseMac();
    uint64_t unique {0};
    for (int i = 0; i < 41; i = i + 8) {
        unique |= ((chipId >> (40 - i)) & 0xff) << i;
    }
#ifdef MVP_AUTH_TOKEN
    snprintf(ptr, size, MVP_AUTH_TOKEN);
#else
    if (this->authToken != nullptr) {
        snprintf(ptr, size, this->authToken);
    }
    else {
        snprintf(ptr, size, "MVP-%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    }
#endif
}

#endif /* INC_MVP_UDP_ESP32_HPP_ */