#ifndef INC_ERA_PNP_ARDUINO_HPP_
#define INC_ERA_PNP_ARDUINO_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "WiFi"
#endif

#include <TinyGsmClient.hpp>

#if defined(__has_include)
    #if __has_include(<ArduinoMDNS.h>)
        #include <ArduinoMDNS.h>
        #define ERA_HAS_ARDUINO_MDNS_H
    #endif
#endif

#if defined(ARDUINO_ARCH_STM32)
    #include <ERa/ERaApiStm32Def.hpp>
    #include <ERa/ERaProtocol.hpp>
    #include <ERa/ERaApiStm32.hpp>
    #include <MQTT/ERaMqtt.hpp>
    #include <Modbus/ERaModbusStm32.hpp>
    #include <Zigbee/ERaZigbeeStm32.hpp>
    #include <Storage/ERaFlashStm32.hpp>
#elif defined(__MBED__)
    #include <ERa/ERaApiMbedDef.hpp>
    #include <ERa/ERaProtocol.hpp>
    #include <ERa/ERaApiMbed.hpp>
    #include <MQTT/ERaMqtt.hpp>
    #include <Modbus/ERaModbusArduino.hpp>
    #include <Zigbee/ERaZigbeeArduino.hpp>
    #include <Storage/ERaFlashMbed.hpp>
#else
    #include <ERa/ERaApiArduinoDef.hpp>
    #include <ERa/ERaProtocol.hpp>
    #include <ERa/ERaApiArduino.hpp>
    #include <MQTT/ERaMqtt.hpp>
    #include <Modbus/ERaModbusArduino.hpp>
    #include <Zigbee/ERaZigbeeArduino.hpp>
    #include <Storage/ERaFlashConfig.hpp>
#endif

#include <PnP/ERaConnectedHandler.hpp>
#include <PnP/ERaCaptiveIndex.hpp>
#include <PnP/ERaWeb.hpp>

#define MDNS_PORT                     80

#if defined(ERA_HAS_ARDUINO_MDNS_H)
    #define MDNS_SERVICE              "_plugandplay"
    #define MDNS_PROTOCOL             MDNSServiceTCP
#endif

#define WIFI_SCAN_MAX                 15
#define WIFI_SCAN_TIMEOUT             20000
#define WIFI_NET_CONNECT_TIMEOUT      60000
#define WIFI_CLOUD_CONNECT_TIMEOUT    60000
#define WIFI_NET_CHECK_TIMEOUT        (WIFI_NET_CONNECT_TIMEOUT * 3)
#define WIFI_AP_PASS                  "Eoh@2021"
#define WIFI_AP_IP                    IPAddress(192, 168, 27, 1)
#define WIFI_AP_Subnet                IPAddress(255, 255, 255, 0)

typedef struct __WiFiConfig_t {
    uint8_t flag;

    char ssid[64];
    char pass[64];
} ERA_ATTR_PACKED WiFiConfig_t;

typedef struct __ERaConfig_t {
    uint32_t magic;
    uint8_t flags;

    char ssid[64];
    char pass[64];
    char backupSSID[64];
    char backupPass[64];
    bool hasBackup;

    char token[64];
    char host[64];
    uint16_t port;
    char username[64];
    char password[64];

    bool connected;
    bool forceSave;

    void setFlag(uint8_t mask, bool value) {
        if (value) {
            flags |= mask;
        }
        else {
            flags &= ~mask;
        }
    }

    bool getFlag(uint8_t mask) {
        return (flags & mask) == mask;
    }
} ERA_ATTR_PACKED ERaConfig_t;

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    #if defined(ERA_USE_SSL)
        static TinyGsmClientSecure  tinyClient;
        static ERaConfig_t          ERaConfig {};
    #else
        static TinyGsmClient tinyClient;
        static ERaConfig_t   ERaConfig {};
    #endif
#else
    #if defined(ERA_USE_SSL)
        #define tinyClient   StaticHelper<TinyGsmClientSecure>::instance()
        #define ERaConfig    StaticHelper<ERaConfig_t>::instance()
    #else
        #define tinyClient   StaticHelper<TinyGsmClient>::instance()
        #define ERaConfig    StaticHelper<ERaConfig_t>::instance()
    #endif
#endif

#if !defined(INADDR_ANY)
    #define INADDR_ANY  (uint32_t)0x00000000
#endif

static const ERaConfig_t ERaDefault = {
    0x27061995,
    0x00,

    "",
    "",
    "",
    "",
    false,

    "invalid",
    ERA_MQTT_HOST,
    ERA_MQTT_PORT,
    ERA_MQTT_USERNAME,
    ERA_MQTT_PASSWORD,

    false,
    false
};

enum WiFiFlagT {
    WIFI_FLAG_INVALID = 0x00,
    WIFI_FLAG_CONNECTED = 0x01,
    WIFI_FLAG_CURRENT = 0x02,
    WIFI_FLAG_FAILED = 0x04
};

enum ConfigFlagT {
    CONFIG_FLAG_VALID = 0x01,
    CONFIG_FLAG_STORE = 0x02,
    CONFIG_FLAG_API = 0x04,
    CONFIG_FLAG_UDP = 0x08,
    CONFIG_FLAG_PNP = 0x10
};

static inline
TinyGsm* getTinyModem() {
    return tinyClient.getModem();
}

static inline
int16_t getRSSI() {
    ERaWatchdogFeed();
    TinyGsm* modem = getTinyModem();
    if (modem == nullptr) {
        return 0;
    }
    return modem->getSignalQuality();
}

static inline
String getSSID() {
    ERaWatchdogFeed();
    TinyGsm* modem = getTinyModem();
    if (modem == nullptr) {
        return String(ERA_NETWORK_TYPE);
    }
    return modem->SSID();
}

static inline
String getSSID(uint8_t networkItem) {
    ERaWatchdogFeed();
    TinyGsm* modem = getTinyModem();
    if (modem == nullptr) {
        return String(ERA_NETWORK_TYPE);
    }
    return modem->SSID(networkItem);
}

static inline
String getBSSID() {
    ERaWatchdogFeed();
    TinyGsm* modem = getTinyModem();
    if (modem == nullptr) {
        return String(ERA_NETWORK_TYPE);
    }
    return modem->BSSID();
}

static inline
String getMAC() {
    ERaWatchdogFeed();
    TinyGsm* modem = getTinyModem();
    if (modem == nullptr) {
        return String(ERA_NETWORK_TYPE);
    }
    return modem->BSSID();
}

static inline
String getLocalIP() {
    ERaWatchdogFeed();
    TinyGsm* modem = getTinyModem();
    if (modem == nullptr) {
        return String(ERA_NETWORK_TYPE);
    }
    return TinyGsmClient::TinyGsmStringFromIp(modem->localIP());
}

static inline
String getSoftAPIP() {
    ERaWatchdogFeed();
    TinyGsm* modem = getTinyModem();
    if (modem == nullptr) {
        return String(ERA_NETWORK_TYPE);
    }
    return TinyGsmClient::TinyGsmStringFromIp(modem->localIPAP());
}

static inline
bool getConnectionStatus() {
    ERaWatchdogFeed();
    TinyGsm* modem = getTinyModem();
    if (modem == nullptr) {
        return false;
    }
    return modem->isNetworkConnected();
}

#include <Network/ERaUdpTiny.hpp>

typedef void (*IdentifyCallback_t)(void);

#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<bool(void)> NetworkCallback_t;
#else
    typedef bool (*NetworkCallback_t)(void);
#endif

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static TinyGsmUdp Udp;
    static ERaUdp<TinyGsmUdp> udpERa(Udp);
    static IdentifyCallback_t IdentifyCallbacks;
#else
    #define Udp     StaticHelper<TinyGsmUdp>::instance()
    #define udpERa  StaticRefHelper<ERaUdp<TinyGsmUdp>, TinyGsmUdp>::instance(Udp)
    #define IdentifyCallbacks StaticHelper<IdentifyCallback_t>::instance()
#endif

