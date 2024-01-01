#ifndef INC_ERA_PNP_ARDUINO_HPP_
#define INC_ERA_PNP_ARDUINO_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "WiFi"
#endif

#include <TinyGsmClient.hpp>

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

#include <PnP/ERaWeb.hpp>

#define WIFI_SCAN_MAX                 15
#define WIFI_SCAN_TIMEOUT             20000
#define WIFI_NET_CONNECT_TIMEOUT      60000
#define WIFI_CLOUD_CONNECT_TIMEOUT    60000
#define WIFI_NET_CHECK_TIMEOUT        (WIFI_NET_CONNECT_TIMEOUT * 3)
#define WIFI_AP_PASS                  "Eoh@2021"
#define WIFI_AP_IP                    IPAddress(192, 168, 27, 1)
#define WIFI_AP_Subnet                IPAddress(255, 255, 255, 0)

typedef struct __WiFiConfig_t {
    char ssid[64];
    char pass[64];
} WiFiConfig_t;

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

#include <Network/ERaUdpTiny.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static TinyGsmUdp Udp;
    static ERaUdp<TinyGsmUdp> udpERa(Udp);
#else
    #define Udp     StaticHelper<TinyGsmUdp>::instance()
    #define udpERa  StaticRefHelper<ERaUdp<TinyGsmUdp>, TinyGsmUdp>::instance(Udp)
#endif

