#ifndef INC_ERA_PNP_ESP32_HPP_
#define INC_ERA_PNP_ESP32_HPP_

#if !defined(ERA_PROTO_TYPE)
    #define ERA_PROTO_TYPE            "WiFi"
#endif

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ERa/ERaProtocol.hpp>
#include <ERa/ERaApiEsp32.hpp>
#include <MQTT/ERaMqtt.hpp>
#include <Modbus/ERaModbusEsp32.hpp>
#include <Zigbee/ERaZigbeeEsp32.hpp>
#include <Storage/ERaFlashEsp32.hpp>
#include <PnP/ERaWeb.hpp>

#define CONFIG_AP_URL                 "era.setup"

#define WIFI_SCAN_MAX                 15
#define WIFI_SCAN_TIMEOUT             20000
#define WIFI_SCAN_CHANNEL_TIMEOUT     150
#define WIFI_NET_CONNECT_TIMEOUT      60000
#define WIFI_CLOUD_CONNECT_TIMEOUT    60000
#define WIFI_NET_CHECK_TIMEOUT        (WIFI_NET_CONNECT_TIMEOUT * 3)
#define WIFI_SCAN_INTERVAL            (WIFI_NET_CONNECT_TIMEOUT * 3)
#define WIFI_AP_PASS                  "Eoh@2021"
#define WIFI_AP_IP                    IPAddress(192, 168, 27, 1)
#define WIFI_AP_Subnet                IPAddress(255, 255, 255, 0)

#define WIFI_SPEC_SSID                "era.spec.ssid"
#define WIFI_SPEC_PASS                "Eoh@2021"

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

static const uint16_t DNS_PORT = 53;
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
    ERA_MQTT_PASSWORD
};

enum ConfigFlagT {
    CONFIG_FLAG_VALID = 0x01,
    CONFIG_FLAG_STORE = 0x02,
    CONFIG_FLAG_API = 0x04,
    CONFIG_FLAG_UDP = 0x08
};

#include <Network/ERaUdpEsp32.hpp>

