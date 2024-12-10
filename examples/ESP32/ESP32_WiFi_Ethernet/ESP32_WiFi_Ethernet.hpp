#ifndef INC_ERA_WIFI_ETHERNET_HPP_
#define INC_ERA_WIFI_ETHERNET_HPP_

#ifndef ETHERNET_PHY_ADDR
    #define ETHERNET_PHY_ADDR   0
#endif

#ifndef ETHERNET_PHY_TYPE
    #define ETHERNET_PHY_TYPE   ETH_PHY_LAN8720
#endif

#ifndef ETHERNET_PHY_POWER
    #define ETHERNET_PHY_POWER  -1
#endif

#ifndef ETHERNET_PHY_MDC
    #define ETHERNET_PHY_MDC    23
#endif

#ifndef ETHERNET_PHY_MDIO
    #define ETHERNET_PHY_MDIO   18
#endif

#ifndef ETHERNET_CLK_MODE
    #define ETHERNET_CLK_MODE   ETH_CLOCK_GPIO0_IN
#endif

#include <ETH.h>

static volatile bool ethConnected = false;

void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            ERA_LOG("Event", ERA_PSTR("WiFi Scan done"));
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            ERA_LOG("Event", ERA_PSTR("WiFi Started"));
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            ERA_LOG("Event", ERA_PSTR("WiFi Connected"));
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            ERA_LOG("Event", ERA_PSTR("WiFi Got IP: %s, MAC: %s"),
                                    WiFi.localIP().toString().c_str(),
                                    WiFi.macAddress().c_str());
            mqtt.setSSID(ERaConfig.ssid);
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            ERA_LOG_WARNING("Event", ERA_PSTR("WiFi Disconnected"));
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            ERA_LOG_WARNING("Event", ERA_PSTR("WiFi Stopped"));
            break;
        case ARDUINO_EVENT_ETH_START:
            ERA_LOG("Event", ERA_PSTR("ETH Started"));
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            ERA_LOG("Event", ERA_PSTR("ETH Connected"));
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            ERA_LOG("Event", ERA_PSTR("ETH Got IP: %s, MAC: %s, Speed: %dMbps"),
                                    ETH.localIP().toString().c_str(),
                                    ETH.macAddress().c_str(),
                                    ETH.linkSpeed());
            WiFi.disconnect();
            mqtt.setSSID("Ethernet");
            ERaState::set(StateT::STATE_CONNECTING_CLOUD);
            ethConnected = true;
            break;
#if (ESP_IDF_VERSION_MAJOR > 4)
        case ARDUINO_EVENT_ETH_LOST_IP:
            ERA_LOG("Event", ERA_PSTR("ETH Lost IP"));
            ethConnected = false;
            break;
#endif
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            ERA_LOG_WARNING("Event", ERA_PSTR("ETH Disconnected"));
            ethConnected = false;
            break;
        case ARDUINO_EVENT_ETH_STOP:
            ERA_LOG_WARNING("Event", ERA_PSTR("ETH Stopped"));
            ethConnected = false;
            break;
        default:
            break;
    }
}

#if (ESP_IDF_VERSION_MAJOR > 4)
    bool ethConnect() {
        static bool initialized = false;
        if (!initialized) {
            initialized = true;
            ETH.begin(ETHERNET_PHY_TYPE, ETHERNET_PHY_ADDR, ETHERNET_PHY_MDC,
                      ETHERNET_PHY_MDIO, ETHERNET_PHY_POWER, ETHERNET_CLK_MODE);
            ERA_LOG("ETH", ERA_PSTR("Connecting to ETH"));
            ERaDelay(1000);
        }
        return ethConnected;
    }
#else
    bool ethConnect() {
        static bool initialized = false;
        if (!initialized) {
            initialized = true;
            ETH.begin(ETHERNET_PHY_ADDR, ETHERNET_PHY_POWER, ETHERNET_PHY_MDC,
                      ETHERNET_PHY_MDIO, ETHERNET_PHY_TYPE, ETHERNET_CLK_MODE);
            ERA_LOG("ETH", ERA_PSTR("Connecting to ETH"));
            ERaDelay(1000);
        }
        return ethConnected;
    }
#endif

ERA_INFO() {
    if (!ethConnected) {
        return;
    }

    ERaSetString(root, INFO_NETWORK_PROTOCOL, "Ethernet");
    ERaSetString(root, INFO_SSID, "Ethernet");
    ERaSetNumber(root, INFO_RSSI, 100);
    ERaSetNumber(root, INFO_SIGNAL_STRENGTH, 100);
    ERaSetString(root, INFO_MAC, ETH.macAddress().c_str());
    ERaSetString(root, INFO_LOCAL_IP, ETH.localIP().toString().c_str());
}

ERA_MODBUS_INFO() {
    if (!ethConnected) {
        return;
    }

    ERaSetNumber(root, INFO_MB_RSSI, 100);
    ERaSetNumber(root, INFO_MB_SIGNAL_STRENGTH, 100);
    ERaSetString(root, INFO_MB_WIFI_USING, "Ethernet");
}

#endif /* INC_ERA_WIFI_ETHERNET_HPP_ */
