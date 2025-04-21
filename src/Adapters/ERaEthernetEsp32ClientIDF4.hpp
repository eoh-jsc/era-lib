#ifndef INC_ERA_ETHERNET_ESP32_CLIENT_IDF_4_HPP_
#define INC_ERA_ETHERNET_ESP32_CLIENT_IDF_4_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "Ethernet"
#endif

#if !defined(ERA_AUTH_TOKEN)
    #define ERA_SCAN_DEVICE
    #pragma message "You did not define ERA_AUTH_TOKEN, switching to 'Scan Device' mode"

    #include <Adapters/ERaConnecting.hpp>
#endif

#include <ETH.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ERa/ERaProtocol.hpp>
#include <ERa/ERaApiEsp32.hpp>
#include <MQTT/ERaMqtt.hpp>
#include <Modbus/ERaModbusEsp32.hpp>
#include <Zigbee/ERaZigbeeEsp32.hpp>
#include <Storage/ERaFlashEsp32.hpp>

template <class Transport>
class ERaEthernet
    : public ERaProto<Transport, ERaFlash>
#if defined(ERA_SCAN_DEVICE)
    , public ERaConnecting<WiFiServer, WiFiClient, WiFiUDP, ERaFlash>
#endif
{
    typedef std::function<void(WiFiEvent_t, WiFiEventInfo_t)> EthernetEventCallback_t;

    const char* TAG = "Ethernet";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;
#if defined(ERA_SCAN_DEVICE)
    typedef ERaConnecting<WiFiServer, WiFiClient, WiFiUDP, ERaFlash> ConnectBase;
#endif

public:
    ERaEthernet(Transport& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
#if defined(ERA_SCAN_DEVICE)
        , ConnectBase(_flash)
#endif
        , authToken(nullptr)
    {}
    ~ERaEthernet()
    {}

    void setEthernetCallbacks(EthernetEventCallback_t callback) {
        this->mEthernetCb = callback;
    }

    bool connectNetwork(uint8_t phyAddr = ETH_PHY_ADDR,
                        int power = ETH_PHY_POWER,
                        int mdc = ETH_PHY_MDC,
                        int mdio = ETH_PHY_MDIO,
                        eth_phy_type_t type = ETH_PHY_LAN8720,
                        eth_clock_mode_t clkMode = ETH_CLOCK_GPIO0_IN) {
        ERaWatchdogFeed();

        this->setupConnectBase();

        WiFi.onEvent(this->mEthernetCb);

        ERA_LOG(TAG, ERA_PSTR("Connecting network..."));
        if (!ETH.begin(phyAddr, power,
                       mdc, mdio, type, clkMode)){
            ERA_LOG_ERROR(TAG, ERA_PSTR("Connect failed"));
            return false;
        }
        this->getTransp().setSSID(ERA_NETWORK_TYPE);

        this->setupHostname();

        ERaWatchdogFeed();

        ERaDelay(1000);
        IPAddress localIP = ETH.localIP();
        ERA_FORCE_UNUSED(localIP);
        ERA_LOG(TAG, ERA_PSTR("Connected to network"));
        ERA_LOG(TAG, ERA_PSTR("IP: %s"), localIP.toString().c_str());

#if defined(ERA_SCAN_DEVICE)
        ConnectBase::assignIPAddress(localIP);
#endif
        return true;
    }

    void config(const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::begin(auth);
        this->getTransp().config(host, port, username, password);
    }

    void config(IPAddress localIP,
                IPAddress gateway,
                IPAddress subnet,
                IPAddress dns1 = (uint32_t)0x00000000,
                IPAddress dns2 = (uint32_t)0x00000000) {
        ETH.config(localIP, gateway, subnet, dns1, dns2);
    }

    void begin(const char* auth,
                uint8_t phyAddr,
                int power,
                int mdc,
                int mdio,
                eth_phy_type_t type,
                eth_clock_mode_t clkMode,
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectNetwork(phyAddr, power, mdc, mdio, type, clkMode);
    }

    void begin(const char* auth,
                uint8_t phyAddr = ETH_PHY_ADDR,
                int power = ETH_PHY_POWER,
                int mdc = ETH_PHY_MDC,
                int mdio = ETH_PHY_MDIO,
                eth_phy_type_t type = ETH_PHY_LAN8720,
                eth_clock_mode_t clkMode = ETH_CLOCK_GPIO0_IN,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, phyAddr, power,
                    mdc, mdio, type, clkMode,
                    host, port, auth, auth);
    }

    void begin(uint8_t phyAddr = ETH_PHY_ADDR,
                int power = ETH_PHY_POWER,
                int mdc = ETH_PHY_MDC,
                int mdio = ETH_PHY_MDIO,
                eth_phy_type_t type = ETH_PHY_LAN8720,
                eth_clock_mode_t clkMode = ETH_CLOCK_GPIO0_IN) {
        this->begin(ERA_AUTHENTICATION_TOKEN,
                    phyAddr, power,
                    mdc, mdio,
                    type, clkMode,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

    void run() {
        switch (ERaState::get()) {
#if defined(ERA_SCAN_DEVICE)
            case StateT::STATE_SCAN_DEVICE:
                ConnectBase::configMode();
                this->config(ERaConfig.token, ERaConfig.host, ERaConfig.port,
                             ERaConfig.username, ERaConfig.password);
                break;
#endif
            case StateT::STATE_CONNECTING_CLOUD:
                if (Base::connect()) {
#if defined(ERA_SCAN_DEVICE)
                    ConnectBase::setConnected(true);
                    ConnectBase::configSave();
#endif
                    ERaOptConnected(this);
                    ERaState::set(StateT::STATE_CONNECTED);
                }
                else {
                    ERaState::set(StateT::STATE_CONNECTING_NETWORK);
                }
                break;
            case StateT::STATE_CONNECTED:
                ERaState::set(StateT::STATE_RUNNING);
                break;
            case StateT::STATE_RUNNING:
                Base::run();
                break;
            case StateT::STATE_CONNECTING_NEW_NETWORK:
                Base::connectNewNetworkResult();
                break;
#if defined(ERA_SCAN_DEVICE)
            case StateT::STATE_RESET_CONFIG:
                ConnectBase::configReset();
                break;
#endif
            case StateT::STATE_RESET_CONFIG_REBOOT:
#if defined(ERA_SCAN_DEVICE)
                ConnectBase::configReset();
#endif
                ERaState::set(StateT::STATE_REBOOT);
                break;
            case StateT::STATE_REQUEST_LIST_WIFI:
                Base::responseListWiFi();
                break;
            case StateT::STATE_REBOOT:
                ERaDelay(1000);
                ERaRestart(false);
                break;
            default:
                ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                break;
        }
    }

protected:
private:
#if defined(ERA_SCAN_DEVICE)
    void setupConnectBase() {
        this->getDeviceName();
        ConnectBase::configLoad();
        ConnectBase::assignDeviceName(this->mDeviceName.c_str());
        ConnectBase::assignDeviceType(Base::getDeviceType());
        ConnectBase::assignDeviceSecretKey(Base::getDeviceSecretKey());
        this->config(ERaConfig.token, ERaConfig.host, ERaConfig.port,
                     ERaConfig.username, ERaConfig.password);
    }

    void setupHostname() {
#if defined(ERA_HOSTNAME_IDF)
        esp_netif_t* espNetif = esp_netif_get_handle_from_ifkey("ETH_DEF");
        esp_err_t err = esp_netif_set_hostname(espNetif, this->mDeviceName.c_str());
        if (err != ESP_OK) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Set hostname failed: %s"), esp_err_to_name(err));
        }
#else
        ETH.setHostname(this->mDeviceName.c_str());
#endif
    }

    const String& getDeviceName() {
        this->mDeviceName = Base::getDeviceName();
        this->mDeviceName += "-";
        this->mDeviceName += this->getChipID(4UL);
        return this->mDeviceName;
    }

    String getChipID(size_t len) {
        char id[20] {0};
        uint64_t unique {0};
        const uint64_t chipId = ESP.getEfuseMac();

        for (int i = 0; i < 41; i = i + 8) {
            unique |= ((chipId >> (40 - i)) & 0xff) << i;
        }
        FormatString(id, "%04x%08x", static_cast<uint16_t>(unique >> 32),
                                    static_cast<uint32_t>(unique));
        if (len) {
            return String(id + (strlen(id) - len));
        }
        return String(id);
    }
#else
    void setupConnectBase() {
    }

    void setupHostname() {
    }
#endif

    void onEthernetEvent(WiFiEvent_t event, WiFiEventInfo_t info);

    EthernetEventCallback_t mEthernetCb = [&, this](WiFiEvent_t event, WiFiEventInfo_t info) {
        this->onEthernetEvent(event, info);
    };

    const char* authToken;
    String mDeviceName;
};

#include <Adapters/ERaEthernetEsp32.hpp>

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
#if defined(ERA_SCAN_DEVICE)
    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_PNP));