template <class Transport>
class ERaPnP
    : public ERaProto<Transport, ERaFlash>
{
    enum PnPRequestT {
        PNP_REQ_BOARD_INFO = 0,
        PNP_REQ_SCAN_WIFI = 1,
        PNP_REQ_CONFIG = 2,
        PNP_REQ_RESET = 3,
        PNP_REQ_REBOOT = 4,
        PNP_REQ_MAX = 255,
    };

    const char* TAG = "WiFi";
    const char* HOSTNAME = "ERa";
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
        , prevMillis(0UL)
        , prevMillisSignal(0UL)
    {}
    ~ERaPnP()
    {}

    void restartOption(bool soft) {
        this->softRestart = soft;
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

        this->connectNetwork(ERaConfig.ssid, ERaConfig.pass);
        if (this->netConnected()) {
            if (Base::connect()) {
                ERaOptConnected(this);
                ERaState::set(StateT::STATE_CONNECTED);
            }
            else {
                ERaState::set(StateT::STATE_CONNECTING_CLOUD);
            }
        }
        else if (Base::isConfigMode()) {
            return;
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

    void setPersistent(bool enable) {
        this->persistent = enable;
    }

    void switchToConfig(bool erase = false) {
        ERaConfig.connected = false;
        ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
        if (erase) {
            ERaState::set(StateT::STATE_RESET_CONFIG);
        }
        else {
            ERaState::set(StateT::STATE_SWITCH_TO_AP);
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
    void configMode();
    void waitHandle(String& networks, unsigned long ms);
    void runServer(String& networks);
    String scanNetworks();
    void addEncryptionType(uint8_t networkItem, cJSON* const item);
    String urlDecode(const String& text);
    String urlFindArg(const String& url, const String& arg);
    String bodyFindArg(const String& body, const String& arg);
    void connectNetwork();
    void connectNewNetwork();
    void connectCloud();
    bool connectNetwork(const char* ssid, const char* pass);
    bool connectWiFi(const char* ssid, const char* pass);
    void connectNewWiFi(const char* ssid, const char* pass) override;
    void connectWiFiBackup();
    void switchToAP();
    void switchToSTA();
    void switchToAPSTA();
    void getSignalQuality();

    template <int size>
    void getWiFiName(char(&ptr)[size], bool withPrefix = true);
    template <int size>
    void getImeiChip(char(&ptr)[size]);
    void replace(char* buf, char src, char dst);

    bool netConnected() {
        static bool _connected {false};

        unsigned long currentMillis = ERaMillis();
        if ((currentMillis - this->prevMillis) < 500UL) {
            return _connected;
        }
        unsigned long skipTimes = ((currentMillis - this->prevMillis) / 500UL);
        // update time
        this->prevMillis += (500UL * skipTimes);

        _connected = this->modem->isNetworkConnected();
        return _connected;
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
            return;
        }

        ERaWatchdogFeed();

        this->powerOff();
        ERaDelay(2000);
        this->powerOn();
        ERaDelay(500);

        ERaWatchdogFeed();

        if (this->softRestart) {
            this->modem->restart();
        }
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

    TinyGsm* modem;
    TinyGsmServer server;
    const char* authToken;
    bool persistent;
    int resetPin;
    bool invertPin;
    bool softRestart;
    WiFiConfig_t newWiFi;

    unsigned long prevMillis;
    unsigned long prevMillisSignal;
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
            if (this->netConnected()) {
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
            if (this->netConnected()) {
                Base::run();

                this->getSignalQuality();
            }
            else {
                ERaWatchdogFeed();

                Base::onDisconnected();

                ERaWatchdogFeed();

                ERaState::set(StateT::STATE_DISCONNECTED);
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

        if (ssid.length()) {
            CopyToArray(ssid, ERaConfig.ssid);
            CopyToArray(pass, ERaConfig.pass);
        }

        if (backupSsid.length()) {
            ERaConfig.hasBackup = true;
            CopyToArray(backupSsid, ERaConfig.backupSSID);
            CopyToArray(backupPass, ERaConfig.backupPass);
        } else {
            ERaConfig.hasBackup = false;
        }

        if (token.length()) {
            CopyToArray(token, ERaConfig.token);
        }
        else if (!strlen(ERaConfig.token) ||
                ERaStrCmp(ERaConfig.token, ERaDefault.token)) {
            CopyToArray(imei, ERaConfig.token);
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
        else if (!ERaStrCmp(ERaConfig.token, ERaDefault.token)) {
            CopyToArray(ERaConfig.token, ERaConfig.username);
        }
        else {
            CopyToArray(imei, ERaConfig.username);
        }
        if (password.length()) {
            CopyToArray(password, ERaConfig.password);
        }
        else if (!ERaStrCmp(ERaConfig.token, ERaDefault.token)) {
            CopyToArray(ERaConfig.token, ERaConfig.password);
        }
        else {
            CopyToArray(imei, ERaConfig.password);
        }

        content = ERA_F(R"json({"status":"ok","message":"Connecting wifi..."})json");
        ERA_FORCE_UNUSED(content);
        if (ssid.length()) {
            // udpERa.send(content.c_str());
            ERaDelay(100);
            /* TODO: send first time */
            udpERa.sendBoardInfo();
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_UDP, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_PNP, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
            ERaState::set(StateT::STATE_SWITCH_TO_AP_STA);
        }
    });

    udpERa.on("connect/token", []() {
        String token = udpERa.arg("token").c_str();

        if (token.length()) {
            CopyToArray(token, ERaConfig.token);
        }
        else if (!strlen(ERaConfig.token) ||
                ERaStrCmp(ERaConfig.token, ERaDefault.token)) {
            CopyToArray(imei, ERaConfig.token);
        }
        if (!ERaStrCmp(ERaConfig.token, ERaDefault.token)) {
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
        String content;
        int nets {0};
        ERaWatchdogFeed();
        TinyGsm* modem = ::getTinyModem();
        if (modem != nullptr) {
            nets = modem->scanNetworks();
        }
        ERaWatchdogFeed();
        ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Found %d wifi"), nets);

        if (nets <= 0) {
            udpERa.send("[]");
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
            udpERa.send("[]");
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
}

template <class Transport>
void ERaPnP<Transport>::configInit() {
    this->configLoad();
    if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
        ERaState::set(StateT::STATE_CONNECTING_NETWORK);
    }
    else if (this->netConnected()) {
        ERaState::set(StateT::STATE_CONNECTING_CLOUD);
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
    Base::ERaApi::getFlash().readFlash("config", &ERaConfig, sizeof(ERaConfig));
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
void ERaPnP<Transport>::configMode() {
    this->configApi();

    ERA_LOG(TAG, ERA_PSTR("Config mode"));

    ERaWatchdogFeed();
    String networks = this->scanNetworks();
    ERaWatchdogFeed();

    udpERa.setERaORG(Base::getERaORG());
    udpERa.setERaModel(Base::getERaModel());
    udpERa.begin((this->authToken == nullptr) ? ERaConfig.token : this->authToken);
    this->modem->beginServer(80);

    MillisTime_t tick = ERaMillis();
    while (ERaState::is(StateT::STATE_WAIT_CONFIG) || ERaState::is(StateT::STATE_CONFIGURING)) {
        ERaDelay(10);
        udpERa.run();
        this->runServer(networks);
        ERaWatchdogFeed();
        Base::appLoop();
        ERaWatchdogFeed();
        if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
            if (!ERaRemainingTime(tick, WIFI_NET_CHECK_TIMEOUT)) {
                ERaState::set(StateT::STATE_SWITCH_TO_STA);
                break;
            }
        }
    }

    this->waitHandle(networks, 1000UL);

    ERaDelay(1000);
    this->modem->endServer();
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
                String response = "200 OK";
                String content = "";
                String contentType = "text/html";

                switch (req) {
                    case PnPRequestT::PNP_REQ_MAX: {
                        content = ERA_FPSTR(webIndex);
                        content += ERA_FPSTR(webScript);
                        content += ERA_FPSTR(webStyle);
                        WebProcessor(content);
                    }
                        break;
                    case PnPRequestT::PNP_REQ_CONFIG: {
                        ERA_LOG(ERA_PSTR("Config"), ERA_PSTR("Configuration..."));
                        String ssid = this->urlFindArg(configLine, "ssid");
                        if (!ssid.length()) {
                            ssid = this->bodyFindArg(contentConfig, "ssid");
                        }
                        String pass = this->urlFindArg(configLine, "pass");
                        if (!pass.length()) {
                            pass = this->bodyFindArg(contentConfig, "pass");
                        }
                        String backupSsid = this->urlFindArg(configLine, "backup_ssid");
                        if (!backupSsid.length()) {
                            backupSsid = this->bodyFindArg(contentConfig, "backup_ssid");
                        }
                        String backupPass = this->urlFindArg(configLine, "backup_pass");
                        if (!backupPass.length()) {
                            backupPass = this->bodyFindArg(contentConfig, "backup_pass");
                        }

                        String token = this->urlFindArg(configLine, "token");
                        if (!token.length()) {
                            token = this->bodyFindArg(contentConfig, "token");
                        }
                        String host = this->urlFindArg(configLine, "host");
                        if (!host.length()) {
                            host = this->bodyFindArg(contentConfig, "host");
                        }
                        String port = this->urlFindArg(configLine, "port");
                        if (!port.length()) {
                            port = this->bodyFindArg(contentConfig, "port");
                        }
                        String username = this->urlFindArg(configLine, "username");
                        if (!username.length()) {
                            username = this->bodyFindArg(contentConfig, "username");
                        }
                        String password = this->urlFindArg(configLine, "password");
                        if (!password.length()) {
                            password = this->bodyFindArg(contentConfig, "password");
                        }

                        if (ssid.length()) {
                            CopyToArray(ssid, ERaConfig.ssid);
                            CopyToArray(pass, ERaConfig.pass);
                        }

                        if (backupSsid.length()) {
                            ERaConfig.hasBackup = true;
                            CopyToArray(backupSsid, ERaConfig.backupSSID);
                            CopyToArray(backupPass, ERaConfig.backupPass);
                        } else {
                            ERaConfig.hasBackup = false;
                        }

                        if (token.length()) {
                            CopyToArray(token, ERaConfig.token);
                        }
                        else if (!strlen(ERaConfig.token) ||
                                ERaStrCmp(ERaConfig.token, ERaDefault.token)) {
                            CopyToArray(imei, ERaConfig.token);
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
                        else if (!ERaStrCmp(ERaConfig.token, ERaDefault.token)) {
                            CopyToArray(ERaConfig.token, ERaConfig.username);
                        }
                        else {
                            CopyToArray(imei, ERaConfig.username);
                        }
                        if (password.length()) {
                            CopyToArray(password, ERaConfig.password);
                        }
                        else if (!ERaStrCmp(ERaConfig.token, ERaDefault.token)) {
                            CopyToArray(ERaConfig.token, ERaConfig.password);
                        }
                        else {
                            CopyToArray(imei, ERaConfig.password);
                        }

                        contentType = "application/json";
                        content = ERA_F(R"json({"status":"ok","message":"Connecting wifi..."})json");
                        if (ssid.length()) {
                            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_PNP, true);
                            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
                            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
                            ERaState::set(StateT::STATE_SWITCH_TO_STA);
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
                                contentType = "application/json";
                                free(ptr);
                            }
                            cJSON_Delete(root);
                            root = nullptr;
                            ptr = nullptr;
                        }
                        break;
                    }
                    case PnPRequestT::PNP_REQ_SCAN_WIFI: {
                        ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Scanning wifi..."));
                        contentType = "application/json";
                        content = networks;
                    }
                        break;
                    case PnPRequestT::PNP_REQ_RESET: {
                        contentType = "application/json";
                        content = ERA_F(R"json({"status":"ok","message":"Configuration reset"})json");
                        ERaState::set(StateT::STATE_RESET_CONFIG);
                    }
                        break;
                    case PnPRequestT::PNP_REQ_REBOOT: {
                        contentType = "application/json";
                        content = ERA_F(R"json({"status":"ok","message":"Rebooting..."})json");
                        ERaState::set(StateT::STATE_REBOOT);
                    }
                    default:
                        break;
                }

                hasBody = false;

                client.println("HTTP/1.1 " + response);
                client.println("Content-type:" + contentType);
                client.println();
                client.println(content);
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

        if (currentLine.indexOf("GET /api/board_info.json") >= 0) {
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
                        ERA_LOG(TAG, ERA_PSTR("Response timeout"));
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
    }

    client.stop();
}

template <class Transport>
String ERaPnP<Transport>::scanNetworks() {
    ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Scanning wifi..."));
    String content;
    int nets = this->modem->scanNetworks();
    ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Found %d wifi"), nets);

    if (nets <= 0) {
        return "[]";
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

    cJSON* root = cJSON_CreateArray();
    if (root != nullptr) {
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
            cJSON_AddNumberToObject(item, "channel", this->modem->channel(id));
            this->addEncryptionType(id, item);
            cJSON_AddItemToArray(root, item);
            item = nullptr;
            if(++limit >= WIFI_SCAN_MAX) {
                break;
            }
        }
        char* ptr = cJSON_PrintUnformatted(root);
        if (ptr != nullptr) {
            content = ptr;
            free(ptr);
        }
        cJSON_Delete(root);
        root = nullptr;
        ptr = nullptr;
    }
    return content;
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
            cJSON_AddStringToObject(item, "encryption", "WPA");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WPA2_PSK:
            cJSON_AddStringToObject(item, "encryption", "WPA2");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WPA_WPA2_PSK:
            cJSON_AddStringToObject(item, "encryption", "WPA/WPA2");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WPA2_ENTERPRISE:
            cJSON_AddStringToObject(item, "encryption", "WPA2/ENTERPRISE");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WPA3_PSK:
            cJSON_AddStringToObject(item, "encryption", "WPA3");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WPA2_WPA3_PSK:
            cJSON_AddStringToObject(item, "encryption", "WPA2/WPA3");
            break;
        case TinyWiFiEncTypeT::WIFI_ENC_TYPE_WAPI_PSK:
            cJSON_AddStringToObject(item, "encryption", "WAPI");
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
            } else {
                decodedChar = encodedChar;
            }
        }
        decoded += decodedChar;
    }
    return decoded;
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
        data = item->valueint;
    }
    else if (cJSON_IsBool(item)) {
        data = item->valueint;
    }

    cJSON_Delete(root);
    root = nullptr;
    item = nullptr;
    return data;
}

template <class Transport>
void ERaPnP<Transport>::connectNetwork() {
    this->connectNetwork(ERaConfig.ssid, ERaConfig.pass);
    this->connectWiFiBackup();
    if (this->netConnected()) {
        /* Udp */
        if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_UDP)) {
            String content = ERA_F(R"json({"status":"ok","message":"Connected to WiFi"})json");
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
        return;
    }
    else {
        /* Udp */
        if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_UDP)) {
            String content = ERA_F(R"json({"status":"error","message":"Connect WiFi failed"})json");
            udpERa.send(content.c_str());
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_UDP, false);
        }
        /* Udp */
        ERaState::set(StateT::STATE_SWITCH_TO_AP);
        ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
    }
}

