#ifndef INC_ERA_PNP_ARDUINO_HPP_
#define INC_ERA_PNP_ARDUINO_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "WiFi"
#endif

#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

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
    static ERaConfig_t  ERaConfig {};
#else
    #define ERaConfig   StaticHelper<ERaConfig_t>::instance()
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

#if defined(ESP32) || defined(ESP8266)

    static inline
    String getSSID() {
        return WiFi.SSID();
    }

    static inline
    String getSSID(uint8_t networkItem) {
        return WiFi.SSID(networkItem);
    }

    static inline
    String getBSSID() {
        return WiFi.BSSIDstr();
    }

    static inline
    String getBSSID(uint8_t networkItem) {
        return WiFi.BSSIDstr(networkItem);
    }

    static inline
    String getMAC() {
        return WiFi.macAddress();
    }

    static inline
    String getLocalIP() {
        return WiFi.localIP().toString();
    }

    static inline
    String getSoftAPIP() {
        return WiFi.softAPIP().toString();
    }

#else

    static inline
    String getSSID() {
        return String(WiFi.SSID());
    }

    static inline
    String getSSID(uint8_t networkItem) {
        return String(WiFi.SSID(networkItem));
    }

    static inline
    String getBSSID() {
        char mac[20] {0};
        uint8_t bssid[6] {0};
        WiFi.BSSID(bssid);
        FormatString(mac, "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1],
                                                        bssid[2], bssid[3],
                                                        bssid[4], bssid[5]);
        return String(mac);
    }

#if !defined(RTL8722DM) &&      \
    !defined(ARDUINO_AMEBA) &&  \
    !defined(ARDUINO_MBED_HAS_WIFI)
    static inline
    String getBSSID(uint8_t networkItem) {
        char mac[20] {0};
        uint8_t bssid[6] {0};
        WiFi.BSSID(networkItem, bssid);
        FormatString(mac, "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1],
                                                        bssid[2], bssid[3],
                                                        bssid[4], bssid[5]);
        return String(mac);
    }
#endif

    static inline
    String getMAC() {
        char mac[20] {0};
        uint8_t macAddr[6] {0};
        WiFi.macAddress(macAddr);
        FormatString(mac, "%02X:%02X:%02X:%02X:%02X:%02X", macAddr[0], macAddr[1],
                                                        macAddr[2], macAddr[3],
                                                        macAddr[4], macAddr[5]);
        return String(mac);
    }

    static inline
    String getLocalIP() {
        char ip[20] {0};
        IPAddress localIP = WiFi.localIP();
        FormatString(ip, "%u.%u.%u.%u", localIP[0], localIP[1], localIP[2], localIP[3]);
        return String(ip);
    }

    static inline
    String getSoftAPIP() {
        char ip[20] {0};
        IPAddress softAPIP = WIFI_AP_IP;
        FormatString(ip, "%u.%u.%u.%u", softAPIP[0], softAPIP[1], softAPIP[2], softAPIP[3]);
        return String(ip);
    }

#endif

#include <Network/ERaUdpArduino.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static WiFiUDP Udp;
    static ERaUdp<WiFiUDP> udpERa(Udp);
#else
    #define Udp     StaticHelper<WiFiUDP>::instance()
    #define udpERa  StaticRefHelper<ERaUdp<WiFiUDP>, WiFiUDP>::instance(Udp)
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
        , authToken(nullptr)
        , persistent(false)
        , server(80)
        , newWiFi {}
        , prevMillis(0UL)
    {}
    ~ERaPnP()
    {}

    void config(const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::begin(auth);
        this->getTransp().config(host, port, username, password);
    }

    void begin(const char* auth,
                const char* ssid,
                const char* pass,
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
#if defined(ESP32) ||   \
    defined(ESP8266)
        WiFi.mode(WIFI_STA);
#endif
        Base::init();
        this->config(auth, host, port, username, password);

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

        this->connectWiFi(ERaConfig.ssid, ERaConfig.pass);
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
                const char* ssid,
                const char* pass,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, ssid, pass,
                    host, port, auth,
                    auth);
    }

    void begin(const char* ssid,
                const char* pass) {
        this->begin(ERA_AUTHENTICATION_TOKEN, ssid, pass,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
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

    void begin(const char* auth = nullptr);
    void run();

protected:
private:
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
    void connectWiFi(const char* ssid, const char* pass);
    void connectNewWiFi(const char* ssid, const char* pass) override;
    void connectWiFiBackup();
    void switchToAP();
    void switchToSTA();
    void switchToAPSTA();

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

        _connected = (WiFi.status() == WL_CONNECTED);
        return _connected;
    }

    const char* authToken;
    const char  channelAP[2] = "1";
    bool persistent;
    WiFiServer  server;
    WiFiConfig_t newWiFi;

    unsigned long prevMillis;
};