template <class Transport>
class ERaPnP
    : public ERaProto<Transport, ERaFlash>
{
    enum PnPRequestT {
        PNP_REQ_CAPTIVE = 0,
        PNP_REQ_REDIRECT_CAPTIVE = 1,
        PNP_REQ_BOARD_INFO = 2,
        PNP_REQ_SCAN_WIFI = 3,
        PNP_REQ_CONFIG = 4,
        PNP_REQ_RESET = 5,
        PNP_REQ_REBOOT = 6,
        PNP_REQ_IDENTIFY = 7,
        PNP_REQ_MAX = 255,
    };

    enum WiFiStateT {
        WIFI_STATE_IDLE = 0x00,
        WIFI_STATE_STARTED = 0x01,
        WIFI_STATE_CONNECTING = 0x02,
        WIFI_STATE_CONNECTED = 0x03,
        WIFI_STATE_RETRYING = 0x04,
        WIFI_STATE_FAILED = 0x05,
    };

    const char* TAG = "WiFi";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;

public:
    ERaPnP(Transport& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , modem(nullptr)
        , authToken(nullptr)
        , persistent(false)
        , resetPin(-1)
        , invertPin(false)
        , softRestart(false)
        , newWiFi {}
        , wifiState(WiFiStateT::WIFI_STATE_IDLE)
#if defined(ERA_HAS_ARDUINO_MDNS_H)
        , mdns(mdnsUDP)
#endif
        , prevMillisSignal(0UL)
    {
        IdentifyCallbacks = this->functionCallbackDummy;
        this->getTransp().setClient(&tinyClient);

        ConnectedHandler().setTransport(_transp);
        ConnectedHandler().setDefaultSSID(ERaConfig.ssid);
    }
    ~ERaPnP()
    {}

    void restartOption(bool soft) {
        this->softRestart = soft;
    }

    void connectWiFiPersistent(const char* ssid, const char* pass) {
        this->configLoad();
        if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
            return;
        }
        this->connectNetwork(ssid, pass);
    }

    void config(TinyGsm& gsm,
                const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::begin(auth);
        this->modem = &gsm;
        Udp.init(this->modem, 2);
        tinyClient.init(this->modem, 1);
        this->server.init(this->modem, 0);
#if defined(ERA_HAS_ARDUINO_MDNS_H)
        this->mdnsUDP.init(this->modem, 3);
#endif
        this->getTransp().setClient(&tinyClient);
        this->getTransp().config(host, port, username, password);
    }

    void begin(const char* auth,
                TinyGsm& gsm,
                const char* ssid,
                const char* pass,
                int rstPin,
                bool invert,
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(gsm, auth, host, port, username, password);
        this->setResetPin(rstPin, invert);
        this->restart();
        ERaWatchdogFeed();

        if (this->persistent) {
            this->configLoad();
        }
        if (!ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
            CopyToArray(ssid, ERaConfig.ssid);
            CopyToArray(pass, ERaConfig.pass);
            CopyToArray(auth, ERaConfig.token);
            CopyToArray(host, ERaConfig.host);
            ERaConfig.port = port;
            CopyToArray(username, ERaConfig.username);
            CopyToArray(password, ERaConfig.password);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
        }
        else {
            this->config(gsm, ERaConfig.token, ERaConfig.host, ERaConfig.port,
                         ERaConfig.username, ERaConfig.password);
        }

        this->connectNetwork(ERaConfig.ssid, ERaConfig.pass);
        if (this->netConnected(false)) {
            if (Base::connect()) {
                ERaOptConnected(this);
                ERaState::set(StateT::STATE_CONNECTED);
            }
            else {
                ERaState::set(StateT::STATE_CONNECTING_CLOUD);
            }
        }
        else if (Base::isConfigMode()) {
        }
        else {
            this->configInit();
        }
    }

    void begin(const char* auth,
                TinyGsm& gsm,
                const char* ssid,
                const char* pass,
                int rstPin = -1,
                bool invert = false,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, gsm, ssid,
                    pass, rstPin, invert,
                    host, port, auth, auth);
    }

    void begin(TinyGsm& gsm,
                const char* ssid,
                const char* pass,
                int rstPin = -1,
                bool invert = false) {
        this->begin(ERA_AUTHENTICATION_TOKEN, gsm, ssid, pass,
                    rstPin, invert, ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

    void setNetworkCallbacks(NetworkCallback_t callback, Client* client = nullptr,
                             const char* ssid = nullptr) {
        ConnectedHandler().setNetworkCallbacks(callback, client, ssid);
    }

    void setNetworkCallbacks(NetworkCallback_t callback, Client& client,
                             const char* ssid = nullptr) {
        ConnectedHandler().setNetworkCallbacks(callback, client, ssid);
    }

    void setGsmCallbacks(NetworkCallback_t callback, Client* client = nullptr,
                         const char* ssid = nullptr) {
        ConnectedHandler().setGsmCallbacks(callback, client, ssid);
    }

    void setGsmCallbacks(NetworkCallback_t callback, Client& client,
                         const char* ssid = nullptr) {
        ConnectedHandler().setGsmCallbacks(callback, client, ssid);
    }

    void addBeforeWiFiCallbacks(NetworkCallback_t callback, Client* client = nullptr,
                                const char* ssid = nullptr) {
        ConnectedHandler().addBeforeWiFiCallbacks(callback, client, ssid);
    }

    void addBeforeWiFiCallbacks(NetworkCallback_t callback, Client& client,
                                const char* ssid = nullptr) {
        ConnectedHandler().addBeforeWiFiCallbacks(callback, client, ssid);
    }

    void addAfterWiFiCallbacks(NetworkCallback_t callback, Client* client = nullptr,
                               const char* ssid = nullptr) {
        ConnectedHandler().addAfterWiFiCallbacks(callback, client, ssid);
    }

    void addAfterWiFiCallbacks(NetworkCallback_t callback, Client& client,
                               const char* ssid = nullptr) {
        ConnectedHandler().addAfterWiFiCallbacks(callback, client, ssid);
    }

    void setIdentifyCallbacks(IdentifyCallback_t callback) {
        IdentifyCallbacks = callback;
    }

    void setPersistent(bool enable) {
        this->persistent = enable;
    }

    void switchToConfig(bool erase = false, bool execute = false) {
        ERaConfig.connected = false;
        ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
        if (!erase) {
            ERaState::set(StateT::STATE_SWITCH_TO_AP);
        }
        else if (execute) {
            this->configReset();
        }
        else {
            ERaState::set(StateT::STATE_RESET_CONFIG);
        }
        ERA_LOG(TAG, ERA_PSTR("Switch to config mode!"));
    }

    void begin(TinyGsm& gsm,
            int rstPin = -1,
            bool invert = false,
            const char* auth = nullptr);
    void run();

protected:
private:
    int beginModem();
    void configApi();
    void configInit();
    void configLoad();
    void configLoadDefault();
    void configSave();
    void configReset();
    void configBegin();
    void configMode();
    void waitHandle(String& networks, unsigned long ms);
    void runServer(String& networks);
    String scanNetworks();
    cJSON* scanNetworksJson(int nets, bool compact = false);
    void addEncryptionType(uint8_t networkItem, cJSON* const item);
    String urlDecode(const String& text);
    String findArg(const String& url, const String& body, const String& arg);
    String urlFindArg(const String& url, const String& arg);
    String bodyFindArg(const String& body, const String& arg);
    void reFlagNewNetwork(const char* ssid);
    void copyNewNetwork();
    void connectNetwork();
    void connectNewNetwork();
    void connectNewNetworkResult() override;
    void connectCloud();
    bool connectNetwork(const char* ssid, const char* pass, bool nonBlocking = false);
    bool connectWiFi(const char* ssid, const char* pass, bool nonBlocking = false);
    void connectNewWiFi(const char* ssid, const char* pass) override;
    void connectWiFiBackup();
    void checkConnectWiFi(const char* ssid);
    void requestListWiFi() override;
    void responseListWiFi() override;
    void switchToAP();
    void switchToSTA();
    void switchToAPSTA();
    void getSignalQuality();

    String getDeviceName();

#if defined(ERA_HAS_ARDUINO_MDNS_H)
    String getMdnsService();
    String getTxtDeviceName();
    String getTxtDeviceType();
    String getTxtDeviceSecretKey();
    String getTxtRecords();
#endif

    String getChipID(size_t len = 0UL);
    void handleScanDevice();

    template <int size>
    void getWiFiName(char(&ptr)[size], bool withPrefix = true, size_t len = 0UL);
    template <int size>
    void getImeiChip(char(&ptr)[size]);
    void replace(char* buf, char src, char dst);

    static inline
    bool wifiConnected(bool skipState = false) {
        static bool isConnected {false};
        static unsigned long prevMillis {0UL};

        unsigned long currentMillis = ERaMillis();
        if ((currentMillis - prevMillis) < 500UL) {
            return isConnected;
        }
        unsigned long skipTimes = ((currentMillis - prevMillis) / 500UL);
        // update time
        prevMillis += (500UL * skipTimes);

        isConnected = ::getConnectionStatus();
        if (isConnected) {
            ConnectedHandler().switchClient(nullptr, skipState);
            return true;
        }
        return false;
    }

    static inline
    bool priorityConnected(bool skipState = false) {
        return (ConnectedHandler().beforeWiFiConnected(skipState) ||
                ERaPnP::wifiConnected(skipState));
    }

    static inline
    bool netConnected(bool skipState = false) {
        return (ERaPnP::priorityConnected(skipState) ||
                ConnectedHandler().afterWiFiConnected(skipState));
    }

    void setResetPin(int pin, bool invert) {
        if (pin < 0) {
            return;
        }
        this->resetPin = pin;
        this->invertPin = invert;
        pinMode(this->resetPin, OUTPUT);
    }

    void restart() {
        if (this->resetPin < 0) {
            this->softwareReset();
            return;
        }

        ERaWatchdogFeed();

        this->powerOff();
        ERaDelay(2000);
        this->powerOn();
        ERaDelay(500);

        ERaWatchdogFeed();

        this->softwareReset();
    }

    void softwareReset() {
        if (!this->softRestart) {
            return;
        }
        this->modem->restart();
    }

    void powerOn() {
        if (this->resetPin < 0) {
            return;
        }
        ::digitalWrite(this->resetPin, !this->invertPin ? HIGH : LOW);
    }

    void powerOff() {
        if (this->resetPin < 0) {
            return;
        }
        ::digitalWrite(this->resetPin, !this->invertPin ? LOW : HIGH);
    }

    static inline
    void functionCallbackDummy() {
    }

    TinyGsm* modem;
    TinyGsmServer server;
    const char* authToken;
    bool persistent;
    int resetPin;
    bool invertPin;
    bool softRestart;
    WiFiConfig_t newWiFi;

    String       networks;
    WiFiStateT   wifiState;

#if defined(ERA_HAS_ARDUINO_MDNS_H)
    MDNS mdns;
    TinyGsmUdp mdnsUDP;
#endif

    unsigned long prevMillisSignal;

    ERA_SINGLETON(ERaConnectedHandler<Transport>, ConnectedHandler)
};

template <class Transport>
void ERaPnP<Transport>::begin(TinyGsm& gsm,
                            int rstPin,
                            bool invert,
                            const char* auth) {
    this->authToken = auth;
    this->modem = &gsm;
    Udp.init(this->modem, 2);
    tinyClient.init(this->modem, 1);
    this->server.init(this->modem, 0);
#if defined(ERA_HAS_ARDUINO_MDNS_H)
    this->mdnsUDP.init(this->modem, 3);
#endif
    this->getTransp().setClient(&tinyClient);
    this->setResetPin(rstPin, invert);

    // Test modem
    this->restart();
    ERaWatchdogFeed();

    MillisTime_t startMillis = ERaMillis();
    while (!this->beginModem()) {
        ERaDelay(500);
        if (!ERaRemainingTime(startMillis, WIFI_NET_CONNECT_TIMEOUT)) {
            break;
        }
    };

    Base::init();
    this->configInit();
}

template <class Transport>
void ERaPnP<Transport>::run() {
    switch (ERaState::get()) {
        case StateT::STATE_WAIT_CONFIG:
        case StateT::STATE_CONFIGURING:
        case StateT::STATE_SCAN_DEVICE:
            this->configMode();
            break;
        case StateT::STATE_SWITCH_TO_AP:
            this->switchToAP();
            break;
        case StateT::STATE_SWITCH_TO_STA:
            this->switchToSTA();
            break;
        case StateT::STATE_SWITCH_TO_AP_STA:
            this->switchToAPSTA();
            break;
        case StateT::STATE_CONNECTING_NETWORK:
            if (this->priorityConnected(false)) {
                ERaState::set(StateT::STATE_CONNECTING_CLOUD);
            }
            else {
                this->connectNetwork();
            }
            break;
        case StateT::STATE_CONNECTING_NEW_NETWORK:
            this->connectNewNetwork();
            break;
        case StateT::STATE_CONNECTING_CLOUD:
            this->connectCloud();
            break;
        case StateT::STATE_CONNECTED:
            ERaState::set(StateT::STATE_RUNNING);
            break;
        case StateT::STATE_RUNNING:
            if (!this->netConnected(false)) {
                ERaWatchdogFeed();

                Base::onDisconnected();

                ERaWatchdogFeed();

                ERaState::set(StateT::STATE_DISCONNECTED);
            }
            else if (!ERaState::is(StateT::STATE_RUNNING)) {
            }
            else {
                Base::run();

                this->getSignalQuality();
            }
            break;
        case StateT::STATE_DISCONNECTED:
            ERaState::set(StateT::STATE_CONNECTING_NETWORK);
            break;
        case StateT::STATE_OTA_UPGRADE:
            ERaState::set(StateT::STATE_RUNNING);
            break;
        case StateT::STATE_RESET_CONFIG:
            this->configReset();
            break;
        case StateT::STATE_RESET_CONFIG_REBOOT:
            this->configReset();
            ERaState::set(StateT::STATE_REBOOT);
            break;
        case StateT::STATE_REQUEST_LIST_WIFI:
            this->responseListWiFi();
            ERaState::set(StateT::STATE_RUNNING);
            break;
        case StateT::STATE_REBOOT:
            ERaDelay(1000);
            ERaRestart(false);
            break;
        default:
            break;
    }
}

template <class Transport>
int ERaPnP<Transport>::beginModem() {
    ERaWatchdogFeed();

    ERA_LOG(TAG, ERA_PSTR("Test modem"));
    if (!this->modem->testAT()) {
        ERaWatchdogFeed();
        this->restart();
        ERaWatchdogFeed();
        ERA_LOG(TAG, ERA_PSTR("Test modem failed. Restarting modem..."));
        return 0;
    }

    ERaWatchdogFeed();

    ERA_LOG(TAG, ERA_PSTR("Modem init"));
    if (!this->modem->begin()) {
        ERA_LOG(TAG, ERA_PSTR("Can't init modem"));
        return 1;
    }

    ERaWatchdogFeed();

    this->modem->persistent(false);

    ERaWatchdogFeed();

    this->modem->networkDisconnect();

    ERaWatchdogFeed();
    return 2;
}

template <class Transport>
void ERaPnP<Transport>::configApi() {
    static bool configured {false};
    if (!ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_API) ||
        configured) {
        return;
    }
    configured = true;
    ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, false);

    ERA_LOG(TAG, ERA_PSTR("Config api"));

    static char imei[64] {0};
    static char ssidAP[64] {0};
    this->getWiFiName(ssidAP);
    this->getImeiChip(imei);

    udpERa.on("connect/wifi", []() {
        ERA_LOG(ERA_PSTR("Config"), ERA_PSTR("Configuration..."));
        String ssid = udpERa.arg("ssid").c_str();
        String pass = udpERa.arg("pass").c_str();
        String backupSsid = udpERa.arg("backup_ssid").c_str();
        String backupPass = udpERa.arg("backup_pass").c_str();

        String token = udpERa.arg("token").c_str();
        String host = udpERa.arg("host").c_str();
        String port = udpERa.arg("port").c_str();
        String username = udpERa.arg("username").c_str();
        String password = udpERa.arg("password").c_str();

        String content;

        if (udpERa.hasArg("token") && !token.length()) {
            content = ERA_F(R"json({"status":"error","message":"Invalid token"})json");
            udpERa.sendAndStop(content.c_str());
            return;
        }

        ERaConfig.forceSave = !!udpERa.arg("save").toInt();

        if (ssid.length()) {
            CopyToArray(ssid, ERaConfig.ssid);
            CopyToArray(pass, ERaConfig.pass);
        }

        if (backupSsid.length()) {
            ERaConfig.hasBackup = true;
            CopyToArray(backupSsid, ERaConfig.backupSSID);
            CopyToArray(backupPass, ERaConfig.backupPass);
        }
        else {
            ERaConfig.hasBackup = false;
        }

        if (token.length()) {
            CopyToArray(token, ERaConfig.token);
        }
        if (host.length()) {
            CopyToArray(host, ERaConfig.host);
        }
        if (port.length()) {
            ERaConfig.port = port.toInt();
        }
        if (username.length()) {
            CopyToArray(username, ERaConfig.username);
        }
        else {
            CopyToArray(ERaConfig.token, ERaConfig.username);
        }
        if (password.length()) {
            CopyToArray(password, ERaConfig.password);
        }
        else {
            CopyToArray(ERaConfig.token, ERaConfig.password);
        }

        if (ERaConfig.forceSave) {
            content = ERA_F(R"json({"status":"ok","message":"Configuration saved"})json");
        }
        else {
            content = ERA_F(R"json({"status":"ok","message":"Connecting wifi..."})json");
        }
        ERA_FORCE_UNUSED(content);

        if (ssid.length() || ERaConfig.forceSave ||
            ERaPnP::priorityConnected(true)) {
            // udpERa.send(content.c_str());
            ERaDelay(100);
            /* TODO: send first time */
            udpERa.sendBoardInfo();
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_UDP, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_PNP, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
            if (ERaPnP::priorityConnected(true)) {
                ::getTinyModem()->endAP();
                ERaWatchdogFeed();
                ERaState::set(StateT::STATE_CONNECTING_CLOUD);
            }
            else {
                ERaState::set(StateT::STATE_SWITCH_TO_AP_STA);
            }
        }
    });

    udpERa.on("connect/token", []() {
        String token = udpERa.arg("token").c_str();

        if (!token.length()) {
            String content = ERA_F(R"json({"status":"error","message":"Invalid token"})json");
            udpERa.sendAndStop(content.c_str());
            return;
        }

        if (token.length()) {
            CopyToArray(token, ERaConfig.token);
            CopyToArray(ERaConfig.token, ERaConfig.username);
            CopyToArray(ERaConfig.token, ERaConfig.password);
        }
    });

    udpERa.on("connect/host", []() {
        String host = udpERa.arg("host").c_str();

        if (host.length()) {
            CopyToArray(host, ERaConfig.host);
        }
    });

    udpERa.on("scan/wifi", []() {
        ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Scanning wifi..."));
        String content = ERA_F(R"json({"type":"data_wifi","wifi":"[]"})json");
        int nets {0};
        ERaWatchdogFeed();
        TinyGsm* modem = ::getTinyModem();
        if (modem != nullptr) {
            nets = modem->scanNetworks();
        }
        ERaWatchdogFeed();
        ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Found %d wifi"), nets);

        if (nets <= 0) {
            udpERa.send(content.c_str());
            return;
        }

        int indices[nets] {0};
        for (int i = 0; i < nets; ++i) {
            indices[i] = i;
        }
        for (int i = 0; i < nets; ++i) {
            for (int j = i + 1; j < nets; ++j) {
                if (modem->RSSI(indices[j]) > modem->RSSI(indices[i])) {
                    std::swap(indices[i], indices[j]);
                }
            }
        }

        cJSON* root = cJSON_CreateObject();
        if (root == nullptr) {
            udpERa.send(content.c_str());
            return;
        }
        cJSON_AddStringToObject(root, "type", "data_wifi");
        cJSON* array = cJSON_CreateArray();
        if (array != nullptr) {
            int limit {0};
            for (int i = 0; i < nets; ++i) {
                int id = indices[i];
                if (!::getSSID(id).length()) {
                    continue;
                }

                cJSON* item = cJSON_CreateObject();
                if (item == nullptr) {
                    break;
                }
                cJSON_AddStringToObject(item, "ssid", ::getSSID(id).c_str());
                cJSON_AddNumberToObject(item, "rssi", modem->RSSI(id));
                cJSON_AddItemToArray(array, item);
                item = nullptr;
                if(++limit >= WIFI_SCAN_MAX) {
                    break;
                }
            }
            cJSON_AddItemToObject(root, "wifi", array);
            char* ptr = cJSON_PrintUnformatted(root);
            if (ptr != nullptr) {
                content = ptr;
                free(ptr);
            }
            cJSON_Delete(root);
            root = nullptr;
            ptr = nullptr;
        }
        udpERa.send(content.c_str());
    });

    udpERa.on("system/reset", []() {
        String content = ERA_F(R"json({"status":"ok","message":"Configuration reset"})json");
        udpERa.send(content.c_str());
        ERaState::set(StateT::STATE_RESET_CONFIG);
    });

    udpERa.on("system/reboot", []() {
        String content = ERA_F(R"json({"status":"ok","message":"Rebooting..."})json");
        udpERa.send(content.c_str());
        ERaState::set(StateT::STATE_REBOOT);
    });

    udpERa.on("system/identify", []() {
        String content = ERA_F(R"json({"status":"ok","message":"Identifying..."})json");
        udpERa.send(content.c_str());
        IdentifyCallbacks();
    });
}

