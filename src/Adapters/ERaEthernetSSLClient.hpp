#ifndef INC_ERA_ETHERNET_SSL_CLIENT_HPP_
#define INC_ERA_ETHERNET_SSL_CLIENT_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "Ethernet"
#endif

#if !defined(ERA_ETHERNET_SPI)
    #define ERA_ETHERNET_SPI
#endif

#if !defined(ERA_AUTH_TOKEN)
    #define ERA_SCAN_DEVICE
    #pragma message "You did not define ERA_AUTH_TOKEN, switching to 'Scan Device' mode"

    #include <Adapters/ERaConnecting.hpp>
#endif

#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqtt.hpp>
#include <Time/ERaNTPTime.hpp>
#include <ArduinoBearSSL.h>

class ERaFlash;

template <class Transport>
class ERaEthernet
    : public ERaProto<Transport, ERaFlash>
#if defined(ERA_SCAN_DEVICE)
    , public ERaConnecting<EthernetServer, EthernetClient,
                           EthernetUDP, ERaFlash>
#endif
{
    enum EthernetModeT {
        ETH_MODE_DHCP = 0,
        ETH_MODE_STATIC_IP = 1,
        ETH_MODE_STATIC = 2
    };

    const char* TAG = "Ethernet";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;
#if defined(ERA_SCAN_DEVICE)
    typedef ERaConnecting<EthernetServer, EthernetClient,
                          EthernetUDP, ERaFlash> ConnectBase;
#endif

public:
    ERaEthernet(Transport& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
#if defined(ERA_SCAN_DEVICE)
        , ConnectBase(_flash)
#endif
        , client(tcpClient)
        , authToken(nullptr)
        , macUser(nullptr)
        , mode(EthernetModeT::ETH_MODE_DHCP)
        , _connected(false)
    {}
    ~ERaEthernet()
    {}

    static
    unsigned long getTime() {
        return ::getNTPTime<EthernetUDP>();
    }

    const uint8_t* getMACAddress(uint8_t* mac) {
        const uint8_t* ret = this->getMacAddress(this->authToken, this->macUser);
        if (mac == nullptr) {
            return ret;
        }

        memmove(mac, ret, 6);
        return ret;
    }

    ERaString getMACAddressString() {
        char mac[20] {0};
        uint8_t macAddr[6] {0};
        this->getMACAddress(macAddr);
        FormatString(mac, "%02X:%02X:%02X:%02X:%02X:%02X", macAddr[0], macAddr[1],
                                                        macAddr[2], macAddr[3],
                                                        macAddr[4], macAddr[5]);
        return ERaString(mac);
    }

    bool connectNetwork(const char* auth,
                        const uint8_t mac[] = nullptr) {
        ERaWatchdogFeed();

        this->setupConnectBase();

        ERA_LOG(TAG, ERA_PSTR("Connecting network..."));
        if (!Ethernet.begin(this->getMacAddress(auth, mac))) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Connect failed"));
            return false;
        }
        this->getTransp().setSSID(ERA_NETWORK_TYPE);

        ERaWatchdogFeed();

        ERaDelay(1000);
        this->getLocalIP();
        this->_connected = true;
        return true;
    }

    bool connectNetwork(const char* auth,
                        IPAddress& localIP,
                        const uint8_t mac[] = nullptr) {
        ERaWatchdogFeed();

        this->setupConnectBase();

        ERA_LOG(TAG, ERA_PSTR("Connecting network static IP..."));
        Ethernet.begin(this->getMacAddress(auth, mac), localIP);
        this->getTransp().setSSID(ERA_NETWORK_TYPE);

        ERaWatchdogFeed();

        ERaDelay(1000);
        this->getLocalIP();
        this->_connected = true;
        return true;
    }

    bool connectNetwork(const char* auth,
                        IPAddress& localIP,
                        IPAddress& dns,
                        IPAddress& gateway,
                        IPAddress& subnet,
                        const uint8_t mac[] = nullptr) {
        ERaWatchdogFeed();

        this->setupConnectBase();

        ERA_LOG(TAG, ERA_PSTR("Connecting network static IP..."));
        Ethernet.begin(this->getMacAddress(auth, mac),
                        localIP, dns, gateway, subnet);
        this->getTransp().setSSID(ERA_NETWORK_TYPE);

        ERaWatchdogFeed();

        ERaDelay(1000);
        this->getLocalIP();
        this->_connected = true;
        return true;
    }

    void config(const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::begin(auth);
        this->authToken = auth;
        ArduinoBearSSL.onGetTime(getTime);
        this->client.setInsecure(BearSSLClient::SNI::Insecure);
        this->getTransp().setClient(&this->client);
        this->getTransp().config(host, port, username, password);
    }

    void begin(const char* auth,
                const uint8_t mac[],
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectNetwork(auth, mac);
        this->mode = EthernetModeT::ETH_MODE_DHCP;
    }

    void begin(const char* auth,
                const uint8_t mac[] = nullptr,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, mac, host,
                    port, auth, auth);
    }

    void begin(const uint8_t mac[] = nullptr) {
        this->begin(ERA_AUTHENTICATION_TOKEN, mac,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

    void begin(const char* auth,
                IPAddress localIP,
                const uint8_t mac[],
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectNetwork(auth, localIP, mac);
        this->setStaticIP(localIP);
        this->mode = EthernetModeT::ETH_MODE_STATIC_IP;
    }

    void begin(const char* auth,
                IPAddress localIP,
                const uint8_t mac[] = nullptr,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, localIP, mac,
                    host, port, auth, auth);
    }

    void begin(IPAddress localIP,
                const uint8_t mac[] = nullptr) {
        this->begin(ERA_AUTHENTICATION_TOKEN, localIP, mac,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

    void begin(const char* auth,
                IPAddress localIP,
                IPAddress dns,
                IPAddress gateway,
                IPAddress subnet,
                const uint8_t mac[],
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectNetwork(auth, localIP, dns, gateway, subnet, mac);
        this->setStaticIP(localIP, dns, gateway, subnet);
        this->mode = EthernetModeT::ETH_MODE_STATIC;
    }

    void begin(const char* auth,
                IPAddress localIP,
                IPAddress dns,
                IPAddress gateway,
                IPAddress subnet,
                const uint8_t mac[] = nullptr,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, localIP, dns,
                    gateway, subnet, mac,
                    host, port, auth, auth);
    }

    void begin(IPAddress localIP,
                IPAddress dns,
                IPAddress gateway,
                IPAddress subnet,
                const uint8_t mac[] = nullptr) {
        this->begin(ERA_AUTHENTICATION_TOKEN, localIP,
                    dns, gateway, subnet, mac,
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
                if (this->netConnected() ||
                    this->reConnectNetwork()) {
                    ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                }
                break;
        }
    }

protected:
private:
#if defined(ERA_SCAN_DEVICE)
    void setupConnectBase() {
        ConnectBase::configLoad();
        ConnectBase::assignDeviceName(Base::getDeviceName());
        ConnectBase::assignDeviceType(Base::getDeviceType());
        ConnectBase::assignDeviceSecretKey(Base::getDeviceSecretKey());
        this->config(ERaConfig.token, ERaConfig.host, ERaConfig.port,
                     ERaConfig.username, ERaConfig.password);
    }
#else
    void setupConnectBase() {
    }
#endif

    bool netConnected() const {
        return this->_connected;
    }

    bool reConnectNetwork() {
        bool status {false};
        switch (this->mode) {
            case EthernetModeT::ETH_MODE_DHCP:
                status = this->connectNetwork(this->authToken, this->macUser);
                break;
            case EthernetModeT::ETH_MODE_STATIC_IP:
                status = this->connectNetwork(this->authToken, this->ipUser, this->macUser);
                break;
            case EthernetModeT::ETH_MODE_STATIC:
                status = this->connectNetwork(this->authToken, this->ipUser, this->dnsUser,
                                            this->gatewayUser, this->subnetUser, this->macUser);
                break;
            default:
                break;
        }
        return status;
    }

    void setStaticIP(IPAddress localIP,
                    IPAddress dns = (uint32_t)0x00000000,
                    IPAddress gateway = (uint32_t)0x00000000,
                    IPAddress subnet = (uint32_t)0x00000000) {
        this->ipUser = localIP;
        this->dnsUser = dns;
        this->gatewayUser = gateway;
        this->subnetUser = subnet;

#if defined(ERA_SCAN_DEVICE)
        ConnectBase::assignIPAddress(this->ipUser);
#endif
    }

    void getLocalIP() {
        IPAddress localIP = Ethernet.localIP();
        ERA_FORCE_UNUSED(localIP);
        ERA_LOG(TAG, ERA_PSTR("Connected to network"));
        ERA_LOG(TAG, ERA_PSTR("IP: %d.%d.%d.%d"), localIP[0], localIP[1],
                                                localIP[2], localIP[3]);

#if defined(ERA_SCAN_DEVICE)
        ConnectBase::assignIPAddress(localIP);
#endif
    }

    uint8_t* getMacAddress(const char* auth,
                        const uint8_t mac[]) {
        if (mac != nullptr) {
            this->macUser = (uint8_t*)mac;
            return this->macUser;
        }

        this->macAddress[0] = 0xFE;
        this->macAddress[1] = 0xED;
        this->macAddress[2] = 0x27;
        this->macAddress[3] = 0x06;
        this->macAddress[4] = 0xFE;
        this->macAddress[5] = 0xED;

        if (auth == nullptr) {
            return this->macAddress;
        }

        size_t index {1};
        size_t len = strlen(auth);
        for (size_t i = 0; i < len; ++i) {
            this->macAddress[index++] ^= auth[i];
            if (index > 5) {
                index = 1;
            }
        }

        ERA_LOG(TAG, ERA_PSTR("Get MAC: %02X-%02X-%02X-%02X-%02X-%02X"),
                            this->macAddress[0], this->macAddress[1],
                            this->macAddress[2], this->macAddress[3],
                            this->macAddress[4], this->macAddress[5]);
        return this->macAddress;
    }

    EthernetClient tcpClient;
    BearSSLClient client;

    const char* authToken;
    uint8_t macAddress[6];
    IPAddress ipUser;
    IPAddress dnsUser;
    IPAddress gatewayUser;
    IPAddress subnetUser;
    uint8_t* macUser;
    uint8_t mode;
    bool _connected;
};

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    char ip[20] {0};
    IPAddress localIP = Ethernet.localIP();
    FormatString(ip, "%d.%d.%d.%d", localIP[0], localIP[1],
                                    localIP[2], localIP[3]);
#if defined(ERA_SCAN_DEVICE)
    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_PNP));
