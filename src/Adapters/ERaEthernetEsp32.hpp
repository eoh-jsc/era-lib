#ifndef INC_ERA_WIFI_ESP32_HPP_
#define INC_ERA_WIFI_ESP32_HPP_

#if (ESP_IDF_VERSION_MAJOR < 4)
    template <class Transport>
    void ERaEthernet<Transport>::onEthernetEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
        switch (event) {
            case SYSTEM_EVENT_SCAN_DONE:
                ERA_LOG(TAG, ERA_PSTR("WiFi Scan done"));
                break;
            case SYSTEM_EVENT_STA_START:
                ERA_LOG(TAG, ERA_PSTR("WiFi Started"));
                break;
            case SYSTEM_EVENT_STA_CONNECTED:
                ERA_LOG(TAG, ERA_PSTR("WiFi Connected"));
                break;
            case SYSTEM_EVENT_STA_GOT_IP:
                ERA_LOG(TAG, ERA_PSTR("WiFi Got IP"));
                break;
            case SYSTEM_EVENT_STA_DISCONNECTED:
                ERA_LOG_WARNING(TAG, ERA_PSTR("WiFi Disconnected"));
                break;
            case SYSTEM_EVENT_STA_STOP:
                ERA_LOG_WARNING(TAG, ERA_PSTR("WiFi Stopped"));
                break;
            case SYSTEM_EVENT_AP_START:
                ERA_LOG(TAG, ERA_PSTR("AP Started"));
                break;
            case SYSTEM_EVENT_AP_STOP:
                ERA_LOG_WARNING(TAG, ERA_PSTR("AP Stopped"));
                break;
            case SYSTEM_EVENT_ETH_START:
                ERA_LOG(TAG, ERA_PSTR("ETH Started"));
                break;
            case SYSTEM_EVENT_ETH_CONNECTED:
                ERA_LOG(TAG, ERA_PSTR("ETH Connected"));
                break;
            case SYSTEM_EVENT_ETH_GOT_IP:
                ERA_LOG(TAG, ERA_PSTR("ETH Got IP"));
                break;
            case SYSTEM_EVENT_ETH_DISCONNECTED:
                ERA_LOG_WARNING(TAG, ERA_PSTR("ETH Disconnected"));
                break;
            case SYSTEM_EVENT_ETH_STOP:
                ERA_LOG_WARNING(TAG, ERA_PSTR("ETH Stopped"));
                break;
            case SYSTEM_EVENT_AP_STACONNECTED:
                ERA_LOG(TAG, ERA_PSTR("AP STA Connected"));
                break;
            case SYSTEM_EVENT_AP_STADISCONNECTED:
                ERA_LOG_WARNING(TAG, ERA_PSTR("AP STA Disconnected"));
                break;
            case SYSTEM_EVENT_AP_STAIPASSIGNED:
                ERA_LOG(TAG, ERA_PSTR("AP STA IP Assigned"));
                break;
            default:
                break;
        }
    }
#else
    template <class Transport>
    void ERaEthernet<Transport>::onEthernetEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
        switch (event) {
            case ARDUINO_EVENT_WIFI_SCAN_DONE:
                ERA_LOG(TAG, ERA_PSTR("WiFi Scan done"));
                break;
            case ARDUINO_EVENT_WIFI_STA_START:
                ERA_LOG(TAG, ERA_PSTR("WiFi Started"));
                break;
            case ARDUINO_EVENT_WIFI_STA_CONNECTED:
                ERA_LOG(TAG, ERA_PSTR("WiFi Connected"));
                break;
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                ERA_LOG(TAG, ERA_PSTR("WiFi Got IP"));
                break;
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                ERA_LOG_WARNING(TAG, ERA_PSTR("WiFi Disconnected"));
                break;
            case ARDUINO_EVENT_WIFI_STA_STOP:
                ERA_LOG_WARNING(TAG, ERA_PSTR("WiFi Stopped"));
                break;
            case ARDUINO_EVENT_WIFI_AP_START:
                ERA_LOG(TAG, ERA_PSTR("AP Started"));
                break;
            case ARDUINO_EVENT_WIFI_AP_STOP:
                ERA_LOG_WARNING(TAG, ERA_PSTR("AP Stopped"));
                break;
            case ARDUINO_EVENT_ETH_START:
                ERA_LOG(TAG, ERA_PSTR("ETH Started"));
                this->setupHostname();
                break;
            case ARDUINO_EVENT_ETH_CONNECTED:
                ERA_LOG(TAG, ERA_PSTR("ETH Connected"));
                break;
            case ARDUINO_EVENT_ETH_GOT_IP:
                ERA_LOG(TAG, ERA_PSTR("ETH Got IP"));
                break;
#if (ESP_IDF_VERSION_MAJOR > 4)
            case ARDUINO_EVENT_ETH_LOST_IP:
                ERA_LOG_WARNING(TAG, ERA_PSTR("ETH Lost IP"));
                break;
#endif
            case ARDUINO_EVENT_ETH_DISCONNECTED:
                ERA_LOG_WARNING(TAG, ERA_PSTR("ETH Disconnected"));
                break;
            case ARDUINO_EVENT_ETH_STOP:
                ERA_LOG_WARNING(TAG, ERA_PSTR("ETH Stopped"));
                break;
            case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
                ERA_LOG(TAG, ERA_PSTR("AP STA Connected"));
                break;
            case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
                ERA_LOG_WARNING(TAG, ERA_PSTR("AP STA Disconnected"));
                break;
            case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
                ERA_LOG(TAG, ERA_PSTR("AP STA IP Assigned"));
                break;
#if (ESP_IDF_VERSION_MAJOR > 4)
            case ARDUINO_EVENT_PPP_START:
                ERA_LOG(TAG, ERA_PSTR("PPP Started"));
                break;
            case ARDUINO_EVENT_PPP_CONNECTED:
                ERA_LOG(TAG, ERA_PSTR("PPP Connected"));
                break;
            case ARDUINO_EVENT_PPP_GOT_IP:
                ERA_LOG(TAG, ERA_PSTR("PPP Got IP"));
                break;
            case ARDUINO_EVENT_PPP_LOST_IP:
                ERA_LOG_WARNING(TAG, ERA_PSTR("PPP Lost IP"));
                break;
            case ARDUINO_EVENT_PPP_DISCONNECTED:
                ERA_LOG_WARNING(TAG, ERA_PSTR("PPP Disconnected"));
                break;
            case ARDUINO_EVENT_PPP_STOP:
                ERA_LOG_WARNING(TAG, ERA_PSTR("PPP Stopped"));
                break;
#endif
            default:
                break;
        }
    }
#endif

#endif /* INC_ERA_WIFI_ESP32_HPP_ */