template <class Transport>
void ERaPnP<Transport>::connectNewNetwork() {
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
    if (this->netConnected()) {
        ClearArray(ERaConfig.ssid);
        ClearArray(ERaConfig.pass);
        CopyToArray(this->newWiFi.ssid, ERaConfig.ssid);
        CopyToArray(this->newWiFi.pass, ERaConfig.pass);
        ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
        ERaState::set(StateT::STATE_CONNECTING_CLOUD);
    }
    else {
        ERaState::set(StateT::STATE_CONNECTING_NETWORK);
    }
}

template <class Transport>
void ERaPnP<Transport>::connectCloud() {
    ERA_LOG(TAG, ERA_PSTR("Connecting cloud..."));
    if (this->modem != nullptr) {
        this->config(*this->modem, ERaConfig.token, ERaConfig.host,
                    ERaConfig.port, ERaConfig.username, ERaConfig.password);
    }
    if (Base::connect()) {
        ERaConfig.connected = true;
        this->configSave();
        ERaOptConnected(this);
        ERaState::set(StateT::STATE_CONNECTED);
    }
    else if (Base::isConfigMode()) {
        return;
    }
    else if ((ERaConfig.connected || ERaConfig.forceSave) &&
        this->getTransp().connectionDenied()) {
        ERaState::set(StateT::STATE_SWITCH_TO_AP);
        ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
    }
    else {
        ERaState::set(StateT::STATE_CONNECTING_NETWORK);
    }
}