template <class Transport>
void ERaPnP<Transport>::configInit() {
    this->configLoad();
    if (this->modem != nullptr) {
        this->config(*this->modem, ERaConfig.token, ERaConfig.host,
                    ERaConfig.port, ERaConfig.username, ERaConfig.password);
    }
    if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
        ERaState::set(StateT::STATE_CONNECTING_NETWORK);
    }
    else if (this->priorityConnected(true)) {
        ERA_LOG(TAG, ERA_PSTR("Switching to Scan device..."));
        ERaState::set(StateT::STATE_SCAN_DEVICE);
        ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
    }
    else {
        ERaState::set(StateT::STATE_SWITCH_TO_AP);
        ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
    }
}

template <class Transport>
void ERaPnP<Transport>::configLoad() {
    if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
        return;
    }
    memset(&ERaConfig, 0, sizeof(ERaConfig));
    Base::ERaApi::readBytesFromFlash("config", &ERaConfig, sizeof(ERaConfig));
    ERA_LOG(TAG, ERA_PSTR("Configuration loaded from flash"));
    if (ERaConfig.magic != ERaDefault.magic) {
        this->configLoadDefault();
    }
}

template <class Transport>
void ERaPnP<Transport>::configLoadDefault() {
    char imei[64] {0};
    ERaConfig = ERaDefault;
    this->getImeiChip(imei);
    CopyToArray(imei, ERaConfig.token);
    CopyToArray(imei, ERaConfig.username);
    CopyToArray(imei, ERaConfig.password);
    ERA_LOG(TAG, ERA_PSTR("Using default config"));
}