#else
    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, 0);
#endif
    cJSON_AddStringToObject(root, INFO_NETWORK_PROTOCOL, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_SSID, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, ETH.linkSpeed());
    cJSON_AddNumberToObject(root, INFO_SIGNAL_STRENGTH, 100);
    cJSON_AddStringToObject(root, INFO_MAC, ETH.macAddress().c_str());
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ETH.localIP().toString().c_str());
    cJSON_AddNumberToObject(root, INFO_SSL, ERaInfoSSL());

    /* Override info */
    ERaInfo(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addSelfInfo(cJSON* root) {
    cJSON_AddNumberToObject(root, SELF_CHIP_TEMPERATURE, static_cast<uint16_t>(temperatureRead() * 100.0f));
    cJSON_AddNumberToObject(root, SELF_SIGNAL_STRENGTH, 100);

    /* Override self info */
    ERaSelfInfo(root);
}

#if defined(ERA_MODBUS)
    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
        cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, static_cast<uint16_t>(temperatureRead() * 100.0f));
        cJSON_AddNumberToObject(root, INFO_MB_RSSI, ETH.linkSpeed());
        cJSON_AddNumberToObject(root, INFO_MB_SIGNAL_STRENGTH, 100);
        cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ERA_NETWORK_TYPE);

        /* Override modbus info */
        ERaModbusInfo(root);
    }
#endif

#endif /* INC_ERA_ETHERNET_ESP32_CLIENT_IDF_4_HPP_ */