template <class Transport>
bool ERaPnP<Transport>::connectNetwork(const char* ssid, const char* pass) {
    if (this->netConnected()) {
        return true;
    }
    if (!strlen(ssid)) {
        return false;
    }

    MillisTime_t startMillis = ERaMillis();
    while (!this->connectWiFi(ssid, pass)) {
        ERaDelay(500);
        Base::appLoop();
        ERaWatchdogFeed();
        if (Base::isConfigMode() ||
            !ERaRemainingTime(startMillis, WIFI_NET_CONNECT_TIMEOUT)) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Connect %s timeout"), ssid);
            return false;
        }
    }
    this->getTransp().setSSID(ssid);
    return true;
}

template <class Transport>
bool ERaPnP<Transport>::connectWiFi(const char* ssid, const char* pass) {
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

    ERaWatchdogFeed();

    ERA_LOG(TAG, ERA_PSTR("Waiting for network..."));
    if (!this->modem->waitForNetwork()) {
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
    if (ERaStrCmp(ERaConfig.ssid, ssid)) {
        return;
    }

    ClearStruct(this->newWiFi);
    CopyToArray(ssid, this->newWiFi.ssid);
    if (pass != nullptr) {
        CopyToArray(pass, this->newWiFi.pass);
    }
    ERaState::set(StateT::STATE_CONNECTING_NEW_NETWORK);
}

template <class Transport>
void ERaPnP<Transport>::connectWiFiBackup() {
    if (!ERaConfig.hasBackup) {
        return;
    }
    this->connectNetwork(ERaConfig.backupSSID, ERaConfig.backupPass);
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
template <int size>
void ERaPnP<Transport>::getWiFiName(char(&ptr)[size], bool withPrefix) {
    String mac = this->modem->BSSID();
    mac.replace(":", "");
    ClearArray(ptr);
    if (withPrefix) {
        FormatString(ptr, "%s.%s.%s", Base::getERaORG(), Base::getERaModel(), mac.c_str());
    }
    else {
        FormatString(ptr, "%s.%s", Base::getERaORG(), mac.c_str());
    }
    ERaToLowerCase(ptr);
}

template <class Transport>
template <int size>
void ERaPnP<Transport>::getImeiChip(char(&ptr)[size]) {
    String mac = this->modem->BSSID();
    mac.replace(":", "");
    ClearArray(ptr);
#if defined(ERA_AUTH_TOKEN)
    FormatString(ptr, ERA_AUTH_TOKEN);
#else
    if ((this->authToken != nullptr) && strlen(this->authToken)) {
        FormatString(ptr, this->authToken);
    }
    else {
        FormatString(ptr, "ERA-%s", mac.c_str());
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
    int16_t signal = ::getRSSI();

    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
    cJSON_AddStringToObject(root, INFO_BOARD_ID, this->thisProto().getBoardID());
    cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().getAuth());
    cJSON_AddStringToObject(root, INFO_BUILD_DATE, BUILD_DATE_TIME);
    cJSON_AddStringToObject(root, INFO_VERSION, ERA_VERSION);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_PNP));
    cJSON_AddStringToObject(root, INFO_NETWORK_PROTOCOL, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_SSID, ((this->thisProto().getTransp().getSSID() == nullptr) ?
                                            ::getSSID().c_str() : this->thisProto().getTransp().getSSID()));
    cJSON_AddStringToObject(root, INFO_BSSID, ::getBSSID().c_str());
    cJSON_AddNumberToObject(root, INFO_RSSI, signal);
    cJSON_AddNumberToObject(root, INFO_SIGNAL_STRENGTH, SignalToPercentage(signal));
    cJSON_AddStringToObject(root, INFO_MAC, ::getMAC().c_str());
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ::getLocalIP().c_str());
    cJSON_AddNumberToObject(root, INFO_SSL, ERaInfoSSL(ERaConfig.port));
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().getTransp().getPing());
    cJSON_AddNumberToObject(root, INFO_FREE_RAM, ERaFreeRam());

#if defined(ERA_RESET_REASON)
    cJSON_AddStringToObject(root, INFO_RESET_REASON, SystemGetResetReason().c_str());
#endif

    /* Override info */
    ERaInfo(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
    int16_t signal = this->thisProto().getTransp().getSignalQuality();

    cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
    cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
    cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
    cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
    cJSON_AddNumberToObject(root, INFO_MB_RSSI, signal);
    cJSON_AddNumberToObject(root, INFO_MB_SIGNAL_STRENGTH, SignalToPercentage(signal));
    cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ((this->thisProto().getTransp().getSSID() == nullptr) ?
                                                    ::getSSID().c_str() : this->thisProto().getTransp().getSSID()));

    /* Override modbus info */
    ERaModbusInfo(root);
}

#endif /* INC_ERA_PNP_ARDUINO_HPP_ */