template <class Transport>
void ERaPnP<Transport>::configSave() {
    if (!ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_STORE)) {
        return;
    }
    ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, false);
    Base::ERaApi::writeBytesToFlash("config", &ERaConfig, sizeof(ERaConfig));
    ERA_LOG(TAG, ERA_PSTR("Configuration stored to flash"));
}

template <class Transport>
void ERaPnP<Transport>::configReset() {
    this->configLoadDefault();
    ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
    ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
    this->configSave();
    ERaState::set(StateT::STATE_SWITCH_TO_AP);
    ERA_LOG(TAG, ERA_PSTR("Resetting configuration"));
}

template <class Transport>
void ERaPnP<Transport>::configBegin() {
    this->configApi();

    ERA_LOG(TAG, ERA_PSTR("Config mode"));

    ERaWatchdogFeed();
    this->networks = this->scanNetworks();
    ERaWatchdogFeed();

    udpERa.setERaORG(Base::getERaORG());
    udpERa.setERaModel(Base::getERaModel());
    udpERa.setDeviceName(this->getDeviceName().c_str());
    udpERa.setDeviceType(Base::getDeviceType());
    udpERa.setDeviceSecretKey(Base::getDeviceSecretKey());
    udpERa.begin((this->authToken == nullptr) ?
                  ERaConfig.token : this->authToken);
    this->modem->beginServer(80);

#if defined(ERA_HAS_ARDUINO_MDNS_H)
    if (this->wifiConnected(true) &&
        this->mdns.begin(::getLocalIP(), this->getDeviceName().c_str())) {
        /* this->mdns.addServiceRecord(this->getMdnsService().c_str(), MDNS_PORT,
                                    MDNS_PROTOCOL); */
        this->mdns.addServiceRecord(this->getMdnsService().c_str(), MDNS_PORT,
                                    MDNS_PROTOCOL, this->getTxtRecords().c_str());
    }
#endif
}

template <class Transport>
void ERaPnP<Transport>::configMode() {
    static bool configStarted {false};
    static MillisTime_t startMillis {0UL};
    if (!configStarted) {
        this->configBegin();
        configStarted = true;
        startMillis = ERaMillis();
    }

    while (ERaState::isConfiguring()) {
        ERaDelay(10);
        udpERa.run();
        this->runServer(this->networks);
        ERaWatchdogFeed();
        Base::appLoop();
        ERaWatchdogFeed();

        this->handleScanDevice();
        if (!ERaState::is(StateT::STATE_SCAN_DEVICE)) {
        }
        else if (this->priorityConnected(true)) {
            if (Base::isNonBlocking()) {
                return;
            }
            continue;
        }
        else {
            ERaState::set(StateT::STATE_SWITCH_TO_AP);
        }

        if (!this->priorityConnected(true)) {
        }
        else if (!ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
            ERaState::set(StateT::STATE_SCAN_DEVICE);
        }
        else if (ERaState::is(StateT::STATE_WAIT_CONFIG)) {
            this->modem->endAP();
            ERaWatchdogFeed();
            ERaState::set(StateT::STATE_CONNECTING_CLOUD);
        }

        if (ERaState::is(StateT::STATE_SWITCH_TO_AP_STA)) {
        }
        else if (!ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
        }
        else if (!ERaRemainingTime(startMillis, WIFI_NET_CHECK_TIMEOUT)) {
            ERaState::set(StateT::STATE_SWITCH_TO_STA);
        }

        if (Base::isNonBlocking()) {
            break;
        }
    }

    if (ERaState::isConfiguring()) {
        return;
    }

    this->waitHandle(this->networks, 1000UL);

    ERaDelay(1000);
    this->modem->scanDelete();
    this->modem->endServer();
#if defined(ERA_HAS_ARDUINO_MDNS_H)
    this->mdns.removeAllServiceRecords();
#endif

    this->networks = "";
    configStarted = false;
}

template <class Transport>
void ERaPnP<Transport>::waitHandle(String& networks, unsigned long ms) {
    if (!ms) {
        return;
    }

    MillisTime_t startMillis = ERaMillis();
    do {
        ERaDelay(10);
        udpERa.run();
        this->runServer(networks);
        ERaWatchdogFeed();
    } while (ERaRemainingTime(startMillis, ms));
}

