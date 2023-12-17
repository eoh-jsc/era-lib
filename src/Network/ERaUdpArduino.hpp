#ifndef INC_ERA_UDP_ARDUINO_HPP_
#define INC_ERA_UDP_ARDUINO_HPP_

#include <Network/ERaUdp.hpp>

template <class Udp>
template <int size>
void ERaUdp<Udp>::getWiFiName(char(&ptr)[size], bool withPrefix) {
    char mac[20] {0};
    uint8_t macAddr[6] {0};
#if defined(RTL8722DM) ||     \
    defined(ARDUINO_AMEBA)
    WiFi.BSSID(macAddr);
#else
    WiFi.macAddress(macAddr);
#endif
    FormatString(mac, "%02x%02x%02x%02x%02x%02x", macAddr[0], macAddr[1],
                                                    macAddr[2], macAddr[3],
                                                    macAddr[4], macAddr[5]);
    ClearArray(ptr);
    if (withPrefix) {
        FormatString(ptr, "%s.%s.%s", this->pORG, this->pModel, mac);
    }
    else {
        FormatString(ptr, "%s.%s", this->pORG, mac);
    }
    ERaToLowerCase(ptr);
}

template <class Udp>
template <int size>
void ERaUdp<Udp>::getImeiChip(char(&ptr)[size]) {
    char mac[20] {0};
    uint8_t macAddr[6] {0};
#if defined(RTL8722DM) ||     \
    defined(ARDUINO_AMEBA)
    WiFi.BSSID(macAddr);
#else
    WiFi.macAddress(macAddr);
#endif
    FormatString(mac, "%02x%02x%02x%02x%02x%02x", macAddr[0], macAddr[1],
                                                    macAddr[2], macAddr[3],
                                                    macAddr[4], macAddr[5]);
    ClearArray(ptr);
#if defined(ERA_AUTH_TOKEN)
    FormatString(ptr, ERA_AUTH_TOKEN);
#else
    if ((this->authToken != nullptr) && strlen(this->authToken)) {
        FormatString(ptr, this->authToken);
    }
    else {
        FormatString(ptr, "ERA-%s", mac);
    }
#endif
}

#endif /* INC_ERA_UDP_ARDUINO_HPP_ */
