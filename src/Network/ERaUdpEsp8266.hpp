#ifndef INC_ERA_UDP_ESP8266_HPP_
#define INC_ERA_UDP_ESP8266_HPP_

#include <Network/ERaUdp.hpp>

template <class Udp>
template <int size>
void ERaUdp<Udp>::getWiFiName(char(&ptr)[size], bool withPrefix) {
	String macAddr = WiFi.macAddress();
	macAddr.replace(":", "");
	macAddr.toLowerCase();
    ClearArray(ptr);
    if (withPrefix) {
        FormatString(ptr, "eoh.era.%s", macAddr.c_str());
    } else {
        FormatString(ptr, "era.%s", macAddr.c_str());
    }
}

template <class Udp>
template <int size>
void ERaUdp<Udp>::getImeiChip(char(&ptr)[size]) {
	String macAddr = WiFi.macAddress();
	macAddr.replace(":", "");
	macAddr.toLowerCase();
    ClearArray(ptr);
#ifdef ERA_AUTH_TOKEN
    FormatString(ptr, ERA_AUTH_TOKEN);
#else
    if (this->authToken != nullptr) {
        FormatString(ptr, this->authToken);
    }
    else {
        FormatString(ptr, "ERA-%s", macAddr.c_str());
    }
#endif
}

#endif /* INC_ERA_UDP_ESP8266_HPP_ */