template <class Transport>
void ERaPnP<Transport>::runServer(String& networks) {
    Client& client = this->server;
    if (!client.connected()) {
        return;
    }

    static char imei[64] {0};
    static char ssidAP[64] {0};
    this->getWiFiName(ssidAP);
    this->getImeiChip(imei);

    PnPRequestT req = PnPRequestT::PNP_REQ_MAX;
    String currentLine = "";
    String configLine = "";
    String contentConfig = "";
    int contentLength {0};
    bool hasBody {false};
    static bool useCaptivePortal {false};
    while (client.connected()) {
        if (!client.available() && !hasBody) {
            ERaDelay(10);
            udpERa.run();
            ERaWatchdogFeed();
            continue;
        }
        char c = client.read();
        if ((c == '\n') || hasBody) {
            if (!currentLine.length()) {
                String response = ERA_F("200 OK");
                String content = ERA_F("");
                String contentType = ERA_F("text/html");

                switch (req) {
                    case PnPRequestT::PNP_REQ_MAX: {
                        response = ERA_F("400 Bad Request");
                        contentType = ERA_F("application/json");
                        content = ERA_F(R"json({"status":"error","message":"Invalid request"})json");
                    }
                        break;
                    case PnPRequestT::PNP_REQ_CAPTIVE:
                    case PnPRequestT::PNP_REQ_REDIRECT_CAPTIVE: {
                        useCaptivePortal = true;
                    }
                        break;
                    case PnPRequestT::PNP_REQ_CONFIG: {
                        ERA_LOG(ERA_PSTR("Config"), ERA_PSTR("Configuration..."));
                        String ssid = this->findArg(configLine, contentConfig, "ssid");
                        String pass = this->findArg(configLine, contentConfig, "pass");
                        String backupSsid = this->findArg(configLine, contentConfig, "backup_ssid");
                        String backupPass = this->findArg(configLine, contentConfig, "backup_pass");

                        String token = this->findArg(configLine, contentConfig, "token");
                        String host = this->findArg(configLine, contentConfig, "host");
                        String port = this->findArg(configLine, contentConfig, "port");
                        String username = this->findArg(configLine, contentConfig, "username");
                        String password = this->findArg(configLine, contentConfig, "password");

                        contentType = ERA_F("application/json");
                        if (!token.length()) {
                            response = ERA_F("400 Bad Request");
                            content = ERA_F(R"json({"status":"error","message":"Invalid token"})json");
                            break;
                        }

                        String save = this->findArg(configLine, contentConfig, "save");
                        ERaConfig.forceSave = !!save.toInt();

                        if (ssid.length()) {
                            CopyToArray(ssid, ERaConfig.ssid);
                            CopyToArray(pass, ERaConfig.pass);
                        }

                        if (backupSsid.length()) {
                            ERaConfig.hasBackup = true;
                            CopyToArray(backupSsid, ERaConfig.backupSSID);
                            CopyToArray(backupPass, ERaConfig.backupPass);
                        }
                        else {
                            ERaConfig.hasBackup = false;
                        }

                        if (token.length()) {
                            CopyToArray(token, ERaConfig.token);
                        }
                        if (host.length()) {
                            CopyToArray(host, ERaConfig.host);
                        }
                        if (port.length()) {
                            ERaConfig.port = port.toInt();
                        }
                        if (username.length()) {
                            CopyToArray(username, ERaConfig.username);
                        }
                        else {
                            CopyToArray(ERaConfig.token, ERaConfig.username);
                        }
                        if (password.length()) {
                            CopyToArray(password, ERaConfig.password);
                        }
                        else {
                            CopyToArray(ERaConfig.token, ERaConfig.password);
                        }

                        if (ERaConfig.forceSave) {
                            content = ERA_F(R"json({"status":"ok","message":"Configuration saved"})json");
                        }
                        else {
                            content = ERA_F(R"json({"status":"ok","message":"Connecting wifi..."})json");
                        }

                        if (ssid.length() || ERaConfig.forceSave ||
                            this->priorityConnected(true)) {
                            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_PNP, true);
                            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
                            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
                            if (this->priorityConnected(true)) {
                                this->modem->endAP();
                                ERaWatchdogFeed();
                                ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                            }
                            else {
                                ERaState::set(StateT::STATE_SWITCH_TO_STA);
                            }
                        }
                        if (ERaConfig.forceSave) {
                            this->configSave();
                        }
                        if (useCaptivePortal) {
                            response = ERA_F("302 Found");
                            content = ERA_F("");
                            contentType = ERA_F("text/plain");
                            req = PnPRequestT::PNP_REQ_REDIRECT_CAPTIVE;
                        }
                    }
                        break;
                    case PnPRequestT::PNP_REQ_BOARD_INFO: {
                        ERA_LOG(ERA_PSTR("Info"), ERA_PSTR("Board info"));
                        cJSON* root = cJSON_CreateObject();
                        if (root != nullptr) {
                            cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
                            cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
                            cJSON_AddStringToObject(root, INFO_IMEI, imei);
                            cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, ERaConfig.token);
                            cJSON_AddStringToObject(root, INFO_DEVICE_NAME, this->getDeviceName().c_str());
                            cJSON_AddStringToObject(root, INFO_DEVICE_TYPE, Base::getDeviceType());
                            cJSON_AddStringToObject(root, INFO_DEVICE_SECRET, Base::getDeviceSecretKey());
                            cJSON_AddStringToObject(root, INFO_BUILD_DATE, BUILD_DATE_TIME);
                            cJSON_AddStringToObject(root, INFO_VERSION, ERA_VERSION);
                            cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
                            cJSON_AddStringToObject(root, INFO_SSID, ssidAP);
                            cJSON_AddStringToObject(root, INFO_PASS, WIFI_AP_PASS);
                            cJSON_AddStringToObject(root, INFO_BSSID, ::getBSSID().c_str());
                            cJSON_AddStringToObject(root, INFO_MAC, ::getMAC().c_str());
                            cJSON_AddStringToObject(root, INFO_LOCAL_IP, ::getSoftAPIP().c_str());
                            char* ptr = cJSON_PrintUnformatted(root);
                            if (ptr != nullptr) {
                                content = ptr;
                                free(ptr);
                            }
                            cJSON_Delete(root);
                            root = nullptr;
                            ptr = nullptr;
                        }
                        contentType = ERA_F("application/json");
                        if (content.length()) {
                            IdentifyCallbacks();
                        }
                        else {
                            response = ERA_F("500 Internal Server Error");
                            content = ERA_F(R"json({"status":"error","message":"Unable to retrieve board information"})json");
                        }
                        break;
                    }
                    case PnPRequestT::PNP_REQ_SCAN_WIFI: {
                        ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Scanning wifi..."));
                        contentType = ERA_F("application/json");
                        content = networks;
                    }
                        break;
                    case PnPRequestT::PNP_REQ_RESET: {
                        contentType = ERA_F("application/json");
                        content = ERA_F(R"json({"status":"ok","message":"Configuration reset"})json");
                        ERaState::set(StateT::STATE_RESET_CONFIG);
                    }
                        break;
                    case PnPRequestT::PNP_REQ_REBOOT: {
                        contentType = ERA_F("application/json");
                        content = ERA_F(R"json({"status":"ok","message":"Rebooting..."})json");
                        ERaState::set(StateT::STATE_REBOOT);
                    }
                        break;
                    case PnPRequestT::PNP_REQ_IDENTIFY: {
                        contentType = ERA_F("application/json");
                        content = ERA_F(R"json({"status":"ok","message":"Identifying..."})json");
                        IdentifyCallbacks();
                    }
                        break;
                    default: {
                        content = ERA_FPSTR(webIndex);
                        content += ERA_FPSTR(webScript);
                        content += ERA_FPSTR(webStyle);
                        WebProcessor(content);
                    }
                        break;
                }

                hasBody = false;

                client.println(ERA_F("HTTP/1.1 ") + response);
                client.println(ERA_F("Content-Type:") + contentType);
                if (req == PnPRequestT::PNP_REQ_REDIRECT_CAPTIVE) {
                    client.println(ERA_F("Location: /wifi?save"));
                }
                else if (req == PnPRequestT::PNP_REQ_CAPTIVE) {
                    client.println(ERA_F("Content-Length:") + String(sizeof(ERA_CAPTIVE_INDEX)));
                    client.println(ERA_F("Content-Encoding: gzip"));
                }
                client.println();
                if (req == PnPRequestT::PNP_REQ_CAPTIVE) {
                    client.write(ERA_CAPTIVE_INDEX, sizeof(ERA_CAPTIVE_INDEX));
                }
                else {
                    client.println(content);
                }
                break;
            }
            else {
                currentLine = "";
            }
        }
        else if (c != '\r') {
            currentLine += c;
            continue;
        }

        if (currentLine.indexOf("GET /wifi") >= 0) {
            req = PnPRequestT::PNP_REQ_CAPTIVE;
        }
        else if (currentLine.indexOf("GET /api/board_info.json") >= 0) {
            req = PnPRequestT::PNP_REQ_BOARD_INFO;
        }
        else if (currentLine.indexOf("GET /api/wifi_scan.json") >= 0) {
            req = PnPRequestT::PNP_REQ_SCAN_WIFI;
        }
        else if (currentLine.indexOf(" /api/config") >= 0) {
            req = PnPRequestT::PNP_REQ_CONFIG;
            int idx = currentLine.indexOf("?");
            configLine = String("&") + currentLine.substring(idx + 1, currentLine.lastIndexOf(' ')) + "&";

            while (client.available()) {
                String line = client.readStringUntil('\r');
                line.trim();
                line.toLowerCase();
                if (line.startsWith("content-length:")) {
                    contentLength = line.substring(line.lastIndexOf(':') + 1).toInt();
                }
                else if (!line.length()) {
                    break;
                }
                ERaDelay(10);
            }

            int written {0};
            uint8_t buf[256] {0};
            if (client.available()) {
                client.readStringUntil('\n');
            }
            while (client.connected() && (written < contentLength)) {
                ERaDelay(10);
                unsigned long timeout = ERaMillis();
                while (client.connected() && !client.available()) {
                    ERaDelay(1);
                    if (!ERaRemainingTime(timeout, 1000UL)) {
                        ERA_LOG_ERROR(TAG, ERA_PSTR("Response timeout"));
                        break;
                    }
                }

                int len = client.read(buf, sizeof(buf) - 1);
                if (len <= 0) {
                    continue;
                }
                buf[len] = 0;
                contentConfig += (char*)buf;
                written += len;
            }
            if (written == contentLength) {
                hasBody = true;
            }
        }
        else if (currentLine.indexOf(" /api/reset") >= 0) {
            req = PnPRequestT::PNP_REQ_RESET;
        }
        else if (currentLine.indexOf(" /api/reboot") >= 0) {
            req = PnPRequestT::PNP_REQ_REBOOT;
        }
        else if (currentLine.indexOf(" /api/identify") >= 0) {
            req = PnPRequestT::PNP_REQ_IDENTIFY;
        }
    }

    client.stop();
}

