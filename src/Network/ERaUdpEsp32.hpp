#ifndef INC_ERA_UDP_ESP32_HPP_
#define INC_ERA_UDP_ESP32_HPP_

#include <Network/ERaUdp.hpp>

template <class Udp>
ERaString ERaUdp<Udp>::getChipID() {
    char id[20] {0};
    uint64_t unique {0};
    const uint64_t chipId = ESP.getEfuseMac();

    for (int i = 0; i < 41; i = i + 8) {
        unique |= ((chipId >> (40 - i)) & 0xff) << i;
    }
    FormatString(id, "%04x%08x", static_cast<uint16_t>(unique >> 32),
                                 static_cast<uint32_t>(unique));
    return ERaString(id);
}

#endif /* INC_ERA_UDP_ESP32_HPP_ */
