#ifndef INC_MVP_UDP_ESP8266_HPP_
#define INC_MVP_UDP_ESP8266_HPP_

#include <Network/MVPUdp.hpp>

template <class Udp>
void MVPUdp<Udp>::getWiFiName(char* ptr, size_t size, bool withPrefix) {
	String macAddr = WiFi.macAddress();
	macAddr.replace(":", "");
	macAddr.toLowerCase();
    if (withPrefix) {
        snprintf(ptr, size, "eoh.mvp.%s", macAddr.c_str());
    } else {
        snprintf(ptr, size, "mvp.%s", macAddr.c_str());
    }
}

template <class Udp>
void MVPUdp<Udp>::getImeiChip(char* ptr, size_t size) {
	String macAddr = WiFi.macAddress();
	macAddr.replace(":", "");
	macAddr.toLowerCase();
#ifdef MVP_AUTH_TOKEN
    snprintf(ptr, size, MVP_AUTH_TOKEN);
#else
    if (this->authToken != nullptr) {
        snprintf(ptr, size, this->authToken);
    }
    else {
        snprintf(ptr, size, "MVP-%s", macAddr.c_str());
    }
#endif
}

#endif /* INC_MVP_UDP_ESP8266_HPP_ */