template <class Transport>
String ERaPnP<Transport>::scanNetworks() {
    ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Scanning wifi..."));
    String content = ERA_F("[]");
    int nets = this->modem->scanNetworks();
    ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Found %d wifi"), nets);

    if (nets <= 0) {
        return content;
    }

    cJSON* root = this->scanNetworksJson(nets, false);
    if (root != nullptr) {
        char* ptr = cJSON_PrintUnformatted(root);
        if (ptr != nullptr) {
            content = ptr;
            free(ptr);
        }
        cJSON_Delete(root);
        root = nullptr;
        ptr = nullptr;
    }

    this->modem->scanDelete();
    return content;
}

template <class Transport>
cJSON* ERaPnP<Transport>::scanNetworksJson(int nets, bool compact) {
    if (nets <= 0) {
        return nullptr;
    }

    cJSON* root = cJSON_CreateArray();
    if (root == nullptr) {
        return nullptr;
    }

    int indices[nets] {0};
    for (int i = 0; i < nets; ++i) {
        indices[i] = i;
    }
    for (int i = 0; i < nets; ++i) {
        for (int j = i + 1; j < nets; ++j) {
            if (this->modem->RSSI(indices[j]) > this->modem->RSSI(indices[i])) {
                std::swap(indices[i], indices[j]);
            }
        }
    }

    int limit {0};
    for (int i = 0; i < nets; ++i) {
        int id = indices[i];
        if (!::getSSID(id).length()) {
            continue;
        }

        cJSON* item = cJSON_CreateObject();
        if (item == nullptr) {
            break;
        }
        cJSON_AddStringToObject(item, "ssid", ::getSSID(id).c_str());
        cJSON_AddNumberToObject(item, "rssi", this->modem->RSSI(id));
        if (!compact) {
            cJSON_AddNumberToObject(item, "channel", this->modem->channel(id));
            this->addEncryptionType(id, item);
        }
        cJSON_AddItemToArray(root, item);
        item = nullptr;
        if(++limit >= WIFI_SCAN_MAX) {
            break;
        }
    }

    return root;
}

template <class Transport>
void ERaPnP<Transport>::addEncryptionType(uint8_t networkItem, cJSON* const item) {
    switch (this->modem->encryptionType(networkItem)) {
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_OPEN:
            cJSON_AddStringToObject(item, "encryption", "OPEN");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WEP:
            cJSON_AddStringToObject(item, "encryption", "WEP");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WPA_PSK:
            cJSON_AddStringToObject(item, "encryption", "WPA/PSK");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WPA2_PSK:
            cJSON_AddStringToObject(item, "encryption", "WPA2/PSK");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WPA_WPA2_PSK:
            cJSON_AddStringToObject(item, "encryption", "WPA/WPA2/PSK");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WPA2_ENTERPRISE:
            cJSON_AddStringToObject(item, "encryption", "WPA2/ENTERPRISE");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WPA3_PSK:
            cJSON_AddStringToObject(item, "encryption", "WPA3/PSK");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WPA2_WPA3_PSK:
            cJSON_AddStringToObject(item, "encryption", "WPA2/WPA3/PSK");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WAPI_PSK:
            cJSON_AddStringToObject(item, "encryption", "WAPI/PSK");
            break;
        default:
            cJSON_AddStringToObject(item, "encryption", "unknown");
            break;
    }
}

template <class Transport>
String ERaPnP<Transport>::urlDecode(const String& text) {
    String decoded = "";
    char temp[] = "0x00";
    unsigned int len = text.length();
    unsigned int i = 0;
    while (i < len) {
        char decodedChar;
        char encodedChar = text.charAt(i++);
        if ((encodedChar == '%') && (i + 1 < len)) {
            temp[2] = text.charAt(i++);
            temp[3] = text.charAt(i++);

            decodedChar = strtol(temp, NULL, 16);
        }
        else {
            if (encodedChar == '+') {
                decodedChar = ' ';
            }
            else {
                decodedChar = encodedChar;
            }
        }
        decoded += decodedChar;
    }
    return decoded;
}

template <class Transport>
String ERaPnP<Transport>::findArg(const String& url, const String& body, const String& arg) {
    String value = this->urlFindArg(url, arg);
    if (!value.length()) {
        value = this->bodyFindArg(body, arg);
    }
    return value;
}

template <class Transport>
String ERaPnP<Transport>::urlFindArg(const String& url, const String& arg) {
    int s = url.indexOf("&" + arg + "=");
    if (s < 0) {
        return "";
    }
    int sLen = arg.length() + 2;
    int e = url.indexOf('&', s + sLen);
    return this->urlDecode(url.substring(s + sLen, e));
}

template <class Transport>
String ERaPnP<Transport>::bodyFindArg(const String& body, const String& arg) {
    cJSON* root = cJSON_Parse(body.c_str());
    if (!cJSON_IsObject(root)) {
        cJSON_Delete(root);
        root = nullptr;
        return String("");
    }

    String data = "";

    cJSON* item = cJSON_GetObjectItem(root, arg.c_str());
    if (cJSON_IsString(item)) {
        data = item->valuestring;
    }
    else if (cJSON_IsNumber(item)) {
        data = String(item->valueint);
    }
    else if (cJSON_IsBool(item)) {
        data = String(item->valueint);
    }

    cJSON_Delete(root);
    root = nullptr;
    item = nullptr;
    return data;
}

template <class Transport>
void ERaPnP<Transport>::reFlagNewNetwork(const char* ssid) {
    if (this->newWiFi.flag == WiFiFlagT::WIFI_FLAG_INVALID) {
        return;
    }

    if (ERaStrCmp(ssid, this->newWiFi.ssid)) {
        return;
    }
    this->newWiFi.flag = WiFiFlagT::WIFI_FLAG_FAILED;
}

template <class Transport>
void ERaPnP<Transport>::copyNewNetwork() {
    ClearArray(ERaConfig.backupSSID);
    ClearArray(ERaConfig.backupPass);
    CopyToArray(ERaConfig.ssid, ERaConfig.backupSSID);
    CopyToArray(ERaConfig.pass, ERaConfig.backupPass);
    ClearArray(ERaConfig.ssid);
    ClearArray(ERaConfig.pass);
    CopyToArray(this->newWiFi.ssid, ERaConfig.ssid);
    CopyToArray(this->newWiFi.pass, ERaConfig.pass);
    ERaConfig.hasBackup = true;
    ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);

    this->getTransp().setSSID(ERaConfig.ssid);
}

template <class Transport>
void ERaPnP<Transport>::connectNetwork() {
    static bool connectBackup {false};

    if (!connectBackup) {
        this->connectNetwork(ERaConfig.ssid, ERaConfig.pass, Base::isNonBlocking());
        this->checkConnectWiFi(ERaConfig.ssid);
    }
    else {
        this->connectWiFiBackup();
    }

    if ((this->wifiState == WiFiStateT::WIFI_STATE_CONNECTING) ||
        (this->wifiState == WiFiStateT::WIFI_STATE_RETRYING)) {
        return;
    }
    else if (this->wifiState == WiFiStateT::WIFI_STATE_CONNECTED) {
    }
    else if (!connectBackup && ERaConfig.hasBackup) {
        connectBackup = true;
        this->wifiState = WiFiStateT::WIFI_STATE_IDLE;
        return;
    }

    if (this->netConnected(false)) {
        /* Udp */
        if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_UDP)) {
            String content = ERA_F(R"json({"status":"ok","message":"Connected to WiFi"})json");
            ERaDelay(100);
            udpERa.send(content.c_str());
            ERaDelay(100);
            udpERa.sendBoardInfo();
            ERaDelay(500);
            udpERa.stop();
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_UDP, false);
        }
        ERaDelay(100);
        this->modem->endAP();
        /* Udp */
        ERaState::set(StateT::STATE_CONNECTING_CLOUD);
    }
    else if (Base::isConfigMode()) {
    }
    else {
        /* Udp */
        if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_UDP)) {
            String content = ERA_F(R"json({"status":"error","message":"Connect WiFi failed"})json");
            ERaDelay(100);
            udpERa.send(content.c_str());
            ERaDelay(500);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_UDP, false);
        }
        /* Udp */
        ERaState::set(StateT::STATE_SWITCH_TO_AP);
        ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
    }

    connectBackup = false;
    this->wifiState = WiFiStateT::WIFI_STATE_IDLE;
}

