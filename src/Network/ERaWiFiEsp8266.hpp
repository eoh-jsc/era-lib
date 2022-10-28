#ifndef INC_ERA_WIFI_ESP8266_HPP_
#define INC_ERA_WIFI_ESP8266_HPP_

void ERaPnP::onWiFiEvent(WiFiEvent_t event) {
	switch (event) {
		case WIFI_EVENT_STAMODE_DISCONNECTED:
			ERA_LOG("WiFi", "Wifi Disconnected");
			break;
		case WIFI_EVENT_STAMODE_GOT_IP:
			ERA_LOG("WiFi", "Wifi Got IP: %s, MAC: %s", WiFi.localIP().toString().c_str(), WiFi.macAddress().c_str());
			break;
		case WIFI_EVENT_MODE_CHANGE:
			ERA_LOG("WiFi", "AP Started SSID: %s, Ip: %s", WiFi.softAPSSID().c_str(), WiFi.softAPIP().toString().c_str());
			break;
		case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
			ERA_LOG("WiFi", "AP STA Connected");
			break;
		case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
			ERA_LOG("WiFi", "AP STA Disconnected");
			break;
		case WIFI_EVENT_SOFTAPMODE_DISTRIBUTE_STA_IP:
			ERA_LOG("WiFi", "AP STA IP Assigned");
			break;
		default:
			break;
	}
}

#endif /* INC_ERA_WIFI_ESP8266_HPP_ */