template <class Transport>
class ERaPnP
    : public ERaProto<Transport, ERaFlash>
{
    typedef std::function<bool(void)> NetworkCallback_t;
    typedef std::function<void(WiFiEvent_t, WiFiEventInfo_t)> WiFiEventCallback_t;

    const char* TAG = "WiFi";
    const char* HOSTNAME = "ERa";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;

public:
    ERaPnP(Transport& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , authToken(nullptr)
        , specSSID(WIFI_SPEC_SSID)
        , specPass(WIFI_SPEC_PASS)
        , hasNetworkCallback(false)
        , prevMillisScan(0UL)
        , server(80)
        , udpERa(Udp)
        , newWiFi {}
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
        WiFi.onEvent(this->wifiCb);
        WiFi.mode(WIFI_STA);
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectWiFi(ssid, pass);
        if (this->netConnected()) {
            CopyToArray(ssid, ERaConfig.ssid);
            CopyToArray(pass, ERaConfig.pass);
            CopyToArray(auth, ERaConfig.token);
            CopyToArray(host, ERaConfig.host);
            ERaConfig.port = port;
            CopyToArray(username, ERaConfig.username);
            CopyToArray(password, ERaConfig.password);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            if (Base::connect()) {
                ERaOptConnected(this);
                ERaState::set(StateT::STATE_CONNECTED);
            }
            else {
                ERaState::set(StateT::STATE_CONNECTING_CLOUD);
            }
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

    void setNetworkCallbacks(NetworkCallback_t callback) {
        this->netCb = callback;
        this->hasNetworkCallback = true;
    }

    void setGsmCallbacks(NetworkCallback_t callback) {
        this->gsmCb = callback;
    }

    void setWiFiCallbacks(WiFiEventCallback_t callback) {
        this->wifiCb = callback;
    }

    void setSpecSSID(const char* ssid, const char* pass = "") {
        this->specSSID = ssid;
        this->specPass = pass;
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
    void waitHandle(unsigned long ms);
    int scanNetworks();
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

    bool wifiConnected() const {
        return (WiFi.status() == WL_CONNECTED);
    }

    bool netConnected() const {
        return (this->netCb() || (WiFi.status() == WL_CONNECTED) || this->gsmCb());
    }

    void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);

    static bool networkCallbackDummy() {
        return false;
    }

    NetworkCallback_t netCb = this->networkCallbackDummy;
    NetworkCallback_t gsmCb = this->networkCallbackDummy;

	WiFiEventCallback_t wifiCb = [&, this](WiFiEvent_t event, WiFiEventInfo_t info) {
		this->onWiFiEvent(event, info);
	};

    const char* authToken;
    const char* specSSID;
    const char* specPass;
    bool hasNetworkCallback;
    unsigned long prevMillisScan;
    WebServer   server;
    DNSServer   dnsServer;
    WiFiUDP     Udp;
    ERaUdp<WiFiUDP> udpERa;
    WiFiConfig_t    newWiFi;
};

template <class Transport>
void ERaPnP<Transport>::begin(const char* auth) {
    this->authToken = auth;
    WiFi.persistent(false);
    WiFi.enableSTA(true);
    WiFi.onEvent(this->wifiCb);

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
            if (this->netCb() ||
                this->wifiConnected()) {
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

    this->dnsServer.setTTL(300);
    this->dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

    this->server.on("/api/config", HTTP_POST, [&, this]() {
        ERA_LOG(ERA_PSTR("Config"), ERA_PSTR("Configuration..."));
        String ssid = this->server.arg("ssid");
        String pass = this->server.arg("pass");
        String backupSsid = this->server.arg("backup_ssid");
        String backupPass = this->server.arg("backup_pass");

        String token = this->server.arg("token");
        String host = this->server.arg("host");
        String port = this->server.arg("port");
        String username = this->server.arg("username");
        String password = this->server.arg("password");

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
        this->server.send(200, "application/json", content);
        if (ssid.length()) {
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
            ERaState::set(StateT::STATE_SWITCH_TO_STA);
        }
    });

    this->server.on("/api/board_info.json", HTTP_POST, [&, this]() {
        ERA_LOG(ERA_PSTR("Info"), ERA_PSTR("Board info"));
        String content;
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
            cJSON_AddStringToObject(root, INFO_BSSID, WiFi.softAPmacAddress().c_str());
            cJSON_AddStringToObject(root, INFO_MAC, WiFi.macAddress().c_str());
            cJSON_AddStringToObject(root, INFO_LOCAL_IP, WiFi.softAPIP().toString().c_str());
            char* ptr = cJSON_PrintUnformatted(root);
            if (ptr != nullptr) {
                content = ptr;
                free(ptr);
            }
            cJSON_Delete(root);
            root = nullptr;
            ptr = nullptr;
        }
        this->server.send(200, "application/json", content);
    });

    this->server.on("/api/wifi_scan.json", HTTP_POST, [&, this]() {
        ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Scanning wifi..."));
        String content;
        int nets = this->scanNetworks();

        if (nets < 0) {
            this->server.send(200, "application/json", "[]");
            return;
        }

        int indices[(nets ? nets : 1)] {0};
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
            std::vector<std::string> listWifi;
            for (int i = 0; i < nets; ++i) {
                int id = indices[i];
                if (WiFi.SSID(id).isEmpty()) {
                    continue;
                }
                std::vector<std::string>::iterator wifi = std::find_if(listWifi.begin(), listWifi.end(), [id](const std::string& e) {
                    return e == WiFi.SSID(id).c_str();
                });
                if (wifi != listWifi.end()) {
                    continue;
                }
                listWifi.push_back(WiFi.SSID(id).c_str());
                cJSON* item = cJSON_CreateObject();
                if (item == nullptr) {
                    break;
                }
                cJSON_AddStringToObject(item, "ssid", WiFi.SSID(id).c_str());
                cJSON_AddStringToObject(item, "bssid", WiFi.BSSIDstr(id).c_str());
                cJSON_AddNumberToObject(item, "rssi", WiFi.RSSI(id));
                cJSON_AddNumberToObject(item, "channel", WiFi.channel(id));
                switch (WiFi.encryptionType(id)) {
                    case wifi_auth_mode_t::WIFI_AUTH_WEP:
                        cJSON_AddStringToObject(item, "encryption", "WEP");
                        break;
                    case wifi_auth_mode_t::WIFI_AUTH_WPA_PSK:
                        cJSON_AddStringToObject(item, "encryption", "WPA/PSK");
                        break;
                    case wifi_auth_mode_t::WIFI_AUTH_WPA2_PSK:
                        cJSON_AddStringToObject(item, "encryption", "WPA2/PSK");
                        break;
                    case wifi_auth_mode_t::WIFI_AUTH_WPA_WPA2_PSK:
                        cJSON_AddStringToObject(item, "encryption", "WPA/WPA2/PSK");
                        break;
                    case wifi_auth_mode_t::WIFI_AUTH_OPEN:
                        cJSON_AddStringToObject(item, "encryption", "OPEN");
                        break;
                    default:
                        cJSON_AddStringToObject(item, "encryption", "unknown");
                        break;
                }
                cJSON_AddItemToArray(root, item);
                item = nullptr;
                if(++limit >= WIFI_SCAN_MAX) {
                    break;
                }
            }

            /* Add spec SSID */
            if (strlen(this->specSSID)) {
                cJSON* item = cJSON_CreateObject();
                if (item != nullptr) {
                    cJSON_AddStringToObject(item, "ssid", this->specSSID);
                    cJSON_AddStringToObject(item, "bssid", "FE:27:06:95:FE:FE");
                    cJSON_AddNumberToObject(item, "rssi", -27);
                    cJSON_AddNumberToObject(item, "channel", 6);
                    if (strlen(this->specPass)) {
                        cJSON_AddStringToObject(item, "encryption", "WPA2/PSK");
                    }
                    else {
                        cJSON_AddStringToObject(item, "encryption", "OPEN");
                    }
                    cJSON_AddItemToArray(root, item);
                }
                item = nullptr;
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

        this->server.send(200, "application/json", content);
    });

    this->server.on("/api/reset", HTTP_POST, [&, this]() {
        String content = ERA_F(R"json({"status":"ok","message":"Configuration reset"})json");
        this->server.send(200, "application/json", content);
        ERaState::set(StateT::STATE_RESET_CONFIG);
    });

    this->server.on("/api/reboot", HTTP_POST, [&, this]() {
        String content = ERA_F(R"json({"status":"ok","message":"Rebooting..."})json");
        this->server.send(200, "application/json", content);
        ERaState::set(StateT::STATE_REBOOT);
    });

    this->server.on("/", [&, this]() {
        String content = ERA_FPSTR(webIndex);
        content += ERA_FPSTR(webScript);
        content += ERA_FPSTR(webStyle);
        WebProcessor(content);
        this->server.send(200, "text/html", content);
    });

    this->server.on("/network", [&, this]() {
        ERA_LOG(ERA_PSTR("Config"), ERA_PSTR("Configuration..."));
        String ssid = this->server.arg("ssid");
        String pass = this->server.arg("pass");
        String backupSsid = this->server.arg("backup_ssid");
        String backupPass = this->server.arg("backup_pass");

        String token = this->server.arg("token");
        String host = this->server.arg("host");
        String port = this->server.arg("port");
        String username = this->server.arg("username");
        String password = this->server.arg("password");

        bool scanWifi  = this->server.hasArg("scan");
        bool forceSave = this->server.hasArg("save");
        bool hasConnect = this->server.hasArg("connect");

        String content;
        int nets {-1};
        uint8_t limit {0};
        std::vector<std::string> listWifi;

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

        if (ERaStrCmp(ERaConfig.ssid, this->specSSID) &&
            ERaStrCmp(ERaConfig.pass, this->specPass)) {
            ClearArray(ERaConfig.ssid);
            ClearArray(ERaConfig.pass);
        }

        if (scanWifi) {
            nets = this->scanNetworks();
        }

        content = ERA_F("<meta charset='utf-8'>");
        content += ERA_F("<form name=WiFiForm method=post>");
        content += ERA_F("<h1>NETWORK</h1>");
        content += ERA_F("<table border=1 width='100%'>");
        content += ERA_F("<tr><th width='50%' height='30px'>WiFi</th>");
        content += ERA_F("<th width='50%'>WiFi 2</th></tr>");
        content += ERA_F("<tr><td style='text-align:center'>") + String(strlen(ERaConfig.ssid) ? ERaConfig.ssid : "--");
        content += ERA_F("</td><td style='text-align:center'>") + String(strlen(ERaConfig.backupSSID) ? ERaConfig.backupSSID : "--");
        content += ERA_F("</td></tr></table>");

        if (nets >= 0) {
            int indices[(nets ? nets : 1)] {0};
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

            content += ERA_F("<select name=ssid id=ssid width='100%'>");
            content += ERA_F("<option value='' disabled selected>%SELECT_SSID%</option>");
            limit = 0;
            listWifi.clear();
            for (int i = 0; i < nets; ++i) {
                int id = indices[i];
                if (WiFi.SSID(id).isEmpty()) {
                    continue;
                }
                std::vector<std::string>::iterator eachWifi = std::find_if(listWifi.begin(), listWifi.end(), [id](const std::string& e) {
                    return e == WiFi.SSID(id).c_str();
                });
                if (eachWifi != listWifi.end()) {
                    continue;
                }
                listWifi.push_back(WiFi.SSID(id).c_str());
                content += ERA_F("<option value='");
                content += WiFi.SSID(id);
                content += ERA_F("'>");
                content += WiFi.SSID(id);
                content += ERA_F(" (");
                content += WiFi.RSSI(id);
                content += ERA_F(") ");
                content += ((WiFi.encryptionType(id) == wifi_auth_mode_t::WIFI_AUTH_OPEN) ? "" : "*");
                content += ERA_F("</option>");
                if (++limit >= WIFI_SCAN_MAX) {
                    break;
                }
            }

            /* Add spec SSID */
            if (strlen(this->specSSID)) {
                content += ERA_F("<option value='");
                content += this->specSSID;
                content += ERA_F("'>");
                content += this->specSSID;
                content += ERA_F(" (");
                content += -27;
                content += ERA_F(") ");
                content += (!strlen(this->specPass) ? "" : "*");
                content += ERA_F("</option>");
            }

            content += ERA_F("</select>");
        }
        else {
            content += ERA_F("<input name=ssid id=ssid placeholder='%SSID%'>");
        }
        content += ERA_F("<input name=pass id=pass placeholder='%PASSWORD%' type=password>");

        if (nets >= 0) {
            content += ERA_F("<select name=backup_ssid id=backup_ssid width='100%'>");
            content += ERA_F("<option value='' disabled selected>%SELECT_BACKUP_SSID%</option>");
            limit = 0;
            listWifi.clear();
            for (int i = 0; i < nets; ++i) {
                if (WiFi.SSID(i).isEmpty()) {
                    continue;
                }
                std::vector<std::string>::iterator eachWifi = std::find_if(listWifi.begin(), listWifi.end(), [i](const std::string& e) {
                    return e == WiFi.SSID(i).c_str();
                });
                if (eachWifi != listWifi.end()) {
                    continue;
                }
                listWifi.push_back(WiFi.SSID(i).c_str());
                content += ERA_F("<option value='");
                content += WiFi.SSID(i);
                content += ERA_F("'>");
                content += WiFi.SSID(i);
                content += ERA_F(" (");
                content += WiFi.RSSI(i);
                content += ERA_F(") ");
                content += ((WiFi.encryptionType(i) == wifi_auth_mode_t::WIFI_AUTH_OPEN) ? "" : "*");
                content += ERA_F("</option>");
                if (++limit >= WIFI_SCAN_MAX) {
                    break;
                }
            }

            /* Add spec SSID */
            if (strlen(this->specSSID)) {
                content += ERA_F("<option value='");
                content += this->specSSID;
                content += ERA_F("'>");
                content += this->specSSID;
                content += ERA_F(" (");
                content += -27;
                content += ERA_F(") ");
                content += (!strlen(this->specPass) ? "" : "*");
                content += ERA_F("</option>");
            }

            content += ERA_F("</select>");
        }
        else {
            content += ERA_F("<input name=backup_ssid id=backup_ssid placeholder='%BACKUP_SSID%'>");
        }
        content += ERA_F("<input name=backup_pass id=backup_pass placeholder='%BACKUP_PASS%' type=password>");
        content += ERA_F("<input name=host id=host value='");
        content += ERaConfig.host;
        content += ERA_F("'>");
        content += ERA_F("<input name=port id=port value='");
        content += String(ERaConfig.port);
        content += ERA_F("'>");
        content += ERA_F("<input name=token id=token placeholder='%TOKEN%'>");
        content += ERA_F("<input name=username id=username placeholder='%USERNAME%'>");
        content += ERA_F("<input name=password id=password placeholder='%PASSWORD%' type=password>");
        content += ERA_F("<input type=checkbox onclick=togglePass() class=checkbox> %SHOW_PASSWORD%");
        if (!scanWifi) {
            content += ERA_F("<input type=submit name=scan id=scan class=btn value='%SCAN_NETWORK%'>");
        }
        content += ERA_F("<input type=submit name=save id=save class=btn value='%SAVE%'>");
        content += ERA_F("<input type=submit onclick=clicked(event) name=connect id=connect class=btn value='%CONNECT%'>");
        content += ERA_F("<input type=submit formaction='/' class=btn value='%BACK%'></form>");

        content += ERA_FPSTR(webScript);
        content += ERA_FPSTR(webStyle);

        WebProcessor(content);
        this->server.send(200, "text/html", content);
        if (forceSave) {
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
            this->configSave();
            ERaState::set(StateT::STATE_SWITCH_TO_STA);
        }
        else if (ssid.length() && hasConnect) {
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
            ERaState::set(StateT::STATE_SWITCH_TO_STA);
        }
    });

    this->server.on("/network/reset", [&, this]() {
        this->server.sendHeader("Location", "/");
        this->server.send(302, "text/plain", "");
        ERaState::set(StateT::STATE_RESET_CONFIG);
    });

    this->server.on("/network/status", HTTP_POST, [&, this]() {
        String content;
        if (this->wifiConnected()) {
            content += LANGUAGE_CONNECTED(LANGUAGE_EN);
            content += ERA_F(" ");
            content += WiFi.SSID();
        }
        else {
            content += ERA_F("--");
        }
        this->server.send(200, "text/plane", content);
    });

    this->server.on("/network/ip", HTTP_POST, [&, this]() {
        String content;
        if (this->wifiConnected()) {
            content += WiFi.localIP().toString();
        }
        else {
            content += ERA_F("--");
        }
        this->server.send(200, "text/plane", content);
    });

    this->server.on("/network/ssid", HTTP_POST, [&, this]() {
        String content;
        if (this->wifiConnected()) {
            content += WiFi.SSID();
        }
        else {
            content += ERA_F("--");
        }
        this->server.send(200, "text/plane", content);
    });

    this->server.on("/network/rssi", HTTP_POST, [&, this]() {
        String content;
        if (this->wifiConnected()) {
            content += WiFi.RSSI();
        }
        else {
            content += ERA_F("--");
        }
        this->server.send(200, "text/plane", content);
    });

    this->server.on("/network/mac", HTTP_POST, [&, this]() {
        String content;
        content += WiFi.macAddress();
        this->server.send(200, "text/plane", content);
    });

    this->server.on("/mqtt/connected", HTTP_POST, [&, this]() {
        String content;
        content += (Base::connected() ? "online" : "offline");
        this->server.send(200, "text/plane", content);
    });

    this->server.on("/system/memory", HTTP_POST, [&, this]() {
        String content;
        content += ERaFreeRam();
        content += ERA_F(" Bytes");
        this->server.send(200, "text/plane", content);
    });

    this->server.on("/system/uptime", HTTP_POST, [&, this]() {
        String content;
        char upTime[15] {0};
        unsigned long seconds = (ERaMillis() / 1000UL);
        uint8_t days = (seconds / (24 * 3600));
        seconds -= (days * 24 * 3600);
        uint8_t hours = (seconds / 3600);
        seconds -= (hours * 3600);
        uint8_t minutes = (seconds / 60);
        seconds -= (minutes * 60);
        FormatString(upTime, "%02d:%02d:%02d:%02lu", days, hours, minutes, seconds);
        content += upTime;
        this->server.send(200, "text/plane", content);
    });

    this->server.on("/system/token", HTTP_POST, [&, this]() {
        String content;
        if (strlen(ERaConfig.token)) {
#if (ESP_IDF_VERSION_MAJOR < 4)
            content += ERaConfig.token;
            content.remove(ERaMin(strlen(ERaConfig.token), (size_t)4));
#else
            content.concat(ERaConfig.token,
                    ERaMin(strlen(ERaConfig.token), (size_t)4));
#endif
            content += ERA_F("-****-****-****");
        }
        else {
            content += ERA_F("--");
        }
        this->server.send(200, "text/plane", content);
    });

    this->server.on("/system/restart", HTTP_POST, [&, this]() {
        ERaRestart(true);
    });

    this->server.on("/ota/update", HTTP_POST, [&, this]() {
        String content;
        if (Update.hasError()) {
            content = LANGUAGE_UPDATE_FAIL(LanguageWebT::LANGUAGE_EN);
        }
        else {
            content = LANGUAGE_UPDATE_SUCCESS(LanguageWebT::LANGUAGE_EN);
        }
        this->server.send(200, "text/plane", content);
    }, [&, this]() {
        HTTPUpload& upload = this->server.upload();
        switch (upload.status) {
            case HTTPUploadStatus::UPLOAD_FILE_START:
                ERA_LOG("OTA", ERA_PSTR("Update start: %s"), upload.filename.c_str());
                if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                    ERA_LOG_ERROR("OTA", ERA_PSTR("Update can't begin"));
                }
                break;
            case HTTPUploadStatus::UPLOAD_FILE_WRITE:
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                    ERA_LOG_ERROR("OTA", ERA_PSTR("Update write failed"));
                }
                break;
            case HTTPUploadStatus::UPLOAD_FILE_END:
                if (Update.end(true)) {
                    ERA_LOG("OTA", ERA_PSTR("Update successfully. Rebooting!"));
                }
                else {
                    ERA_LOG_ERROR("OTA", ERA_PSTR("Update failed"));
                }
                break;
            default:
                ERA_LOG_ERROR(TAG, ERA_PSTR("Update failed"));
                break;
        }
    });

    this->server.on("/ota", HTTP_POST, [&, this]() {
        String content;
        content += ERA_FPSTR(webOTA);
        content += ERA_FPSTR(webStyle);
        WebProcessor(content);
        this->server.send(200, "text/html", content);
    });

    this->server.onNotFound([&, this]() {
        this->server.sendHeader("Location", "/");
        this->server.send(302, "text/plain", "Not found");
    });

    this->udpERa.on("connect/wifi", [&, this]() {
        ERA_LOG(ERA_PSTR("Config"), ERA_PSTR("Configuration..."));
        String ssid = this->udpERa.arg("ssid").c_str();
        String pass = this->udpERa.arg("pass").c_str();
        String backupSsid = this->udpERa.arg("backup_ssid").c_str();
        String backupPass = this->udpERa.arg("backup_pass").c_str();

        String token = this->udpERa.arg("token").c_str();
        String host = this->udpERa.arg("host").c_str();
        String port = this->udpERa.arg("port").c_str();
        String username = this->udpERa.arg("username").c_str();
        String password = this->udpERa.arg("password").c_str();

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

        if (ERaStrCmp(ERaConfig.ssid, this->specSSID) &&
            ERaStrCmp(ERaConfig.pass, this->specPass)) {
            ClearArray(ERaConfig.ssid);
            ClearArray(ERaConfig.pass);
        }

        content = ERA_F(R"json({"status":"ok","message":"Connecting wifi..."})json");
        ERA_FORCE_UNUSED(content);
        if (ssid.length()) {
            // this->udpERa.send(content.c_str());
            ERaDelay(100);
            /* TODO: send first time */
            this->udpERa.sendBoardInfo();
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_UDP, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
            ERaState::set(StateT::STATE_SWITCH_TO_AP_STA);
        }
    });

    this->udpERa.on("connect/token", [&, this]() {
        String token = this->udpERa.arg("token").c_str();

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

    this->udpERa.on("connect/host", [&, this]() {
        String host = this->udpERa.arg("host").c_str();

        if (host.length()) {
            CopyToArray(host, ERaConfig.host);
        }
    });

    this->udpERa.on("scan/wifi", [&, this]() {
        ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Scanning wifi..."));
        String content;
        int nets = this->scanNetworks();

        if (nets < 0) {
            this->udpERa.send("[]");
            return;
        }

        int indices[(nets ? nets : 1)] {0};
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
            this->udpERa.send("[]");
            return;
        }
        cJSON_AddStringToObject(root, "type", "data_wifi");
        cJSON* array = cJSON_CreateArray();
        if (array != nullptr) {
            int limit {0};
            std::vector<std::string> listWifi;
            for (int i = 0; i < nets; ++i) {
                int id = indices[i];
                if (WiFi.SSID(id).isEmpty()) {
                    continue;
                }
                std::vector<std::string>::iterator wifi = std::find_if(listWifi.begin(), listWifi.end(), [id](const std::string& e) {
                    return e == WiFi.SSID(id).c_str();
                });
                if (wifi != listWifi.end()) {
                    continue;
                }
                listWifi.push_back(WiFi.SSID(id).c_str());
                cJSON* item = cJSON_CreateObject();
                if (item == nullptr) {
                    break;
                }
                cJSON_AddStringToObject(item, "ssid", WiFi.SSID(id).c_str());
                cJSON_AddNumberToObject(item, "rssi", WiFi.RSSI(id));
                cJSON_AddItemToArray(array, item);
                item = nullptr;
                if(++limit >= WIFI_SCAN_MAX) {
                    break;
                }
            }

            /* Add spec SSID */
            if (strlen(this->specSSID)) {
                cJSON* item = cJSON_CreateObject();
                if (item != nullptr) {
                    cJSON_AddStringToObject(item, "ssid", this->specSSID);
                    cJSON_AddNumberToObject(item, "rssi", -27);
                    cJSON_AddItemToArray(array, item);
                }
                item = nullptr;
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

        this->udpERa.send(content.c_str());
    });

    this->udpERa.on("system/reset", [&, this]() {
        String content = ERA_F(R"json({"status":"ok","message":"Configuration reset"})json");
        this->udpERa.send(content.c_str());
        ERaState::set(StateT::STATE_RESET_CONFIG);
    });

    this->udpERa.on("system/reboot", [&, this]() {
        String content = ERA_F(R"json({"status":"ok","message":"Rebooting..."})json");
        this->udpERa.send(content.c_str());
        ERaState::set(StateT::STATE_REBOOT);
    });
}

template <class Transport>
void ERaPnP<Transport>::configInit() {
    this->configLoad();
    if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
        ERaState::set(StateT::STATE_CONNECTING_NETWORK);
    }
    else if (this->netCb() ||
            this->wifiConnected()) {
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
    this->scanNetworks();
    ERaWatchdogFeed();

    this->dnsServer.start(DNS_PORT, CONFIG_AP_URL, WiFi.softAPIP());
    ERA_LOG(TAG, ERA_PSTR("AP URL: %s"), CONFIG_AP_URL);

    this->server.begin();
    this->udpERa.setERaModel(Base::getERaModel());
    this->udpERa.begin((this->authToken == nullptr) ? ERaConfig.token : this->authToken);

    MillisTime_t tick = ERaMillis();
    while (ERaState::is(StateT::STATE_WAIT_CONFIG) || ERaState::is(StateT::STATE_CONFIGURING)) {
        ERaDelay(10);
        this->udpERa.run();
        this->server.handleClient();
        this->dnsServer.processNextRequest();
        ERaWatchdogFeed();
        if (ERaState::is(StateT::STATE_CONFIGURING) && !WiFi.softAPgetStationNum()) {
            ERaState::set(StateT::STATE_WAIT_CONFIG);
        }
        else if (!WiFi.softAPgetStationNum()) {
            this->scanNetworks();
        }
        if (ERaState::is(StateT::STATE_WAIT_CONFIG) && this->netCb()) {
            ERaState::set(StateT::STATE_CONNECTING_CLOUD);
            break;
        }
        if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
            if (!ERaRemainingTime(tick, WIFI_NET_CHECK_TIMEOUT)) {
                ERaState::set(StateT::STATE_SWITCH_TO_STA);
                break;
            }
        }
    }

    this->waitHandle(1000UL);

    if (!strlen(ERaConfig.ssid) &&
        this->hasNetworkCallback &&
        ERaState::is(StateT::STATE_SWITCH_TO_AP_STA)) {
        tick = ERaMillis();
        ERA_LOG(TAG, ERA_PSTR("Wait network connected..."));
        do {
            if (this->netCb()) {
                break;
            }
            ERaDelay(10);
            ERaWatchdogFeed();
        } while (ERaRemainingTime(tick, 10000UL));
    }

    ERaDelay(1000);
    WiFi.scanDelete();
    this->server.stop();
    this->dnsServer.stop();
}

template <class Transport>
void ERaPnP<Transport>::waitHandle(unsigned long ms) {
    if (!ms) {
        return;
    }

    MillisTime_t startMillis = ERaMillis();
    do {
        ERaDelay(10);
        this->udpERa.run();
        this->server.handleClient();
        this->dnsServer.processNextRequest();
        ERaWatchdogFeed();
    } while (ERaRemainingTime(startMillis, ms));
}

template <class Transport>
int ERaPnP<Transport>::scanNetworks() {
    int nets = WiFi.scanComplete();
    unsigned long currentMillis = ERaMillis();
    if ((nets >= 0) && ((currentMillis - this->prevMillisScan) < WIFI_SCAN_INTERVAL)) {
        return nets;
    }
    unsigned long skipTimes = ((currentMillis - this->prevMillisScan) / WIFI_SCAN_INTERVAL);
    // update time
    this->prevMillisScan += (WIFI_SCAN_INTERVAL * skipTimes);

    nets = WiFi.scanNetworks(true, true, false,
                    WIFI_SCAN_CHANNEL_TIMEOUT);
    MillisTime_t tick = ERaMillis();
    while (nets < 0 && ERaRemainingTime(tick, WIFI_SCAN_TIMEOUT)) {
        ERaDelay(100);
        nets = WiFi.scanComplete();
    }
    ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Found %d wifi"), nets);

    return nets;
}

template <class Transport>
void ERaPnP<Transport>::connectNetwork() {
    char ssidAP[64] {0};
    this->getWiFiName(ssidAP);
    this->replace(ssidAP, '.', '-');
    WiFi.setHostname(ssidAP);

    this->connectWiFi(ERaConfig.ssid, ERaConfig.pass);
    this->connectWiFiBackup();
    if (this->netConnected()) {
        /* Udp */
        if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_UDP)) {
            String content = ERA_F(R"json({"status":"ok","message":"Connected to WiFi"})json");
            this->udpERa.send(content.c_str());
            ERaDelay(100);
            this->udpERa.sendBoardInfo();
            ERaDelay(500);
            this->udpERa.stop();
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_UDP, false);
        }
        ERaDelay(100);
        WiFi.mode(WIFI_STA);
        /* Udp */
        ERaState::set(StateT::STATE_CONNECTING_CLOUD);
    }
    else {
        /* Udp */
        if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_UDP)) {
            String content = ERA_F(R"json({"status":"error","message":"Connect WiFi failed"})json");
            this->udpERa.send(content.c_str());
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
    if (this->wifiConnected()) {
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
        this->configSave();
        ERaOptConnected(this);
        ERaState::set(StateT::STATE_CONNECTED);
    }
    else if (ERaState::is(StateT::STATE_RESET_CONFIG)) {
        return;
    }
    else {
        ERaState::set(StateT::STATE_CONNECTING_NETWORK);
    }
}