template <class Transport>
void ERaPnP<Transport>::connectNewNetwork() {
    if (this->newWiFi.flag != WiFiFlagT::WIFI_FLAG_INVALID) {
        this->connectNewNetworkResult();
        ERaState::set(StateT::STATE_CONNECTED);
        return;
    }

    if (!strlen(this->newWiFi.ssid)) {
        ERaState::set(StateT::STATE_CONNECTING_NETWORK);
        return;
    }

    this->modem->networkDisconnect();

    /* Waiting WiFi disconnect */
    MillisTime_t startMillis = ERaMillis();
    while (this->modem->isNetworkConnected()) {
        ERaDelay(10);
        ERaWatchdogFeed();
        if (!ERaRemainingTime(startMillis, 5000)) {
            ERaState::set(StateT::STATE_CONNECTING_NETWORK);
            return;
        }
    }

    this->connectNetwork(this->newWiFi.ssid, this->newWiFi.pass);
    if (this->wifiConnected(true)) {
        this->newWiFi.flag = WiFiFlagT::WIFI_FLAG_CONNECTED;
        ERaState::set(StateT::STATE_CONNECTING_CLOUD);
    }
    else {
        this->newWiFi.flag = WiFiFlagT::WIFI_FLAG_FAILED;
        ERaState::set(StateT::STATE_CONNECTING_NETWORK);
    }
}

template <class Transport>
void ERaPnP<Transport>::connectNewNetworkResult() {
    bool status {false};
    switch (this->newWiFi.flag) {
        case WiFiFlagT::WIFI_FLAG_CONNECTED:
            status = Base::sendChangeResultWiFi(R"json({"success":1})json");
            if (status) {
                this->copyNewNetwork();
            }
            ERA_LOG(TAG, ERA_PSTR("Connected to new WiFi: %s"), this->newWiFi.ssid);
            break;
        case WiFiFlagT::WIFI_FLAG_CURRENT:
            status = Base::sendChangeResultWiFi(R"json({"success":1})json");
            ERA_LOG(TAG, ERA_PSTR("Connected to new WiFi: %s"), this->newWiFi.ssid);
            break;
        case WiFiFlagT::WIFI_FLAG_FAILED:
            status = Base::sendChangeResultWiFi(R"json({"success":0})json");
            ERA_LOG_ERROR(TAG, ERA_PSTR("Connect new WiFi failed: %s"), this->newWiFi.ssid);
            break;
        default:
            break;
    }
    if (status) {
        ClearStruct(this->newWiFi);
    }
}

template <class Transport>
void ERaPnP<Transport>::connectCloud() {
    static bool configStarted {false};
    if (!configStarted && (this->modem != nullptr)) {
        ERA_LOG(TAG, ERA_PSTR("Connecting cloud..."));
        this->getTransp().setSignalQuality(this->modem->getSignalQuality());
        this->config(*this->modem, ERaConfig.token, ERaConfig.host,
                    ERaConfig.port, ERaConfig.username, ERaConfig.password);
        configStarted = true;
    }

    if (Base::connect()) {
        ERaConfig.connected = true;
        ERaOptConnected(this);
        this->connectNewNetworkResult();
        this->configSave();
        ERaState::set(StateT::STATE_CONNECTED);
    }
    else if (Base::isConfigMode()) {
    }
    else if ((ERaConfig.connected || ERaConfig.forceSave) &&
        this->getTransp().connectionDenied()) {
        ERaState::set(StateT::STATE_SWITCH_TO_AP);
        ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
    }
    else if (this->getTransp().reachedConnectionLimit()) {
        ERaState::set(StateT::STATE_CONNECTING_NETWORK);
    }

    if (!ERaState::is(StateT::STATE_CONNECTING_CLOUD)) {
        configStarted = false;
    }
}

template <class Transport>
bool ERaPnP<Transport>::connectNetwork(const char* ssid, const char* pass, bool nonBlocking) {
    if (this->wifiState == WiFiStateT::WIFI_STATE_CONNECTING) {
        return false;
    }
    if (this->priorityConnected(true)) {
        this->wifiState = WiFiStateT::WIFI_STATE_CONNECTED;
        return true;
    }
    if (ssid == nullptr) {
        return false;
    }
    if (!strlen(ssid)) {
        return false;
    }

    MillisTime_t startMillis = ERaMillis();
    while (!this->connectWiFi(ssid, pass, nonBlocking)) {
        ERaWatchdogFeed();
        ERaDelay(100);
        Base::appLoop();
        ERaWatchdogFeed();
        if (Base::isConfigMode() ||
            !ERaRemainingTime(startMillis, WIFI_NET_CONNECT_TIMEOUT)) {
            this->wifiState = WiFiStateT::WIFI_STATE_FAILED;
            ERA_LOG_ERROR(TAG, ERA_PSTR("Connect %s failed"), ssid);
            return false;
        }
    }

    if (nonBlocking) {
        return true;
    }

    this->reFlagNewNetwork(ssid);
    this->getTransp().setSSID(ssid);

    this->wifiState = WiFiStateT::WIFI_STATE_CONNECTED;
    return true;
}

template <class Transport>
bool ERaPnP<Transport>::connectWiFi(const char* ssid, const char* pass, bool nonBlocking) {
    if (!strlen(ssid)) {
        return false;
    }

    ERaWatchdogFeed();

    ERA_LOG(TAG, ERA_PSTR("Modem init"));
    if (!this->beginModem()) {
        ERA_LOG_ERROR(TAG, ERA_PSTR("Can't init modem"));
        return false;
    }

    ERaWatchdogFeed();

    ERA_LOG(TAG, ERA_PSTR("Connecting to %s..."), ssid);
    if (!this->modem->networkConnect(ssid, pass)) {
        ERA_LOG_ERROR(TAG, ERA_PSTR("Connect %s failed"), ssid);
        return false;
    }

    if (this->wifiState != WiFiStateT::WIFI_STATE_RETRYING) {
        this->wifiState = WiFiStateT::WIFI_STATE_STARTED;
    }
    if (nonBlocking) {
        return true;
    }

    ERaWatchdogFeed();
    this->wifiState = WiFiStateT::WIFI_STATE_CONNECTING;

    ERA_LOG(TAG, ERA_PSTR("Waiting for network..."));
    if (!this->modem->waitForNetwork()) {
        this->wifiState = WiFiStateT::WIFI_STATE_RETRYING;
        ERA_LOG_ERROR(TAG, ERA_PSTR("Connect %s failed"), ssid);
        return false;
    }

    ERaWatchdogFeed();

    ERA_LOG(TAG, ERA_PSTR("Connected to %s"), ssid);
    return true;
}

template <class Transport>
void ERaPnP<Transport>::connectNewWiFi(const char* ssid, const char* pass) {
    if (ssid == nullptr) {
        return;
    }
    if (!strlen(ssid)) {
        return;
    }
    if (pass == nullptr) {
        pass = "";
    }

    ClearStruct(this->newWiFi);
    CopyToArray(ssid, this->newWiFi.ssid);
    CopyToArray(pass, this->newWiFi.pass);
    if (!ERaStrCmp(ERaConfig.ssid, ssid)) {
    }
    else if (ERaStrCmp(ERaConfig.pass, pass)) {
        this->newWiFi.flag = WiFiFlagT::WIFI_FLAG_CURRENT;
    }
    else {
        this->newWiFi.flag = WiFiFlagT::WIFI_FLAG_FAILED;
    }

    ERaState::set(StateT::STATE_CONNECTING_NEW_NETWORK);
}

template <class Transport>
void ERaPnP<Transport>::connectWiFiBackup() {
    if (!ERaConfig.hasBackup) {
        return;
    }
    this->connectNetwork(ERaConfig.backupSSID, ERaConfig.backupPass, Base::isNonBlocking());
    this->checkConnectWiFi(ERaConfig.backupSSID);
}