template <class Transport>
void ERaPnP<Transport>::begin(const char* auth) {
    this->authToken = auth;

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
        int nets = WiFi.scanNetworks();
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
                if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
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
                cJSON_AddNumberToObject(item, "rssi", WiFi.RSSI(id));
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

    this->server.begin();
    udpERa.setERaORG(Base::getERaORG());
    udpERa.setERaModel(Base::getERaModel());
    udpERa.begin((this->authToken == nullptr) ? ERaConfig.token : this->authToken);

    MillisTime_t tick = ERaMillis();
    while (ERaState::is(StateT::STATE_WAIT_CONFIG) || ERaState::is(StateT::STATE_CONFIGURING)) {
        ERaDelay(10);
        udpERa.run();
        this->runServer(networks);
        ERaWatchdogFeed();
        Base::appLoop();
        ERaWatchdogFeed();
#if defined(ESP32) ||           \
    defined(ESP8266)
        if (ERaState::is(StateT::STATE_CONFIGURING) && !WiFi.softAPgetStationNum()) {
            ERaState::set(StateT::STATE_WAIT_CONFIG);
        }
#elif !defined(RTL8722DM) &&    \
    !defined(ARDUINO_AMEBA)
        if (ERaState::is(StateT::STATE_CONFIGURING) && (WiFi.status() != WL_AP_CONNECTED)) {
            ERaState::set(StateT::STATE_WAIT_CONFIG);
        }
#endif
        if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
            if (!ERaRemainingTime(tick, WIFI_NET_CHECK_TIMEOUT)) {
                ERaState::set(StateT::STATE_SWITCH_TO_STA);
                break;
            }
        }
    }

    this->waitHandle(networks, 1000UL);

    ERaDelay(1000);

#if defined(ERA_ARDUINO_WIFI_ESP_AT)
    this->server.end();
#elif !defined(ERA_ARDUINO_WIFI)
    this->server.stop();
#endif
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
    WiFiClient client = this->server.available();
    if (!client) {
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
    }

    client.stop();
}

