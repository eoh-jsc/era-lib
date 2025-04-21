#ifndef INC_ERA_UDP_TINY_HPP_
#define INC_ERA_UDP_TINY_HPP_

#include <Network/ERaUdp.hpp>

template <class Udp>
ERaString ERaUdp<Udp>::getChipID() {
    String mac = "2706";
    TinyGsm* modem = ::getTinyModem();
    if (modem != nullptr) {
        mac = modem->BSSID();
        mac.replace(":", "");
    }
    return ERaString(mac.c_str());
}

#endif /* INC_ERA_UDP_TINY_HPP_ */
