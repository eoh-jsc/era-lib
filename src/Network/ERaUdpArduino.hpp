#ifndef INC_ERA_UDP_ARDUINO_HPP_
#define INC_ERA_UDP_ARDUINO_HPP_

#include <Network/ERaUdp.hpp>

template <class Udp>
ERaString ERaUdp<Udp>::getChipID() {
    char mac[20] {0};
    uint8_t macAddr[6] {0};
    WiFi.macAddress(macAddr);
    FormatString(mac, "%02x%02x%02x%02x%02x%02x", macAddr[0], macAddr[1],
                                                    macAddr[2], macAddr[3],
                                                    macAddr[4], macAddr[5]);
    return ERaString(mac);
}

#endif /* INC_ERA_UDP_ARDUINO_HPP_ */