template <class Transport>
String ERaPnP<Transport>::scanNetworks() {
    ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Scanning wifi..."));
    String content;
    int nets = WiFi.scanNetworks();
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
            if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
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
#if !defined(RTL8722DM) &&      \
    !defined(ARDUINO_AMEBA) &&  \
    !defined(ARDUINO_MBED_HAS_WIFI)
            cJSON_AddStringToObject(item, "bssid", ::getBSSID(id).c_str());
            cJSON_AddNumberToObject(item, "channel", WiFi.channel(id));
#endif
            cJSON_AddNumberToObject(item, "rssi", WiFi.RSSI(id));
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
    char ssidAP[64] {0};
    this->getWiFiName(ssidAP);
    this->replace(ssidAP, '.', '-');

#if !defined(ERA_ARDUINO_WIFI) ||       \
    defined(ERA_ARDUINO_WIFI_NINA) ||   \
    defined(ERA_ARDUINO_WIFI_ESP_AT)
    WiFi.setHostname(ssidAP);
#endif

    this->connectWiFi(ERaConfig.ssid, ERaConfig.pass);
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
#if defined(ESP32) ||   \
    defined(ESP8266)
        WiFi.mode(WIFI_STA);
#endif
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

    WiFi.disconnect();

    /* Waiting WiFi disconnect */
    MillisTime_t startMillis = ERaMillis();
    while (WiFi.status() == WL_CONNECTED) {
        ERaDelay(10);
        ERaWatchdogFeed();
        if (!ERaRemainingTime(startMillis, 5000)) {
            ERaState::set(StateT::STATE_CONNECTING_NETWORK);
            return;
        }
    }

    this->connectWiFi(this->newWiFi.ssid, this->newWiFi.pass);
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
    this->config(ERaConfig.token, ERaConfig.host, ERaConfig.port,
                ERaConfig.username, ERaConfig.password);
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
void ERaPnP<Transport>::connectWiFi(const char* ssid, const char* pass) {
    if (this->netConnected()) {
        return;
    }
    if (!strlen(ssid)) {
        return;
    }

    int status = WiFi.status();

    if (status == WL_NO_SHIELD) {
        ERA_LOG_ERROR(TAG, ERA_PSTR("WiFi module not found"));
    }

    MillisTime_t started = ERaMillis();
    while (status != WL_CONNECTED) {
        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Connecting to %s..."), ssid);
        if (pass && strlen(pass)) {
            WiFi.begin((char*)ssid, (char*)pass);
        } else {
#if defined(ARDUINO_MBED_HAS_WIFI)
            WiFi.begin((char*)ssid, (char*)"");
#else
            WiFi.begin((char*)ssid);
#endif
        }

        ERaWatchdogFeed();

        MillisTime_t startMillis = ERaMillis();
        while (status != WL_CONNECTED) {
            ERaDelay(500);
            ERaWatchdogFeed();
            status = WiFi.status();
            Base::appLoop();
            ERaWatchdogFeed();
            if (Base::isConfigMode() ||
                !ERaRemainingTime(started, WIFI_NET_CONNECT_TIMEOUT)) {
                WiFi.disconnect();
                ERA_LOG_ERROR(TAG, ERA_PSTR("Connect %s failed"), ssid);
                return;
            }
            else if (!ERaRemainingTime(startMillis, 20000)) {
                WiFi.disconnect();
                ERA_LOG_ERROR(TAG, ERA_PSTR("Connect %s timeout"), ssid);
                break;
            }
        }
    }
    this->getTransp().setSSID(ssid);
    ERA_LOG(TAG, ERA_PSTR("Connected to WiFi"));
    ERA_LOG(TAG, ERA_PSTR("IP: %s"), ::getLocalIP().c_str());
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
    this->connectWiFi(ERaConfig.backupSSID, ERaConfig.backupPass);
}

template <class Transport>
void ERaPnP<Transport>::switchToAP() {
    ERA_LOG(TAG, ERA_PSTR("Switching to AP..."));

    char ssidAP[64] {0};
    this->getWiFiName(ssidAP);

    if (WiFi.status() == WL_NO_SHIELD) {
        ERA_LOG_ERROR(TAG, ERA_PSTR("WiFi module not found"));
    }
    ERaWatchdogFeed();
    ERaDelay(100);
#if defined(ESP32) ||           \
    defined(ESP8266)
    WiFi.mode(WIFI_OFF);
    ERaDelay(100);
    WiFi.mode(WIFI_AP);
    ERaDelay(2000);
    WiFi.softAPConfig(WIFI_AP_IP, WIFI_AP_IP, WIFI_AP_Subnet);
    WiFi.softAP(ssidAP, WIFI_AP_PASS);
#elif defined(RTL8722DM) ||     \
    defined(ARDUINO_AMEBA)
    WiFi.config(WIFI_AP_IP);
    WiFi.apbegin(ssidAP, WIFI_AP_PASS, (char*)this->channelAP, 0);
#else
    #if defined(ERA_ARDUINO_WIFI_ESP_AT)
        WiFi.configureAP(WIFI_AP_IP);
    #else
        WiFi.config(WIFI_AP_IP);
    #endif
    WiFi.beginAP(ssidAP, WIFI_AP_PASS);
#endif

    ERaDelay(500);
    ERaWatchdogFeed();

    ERA_LOG(TAG, ERA_PSTR("AP SSID: %s"), ssidAP);
    ERA_LOG(TAG, ERA_PSTR("AP IP: %s"), ::getSoftAPIP().c_str());

    ERaState::set(StateT::STATE_WAIT_CONFIG);
}

template <class Transport>
void ERaPnP<Transport>::switchToSTA() {
    ERA_LOG(TAG, ERA_PSTR("Switching to STA..."));

#if defined(ESP32) ||   \
    defined(ESP8266)
    ERaDelay(1000);
    WiFi.mode(WIFI_OFF);
    ERaDelay(100);
    WiFi.mode(WIFI_STA);
#endif
    ERaWatchdogFeed();

    ERaState::set(StateT::STATE_CONNECTING_NETWORK);
}

template <class Transport>
void ERaPnP<Transport>::switchToAPSTA() {
    ERA_LOG(TAG, ERA_PSTR("Switching to AP STA..."));

#if defined(ESP32) ||   \
    defined(ESP8266)
    ERaDelay(100);
    WiFi.mode(WIFI_AP_STA);
#endif
    ERaWatchdogFeed();

    ERaState::set(StateT::STATE_CONNECTING_NETWORK);
}

template <class Transport>
template <int size>
void ERaPnP<Transport>::getWiFiName(char(&ptr)[size], bool withPrefix) {
    char mac[20] {0};
    uint8_t macAddr[6] {0};
#if defined(RTL8722DM) ||     \
    defined(ARDUINO_AMEBA)
    WiFi.BSSID(macAddr);
#else
    WiFi.macAddress(macAddr);
#endif
    FormatString(mac, "%02x%02x%02x%02x%02x%02x", macAddr[0], macAddr[1],
                                                    macAddr[2], macAddr[3],
                                                    macAddr[4], macAddr[5]);
    ClearArray(ptr);
    if (withPrefix) {
        FormatString(ptr, "%s.%s.%s", Base::getERaORG(), Base::getERaModel(), mac);
    }
    else {
        FormatString(ptr, "%s.%s", Base::getERaORG(), mac);
    }
    ERaToLowerCase(ptr);
}

template <class Transport>
template <int size>
void ERaPnP<Transport>::getImeiChip(char(&ptr)[size]) {
    char mac[20] {0};
    uint8_t macAddr[6] {0};
#if defined(RTL8722DM) ||     \
    defined(ARDUINO_AMEBA)
    WiFi.BSSID(macAddr);
#else
    WiFi.macAddress(macAddr);
#endif
    FormatString(mac, "%02x%02x%02x%02x%02x%02x", macAddr[0], macAddr[1],
                                                    macAddr[2], macAddr[3],
                                                    macAddr[4], macAddr[5]);
    ClearArray(ptr);
#if defined(ERA_AUTH_TOKEN)
    FormatString(ptr, ERA_AUTH_TOKEN);
#else
    if ((this->authToken != nullptr) && strlen(this->authToken)) {
        FormatString(ptr, this->authToken);
    }
    else {
        FormatString(ptr, "ERA-%s", mac);
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
    int16_t signal = WiFi.RSSI();

    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
    cJSON_AddStringToObject(root, INFO_BOARD_ID, this->thisProto().getBoardID());
    cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().getAuth());
    cJSON_AddStringToObject(root, INFO_BUILD_DATE, BUILD_DATE_TIME);
    cJSON_AddStringToObject(root, INFO_VERSION, ERA_VERSION);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_PNP));
    cJSON_AddStringToObject(root, INFO_NETWORK_PROTOCOL, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_SSID, ::getSSID().c_str());
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
    int16_t signal = WiFi.RSSI();

#if defined(ESP32)
    cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, static_cast<uint16_t>(temperatureRead() * 100.0f));
#else
    cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
#endif
    cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
    cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
    cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
    cJSON_AddNumberToObject(root, INFO_MB_RSSI, signal);
    cJSON_AddNumberToObject(root, INFO_MB_SIGNAL_STRENGTH, SignalToPercentage(signal));
    cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ::getSSID().c_str());

    /* Override modbus info */
    ERaModbusInfo(root);
}

#endif /* INC_ERA_PNP_ARDUINO_HPP_ */
