#ifndef INC_ERA_UDP_ESP8266_HPP_
#define INC_ERA_UDP_ESP8266_HPP_

#include <Network/ERaUdp.hpp>

template <class Udp>
void ERaUdp<Udp>::getWiFiName(char* ptr, size_t size, bool withPrefix) {
	String macAddr = WiFi.macAddress();
	macAddr.replace(":", "");
	macAddr.toLowerCase();
    if (withPrefix) {
        snprintf(ptr, size, "eoh.era.%s", macAddr.c_str());
    } else {
        snprintf(ptr, size, "era.%s", macAddr.c_str());
    }
}

template <class Udp>
void ERaUdp<Udp>::getImeiChip(char* ptr, size_t size) {
	String macAddr = WiFi.macAddress();
	macAddr.replace(":", "");
	macAddr.toLowerCase();
#ifdef ERA_AUTH_TOKEN
    snprintf(ptr, size, ERA_AUTH_TOKEN);
#else
    if (this->authToken != nullptr) {
        snprintf(ptr, size, this->authToken);
    }
    else {
        snprintf(ptr, size, "ERA-%s", macAddr.c_str());
    }
#endif
}

#endif /* INC_ERA_UDP_ESP8266_HPP_ */