template <class Transport>
void ERaPnP<Transport>::connectWiFi(const char* ssid, const char* pass) {
    if (this->netCb() ||
        this->wifiConnected()) {
        return;
    }
    if (!strlen(ssid)) {
        return;
    }

    int status = WiFi.status();

    MillisTime_t started = ERaMillis();
    while (status != WL_CONNECTED) {
        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Connecting to %s..."), ssid);
        if (pass && strlen(pass)) {
            WiFi.begin(ssid, pass);
        } else {
            WiFi.begin(ssid);
        }

        ERaWatchdogFeed();

        MillisTime_t startMillis = ERaMillis();
        while (status != WL_CONNECTED) {
            ERaDelay(500);
            ERaWatchdogFeed();
            status = WiFi.status();
            if (!ERaRemainingTime(started, WIFI_NET_CONNECT_TIMEOUT)) {
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

    IPAddress localIP = WiFi.localIP();
    ERA_FORCE_UNUSED(localIP);
    ERA_LOG(TAG, ERA_PSTR("IP: %s"), localIP.toString().c_str());
}

template <class Transport>
void ERaPnP<Transport>::connectNewWiFi(const char* ssid, const char* pass) {
    if (ssid == nullptr) {
        return;
    }
    if (!strlen(ssid)) {
        return;
    }
    if (ERaStrCmp(this->newWiFi.ssid, ssid)) {
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

    ERaWatchdogFeed();
    ERaDelay(100);
    WiFi.mode(WIFI_OFF);
    ERaDelay(100);
    WiFi.mode(WIFI_AP);
    ERaDelay(2000);
    WiFi.softAPConfig(WIFI_AP_IP, WIFI_AP_IP, WIFI_AP_Subnet);
    WiFi.softAP(ssidAP, WIFI_AP_PASS);
    ERaDelay(500);
    ERaWatchdogFeed();

    IPAddress myIP = WiFi.softAPIP();
    ERA_FORCE_UNUSED(myIP);
    ERA_LOG(TAG, ERA_PSTR("AP SSID: %s"), ssidAP);
    ERA_LOG(TAG, ERA_PSTR("AP IP: %s"), myIP.toString().c_str());

    ERaState::set(StateT::STATE_WAIT_CONFIG);
}

template <class Transport>
void ERaPnP<Transport>::switchToSTA() {
    ERA_LOG(TAG, ERA_PSTR("Switching to STA..."));

    ERaDelay(1000);
    WiFi.mode(WIFI_OFF);
    ERaDelay(100);
    WiFi.mode(WIFI_STA);
    ERaWatchdogFeed();
    
    ERaState::set(StateT::STATE_CONNECTING_NETWORK);
}

template <class Transport>
void ERaPnP<Transport>::switchToAPSTA() {
    ERA_LOG(TAG, ERA_PSTR("Switching to AP STA..."));

    ERaDelay(100);
    WiFi.mode(WIFI_AP_STA);
    ERaWatchdogFeed();
    
    ERaState::set(StateT::STATE_CONNECTING_NETWORK);
}

template <class Transport>
template <int size>
void ERaPnP<Transport>::getWiFiName(char(&ptr)[size], bool withPrefix) {
    const uint64_t chipId = ESP.getEfuseMac();
    uint64_t unique {0};
    for (int i = 0; i < 41; i = i + 8) {
        unique |= ((chipId >> (40 - i)) & 0xff) << i;
    }
    ClearArray(ptr);
    if (withPrefix) {
        FormatString(ptr, "eoh.%s.%04x%08x", ((Base::getERaModel() != nullptr) ? Base::getERaModel() : ERA_MODEL_NAME),
                                            static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    } else {
        FormatString(ptr, "era.%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    }
}

template <class Transport>
template <int size>
void ERaPnP<Transport>::getImeiChip(char(&ptr)[size]) {
    const uint64_t chipId = ESP.getEfuseMac();
    uint64_t unique {0};
    for (int i = 0; i < 41; i = i + 8) {
        unique |= ((chipId >> (40 - i)) & 0xff) << i;
    }
    ClearArray(ptr);
#ifdef ERA_AUTH_TOKEN
    FormatString(ptr, ERA_AUTH_TOKEN);
#else
    if ((this->authToken != nullptr) && strlen(this->authToken)) {
        FormatString(ptr, this->authToken);
    }
    else {
        FormatString(ptr, "ERA-%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
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

#if (ESP_IDF_VERSION_MAJOR < 4)
    #include <rom/rtc.h>
    #define ERA_RESET_REASON
#else
    #if CONFIG_IDF_TARGET_ESP32
        #include <esp32/rom/rtc.h>
        #define ERA_RESET_REASON
    #elif CONFIG_IDF_TARGET_ESP32S2
        #include <esp32s2/rom/rtc.h>
        #define ERA_RESET_REASON
    #elif CONFIG_IDF_TARGET_ESP32C3
        #include <esp32c3/rom/rtc.h>
        #define ERA_RESET_REASON
    #elif CONFIG_IDF_TARGET_ESP32S3
        #include <esp32s3/rom/rtc.h>
        #define ERA_RESET_REASON
    #endif
#endif

#if defined(ERA_RESET_REASON)
    static inline
    String SystemGetResetReason() {
        String rstReason;
        int reason = rtc_get_reset_reason(0);
        switch (reason) {
            case 1:
                rstReason = "POWERON_RESET";          /** <1,  Vbat power on reset */
                break;
            case 3:
                rstReason = "SW_RESET";               /** <3,  Software reset digital core */
                break;
            case 4:
                rstReason = "OWDT_RESET";             /** <4,  Legacy watch dog reset digital core */
                break;
            case 5:
                rstReason = "DEEPSLEEP_RESET";        /** <5,  Deep Sleep reset digital core */
                break;
            case 6:
                rstReason = "SDIO_RESET";             /** <6,  Reset by SLC module, reset digital core */
                break;
            case 7:
                rstReason = "TG0WDT_SYS_RESET";       /** <7,  Timer Group0 Watch dog reset digital core */
                break;
            case 8:
                rstReason = "TG1WDT_SYS_RESET";       /** <8,  Timer Group1 Watch dog reset digital core */
                break;
            case 9:
                rstReason = "RTCWDT_SYS_RESET";       /** <9,  RTC Watch dog Reset digital core */
                break;
            case 10:
                rstReason = "INTRUSION_RESET";       /** <10, Instrusion tested to reset CPU */
                break;
            case 11:
                rstReason = "TGWDT_CPU_RESET";       /** <11, Time Group reset CPU */
                break;
            case 12:
                rstReason = "SW_CPU_RESET";          /** <12, Software reset CPU */
                break;
            case 13:
                rstReason = "RTCWDT_CPU_RESET";      /** <13, RTC Watch dog Reset CPU */
                break;
            case 14:
                rstReason = "EXT_CPU_RESET";         /** <14, for APP CPU, reseted by PRO CPU */
                break;
            case 15:
                rstReason = "RTCWDT_BROWN_OUT_RESET";/** <15, Reset when the vdd voltage is not stable */
                break;
            case 16:
                rstReason = "RTCWDT_RTC_RESET";      /** <16, RTC Watch dog reset digital core and rtc module */
                break;
            default:
                rstReason = "UNKNOWN";
                break;
        }
        return rstReason;
    }
#endif

#include <Network/ERaWiFiEsp32.hpp>

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_BOARD_ID, this->thisProto().getBoardID());
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().getAuth());
    cJSON_AddStringToObject(root, INFO_BUILD_DATE, BUILD_DATE_TIME);
    cJSON_AddStringToObject(root, INFO_VERSION, ERA_VERSION);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, WiFi.SSID().c_str());
    cJSON_AddStringToObject(root, INFO_BSSID, WiFi.BSSIDstr().c_str());
    cJSON_AddNumberToObject(root, INFO_RSSI, WiFi.RSSI());
    cJSON_AddStringToObject(root, INFO_MAC, WiFi.macAddress().c_str());
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, WiFi.localIP().toString().c_str());
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().getTransp().getPing());

#if defined(ERA_RESET_REASON)
    cJSON_AddStringToObject(root, INFO_RESET_REASON, SystemGetResetReason().c_str());
#endif

    /* Override info */
    ERaInfo(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
    cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, static_cast<uint16_t>(temperatureRead() * 100.0f));
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, WiFi.RSSI());
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, WiFi.SSID().c_str());

    /* Override modbus info */
    ERaModbusInfo(root);
}

#endif /* INC_ERA_PNP_ESP32_HPP_ */