#else
    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, 0);
#endif
    cJSON_AddStringToObject(root, INFO_NETWORK_PROTOCOL, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_SSID, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddNumberToObject(root, INFO_SIGNAL_STRENGTH, 100);
    cJSON_AddStringToObject(root, INFO_MAC, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ip);
    cJSON_AddNumberToObject(root, INFO_SSL, ERaInfoSSL());

    /* Override info */
    ERaInfo(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addSelfInfo(cJSON* root) {
#if defined(ESP32)
    cJSON_AddNumberToObject(root, SELF_CHIP_TEMPERATURE, static_cast<uint16_t>(temperatureRead() * 100.0f));
#else
    cJSON_AddNumberToObject(root, SELF_CHIP_TEMPERATURE, 5000);
#endif
    cJSON_AddNumberToObject(root, SELF_SIGNAL_STRENGTH, 100);

    /* Override self info */
    ERaSelfInfo(root);
}

#if defined(ERA_MODBUS)
    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
    #if defined(ESP32)
        cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, static_cast<uint16_t>(temperatureRead() * 100.0f));
    #else
        cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
    #endif
        cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
        cJSON_AddNumberToObject(root, INFO_MB_SIGNAL_STRENGTH, 100);
        cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ERA_NETWORK_TYPE);

        /* Override modbus info */
        ERaModbusInfo(root);
    }
#endif

#endif /* INC_ERA_ETHERNET_SSL_CLIENT_HPP_ */
