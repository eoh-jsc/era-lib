#ifndef INC_ERA_UDP_ESP8266_HPP_
#define INC_ERA_UDP_ESP8266_HPP_

#include <Network/ERaUdp.hpp>

template <class Udp>
ERaString ERaUdp<Udp>::getChipID() {
    String macAddr = WiFi.macAddress();
    macAddr.replace(":", "");
    macAddr.toLowerCase();
    return ERaString(macAddr.c_str());
}

#endif /* INC_ERA_UDP_ESP8266_HPP_ */
