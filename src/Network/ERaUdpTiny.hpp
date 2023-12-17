#ifndef INC_ERA_UDP_TINY_HPP_
#define INC_ERA_UDP_TINY_HPP_

#include <Network/ERaUdp.hpp>

template <class Udp>
template <int size>
void ERaUdp<Udp>::getWiFiName(char(&ptr)[size], bool withPrefix) {
    String mac = "2706";
    TinyGsm* modem = ::getTinyModem();
    if (modem != nullptr) {
        mac = modem->BSSID();
        mac.replace(":", "");
    }
    ClearArray(ptr);
    if (withPrefix) {
        FormatString(ptr, "%s.%s.%s", this->pORG, this->pModel, mac.c_str());
    }
    else {
        FormatString(ptr, "%s.%s", this->pORG, mac.c_str());
    }
    ERaToLowerCase(ptr);
}

template <class Udp>
template <int size>
void ERaUdp<Udp>::getImeiChip(char(&ptr)[size]) {
    String mac = "2706";
    TinyGsm* modem = ::getTinyModem();
    if (modem != nullptr) {
        mac = modem->BSSID();
        mac.replace(":", "");
    }
    ClearArray(ptr);
#if defined(ERA_AUTH_TOKEN)
    FormatString(ptr, ERA_AUTH_TOKEN);
#else
    if ((this->authToken != nullptr) && strlen(this->authToken)) {
        FormatString(ptr, this->authToken);
    }
    else {
        FormatString(ptr, "ERA-%s", mac.c_str());
    }
#endif
}

#endif /* INC_ERA_UDP_TINY_HPP_ */