template <class Transport>
void ERaPnP<Transport>::checkConnectWiFi(const char* ssid) {
    static MillisTime_t startRetry {0UL};
    static MillisTime_t startMillis {0UL};

    if (!Base::isNonBlocking()) {
        return;
    }

    if (this->wifiState == WiFiStateT::WIFI_STATE_STARTED) {
        startRetry = ERaMillis();
        startMillis = ERaMillis();
        this->wifiState = WiFiStateT::WIFI_STATE_CONNECTING;
    }
    else if (this->wifiState == WiFiStateT::WIFI_STATE_RETRYING) {
        startRetry = ERaMillis();
        this->wifiState = WiFiStateT::WIFI_STATE_CONNECTING;
    }
    else if (this->wifiState != WiFiStateT::WIFI_STATE_CONNECTING) {
    }
    else if (!ERaRemainingTime(startMillis, WIFI_NET_CONNECT_TIMEOUT)) {
        this->wifiState = WiFiStateT::WIFI_STATE_FAILED;
        ERA_LOG_ERROR(TAG, ERA_PSTR("Connect %s failed"), ssid);
    }
    else if (!ERaRemainingTime(startRetry, 20000UL)) {
        this->wifiState = WiFiStateT::WIFI_STATE_RETRYING;
        ERA_LOG_ERROR(TAG, ERA_PSTR("Connect %s timeout"), ssid);
    }

    ERaWatchdogFeed();
    Base::appLoop();
    ERaWatchdogFeed();

    if (!this->wifiConnected(true)) {
        return;
    }
    this->reFlagNewNetwork(ssid);
    this->getTransp().setSSID(ssid);
    ERA_LOG(TAG, ERA_PSTR("Connected to %s"), ssid);

    this->wifiState = WiFiStateT::WIFI_STATE_CONNECTED;
}

template <class Transport>
void ERaPnP<Transport>::requestListWiFi() {
    ERaState::set(StateT::STATE_REQUEST_LIST_WIFI);
}

template <class Transport>
void ERaPnP<Transport>::responseListWiFi() {
    String networks = this->scanNetworks();
    Base::sendListWiFi(networks.c_str());
}

template <class Transport>
void ERaPnP<Transport>::switchToAP() {
    ERA_LOG(TAG, ERA_PSTR("Switching to AP..."));

    char ssidAP[64] {0};
    this->getWiFiName(ssidAP);

    ERaDelay(100);
    ERaWatchdogFeed();
    this->modem->beginAP(ssidAP, WIFI_AP_PASS);
    ERaWatchdogFeed();
    this->modem->configAP(WIFI_AP_IP);
    ERaWatchdogFeed();

    ERA_LOG(TAG, ERA_PSTR("AP SSID: %s"), ssidAP);
    ERA_LOG(TAG, ERA_PSTR("AP IP: %s"), ::getSoftAPIP().c_str());

    ERaState::set(StateT::STATE_WAIT_CONFIG);
}

template <class Transport>
void ERaPnP<Transport>::switchToSTA() {
    ERA_LOG(TAG, ERA_PSTR("Switching to STA..."));

    ERaDelay(100);
    this->modem->endAP();
    ERaWatchdogFeed();

    ERaState::set(StateT::STATE_CONNECTING_NETWORK);
}

template <class Transport>
void ERaPnP<Transport>::switchToAPSTA() {
    ERA_LOG(TAG, ERA_PSTR("Switching to AP STA..."));

    ERaDelay(100);

    ERaState::set(StateT::STATE_CONNECTING_NETWORK);
}

template <class Transport>
void ERaPnP<Transport>::getSignalQuality() {
    unsigned long currentMillis = ERaMillis();
    if ((currentMillis - this->prevMillisSignal) < ERA_GET_SIGNAL_TIMEOUT) {
        return;
    }
    unsigned long skipTimes = ((currentMillis - this->prevMillisSignal) / ERA_GET_SIGNAL_TIMEOUT);
    // update time
    this->prevMillisSignal += (ERA_GET_SIGNAL_TIMEOUT * skipTimes);

    ERaWatchdogFeed();

    this->getTransp().setSignalQuality(this->modem->getSignalQuality());

    ERaWatchdogFeed();
}

template <class Transport>
String ERaPnP<Transport>::getDeviceName() {
    String deviceName = Base::getDeviceName();
    deviceName += "-";
    deviceName += this->getChipID(4UL);
    return deviceName;
}

#if defined(ERA_HAS_ARDUINO_MDNS_H)
    template <class Transport>
    String ERaPnP<Transport>::getMdnsService() {
        String service = this->getDeviceName();
        service += ".";
        service += MDNS_SERVICE;
        return service;
    }

    template <class Transport>
    String ERaPnP<Transport>::getTxtDeviceName() {
        String record;
        String name = INFO_DEVICE_NAME;
        name += "=";
        name += this->getDeviceName();
        record += (char)name.length();
        record += name;
        return record;
    }

    template <class Transport>
    String ERaPnP<Transport>::getTxtDeviceType() {
        String record; 
        String type = INFO_DEVICE_TYPE;
        type += "=";
        type += Base::getDeviceType();
        record += (char)type.length();
        record += type;
        return record;
    }

    template <class Transport>
    String ERaPnP<Transport>::getTxtDeviceSecretKey() {
        String record;
        String secret = INFO_DEVICE_SECRET;
        secret += "=";
        secret += Base::getDeviceSecretKey();
        record += (char)secret.length();
        record += secret;
        return record;
    }

    template <class Transport>
    String ERaPnP<Transport>::getTxtRecords() {
        String records = "\x6path=/";
        records += this->getTxtDeviceName();
        records += this->getTxtDeviceType();
        records += this->getTxtDeviceSecretKey();
        return records;
    }
#endif

template <class Transport>
String ERaPnP<Transport>::getChipID(size_t len) {
    String mac = this->modem->BSSID();
    mac.replace(":", "");
    if (len) {
        mac = mac.substring(mac.length() - len);
    }
    return mac;
}

#if defined(ERA_HAS_ARDUINO_MDNS_H)
    template <class Transport>
    void ERaPnP<Transport>::handleScanDevice() {
        this->mdns.run();
    }
#else
    template <class Transport>
    void ERaPnP<Transport>::handleScanDevice() {
    }
#endif

template <class Transport>
template <int size>
void ERaPnP<Transport>::getWiFiName(char(&ptr)[size], bool withPrefix, size_t len) {
    ClearArray(ptr);
    if (withPrefix) {
        FormatString(ptr, "%s.%s.%s", Base::getERaORG(), Base::getERaModel(),
                                      this->getChipID(len).c_str());
    }
    else {
        FormatString(ptr, "%s.%s", Base::getERaORG(), this->getChipID(len).c_str());
    }
    ERaToLowerCase(ptr);
}

template <class Transport>
template <int size>
void ERaPnP<Transport>::getImeiChip(char(&ptr)[size]) {
    ClearArray(ptr);
#if defined(ERA_AUTH_TOKEN)
    FormatString(ptr, ERA_AUTH_TOKEN);
#else
    if ((this->authToken != nullptr) && strlen(this->authToken)) {
        FormatString(ptr, this->authToken);
    }
    else {
        FormatString(ptr, "ERA-%s", this->getChipID().c_str());
    }
#endif
}

template <class Transport>
void ERaPnP<Transport>::replace(char* buf, char src, char dst) {
    if (buf == nullptr) {
        return;
    }
    size_t len = strlen(buf);
    for (size_t i = 0; i < len; ++i) {
        if (buf[i] != src) {
            continue;
        }
        buf[i] = dst;
    }
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    const char* ssid = this->thisProto().getTransp().getSSID();
    int16_t signal = this->thisProto().getTransp().getSignalQuality();

    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_PNP));
    cJSON_AddStringToObject(root, INFO_NETWORK_PROTOCOL, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_SSID, ((ssid != nullptr) ? ssid : (::getSSID().c_str())));
    cJSON_AddStringToObject(root, INFO_BSSID, ::getBSSID().c_str());
    cJSON_AddNumberToObject(root, INFO_RSSI, signal);
    cJSON_AddNumberToObject(root, INFO_SIGNAL_STRENGTH, SignalToPercentage(signal));
    cJSON_AddStringToObject(root, INFO_MAC, ::getMAC().c_str());
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ::getLocalIP().c_str());
    cJSON_AddNumberToObject(root, INFO_SSL, ERaInfoSSL(ERaConfig.port));

    /* Override info */
    ERaInfo(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addSelfInfo(cJSON* root) {
    int16_t signal = this->thisProto().getTransp().getSignalQuality();

    cJSON_AddNumberToObject(root, SELF_CHIP_TEMPERATURE, 5000);
    cJSON_AddNumberToObject(root, SELF_SIGNAL_STRENGTH, SignalToPercentage(signal));

    /* Override self info */
    ERaSelfInfo(root);
}

#if defined(ERA_MODBUS)
    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
        const char* ssid = this->thisProto().getTransp().getSSID();
        int16_t signal = this->thisProto().getTransp().getSignalQuality();

        cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
        cJSON_AddNumberToObject(root, INFO_MB_RSSI, signal);
        cJSON_AddNumberToObject(root, INFO_MB_SIGNAL_STRENGTH, SignalToPercentage(signal));
        cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ((ssid != nullptr) ? ssid : (::getSSID().c_str())));

        /* Override modbus info */
        ERaModbusInfo(root);
    }
#endif

#endif /* INC_ERA_PNP_ARDUINO_HPP_ */
