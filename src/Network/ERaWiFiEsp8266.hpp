#ifndef INC_ERA_WIFI_ESP8266_HPP_
#define INC_ERA_WIFI_ESP8266_HPP_

template <class Transport>
void ERaPnP<Transport>::onWiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            ERA_LOG_WARNING(ERA_PSTR("WiFi"), ERA_PSTR("WiFi Disconnected"));
            break;
        case WIFI_EVENT_STAMODE_GOT_IP:
            ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("WiFi Got IP: %s, MAC: %s"), WiFi.localIP().toString().c_str(), WiFi.macAddress().c_str());
            break;
        case WIFI_EVENT_MODE_CHANGE:
            ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("AP Started SSID: %s, Ip: %s"), WiFi.softAPSSID().c_str(), WiFi.softAPIP().toString().c_str());
            break;
        case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
            ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("AP STA Connected"));
            break;
        case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
            ERA_LOG_WARNING(ERA_PSTR("WiFi"), ERA_PSTR("AP STA Disconnected"));
            break;
        case WIFI_EVENT_SOFTAPMODE_DISTRIBUTE_STA_IP:
            ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("AP STA IP Assigned"));
            break;
        default:
            break;
    }
}

#endif /* INC_ERA_WIFI_ESP8266_HPP_ */
