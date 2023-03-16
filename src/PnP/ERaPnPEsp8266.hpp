#ifndef INC_ERA_PNP_ESP8266_HPP_
#define INC_ERA_PNP_ESP8266_HPP_

#if !defined(ERA_PROTO_TYPE)
    #define ERA_PROTO_TYPE            "WiFi"
#endif

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ERa/ERaApiArduinoDef.hpp>
#include <ERa/ERaProtocol.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <MQTT/ERaMqtt.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Utility/ERaFlashEsp8266.hpp>
#include <PnP/ERaWeb.hpp>
#include <PnP/ERaState.hpp>

#define CONFIG_AP_URL                 "era.setup"

#define WIFI_SCAN_MAX                 20
#define WIFI_SCAN_TIMEOUT             20000
#define WIFI_NET_CONNECT_TIMEOUT      60000
#define WIFI_CLOUD_CONNECT_TIMEOUT    60000
#define WIFI_NET_CHECK_TIMEOUT        WIFI_NET_CONNECT_TIMEOUT * 3
#define WIFI_AP_PASS                  "Eoh@2021"
#define WIFI_AP_IP                    IPAddress(192, 168, 27, 1)
#define WIFI_AP_Subnet                IPAddress(255, 255, 255, 0)

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
    static ESP8266WebServer server(80);
    static DNSServer dnsServer;
    static ERaConfig_t ERaConfig {};
#else
    #define server      StaticHelper<ESP8266WebServer, int>::instance(80)
    #define dnsServer   StaticHelper<DNSServer>::instance()
    #define ERaConfig   StaticHelper<ERaConfig_t>::instance()
#endif

static const uint16_t DNS_PORT = 53;
static const ERaConfig_t ERaDefault = {
    .magic = 0x27061994,
    .flags = 0x00,
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
    CONFIG_FLAG_API = 0x04
};

#include <Network/ERaUdpEsp8266.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static WiFiUDP Udp;
    static ERaUdp<WiFiUDP> eraUdp(Udp);
#else
    #define Udp     StaticHelper<WiFiUDP>::instance()
    #define eraUdp  StaticRefHelper<ERaUdp<WiFiUDP>, WiFiUDP>::instance(Udp)
#endif

template <class Transport>
class ERaPnP
    : public ERaProto<Transport, ERaFlash>
{
    const char* TAG = "WiFi";
    const char* HOSTNAME = "ERa";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;

public:
    ERaPnP(Transport& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , authToken(nullptr)
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
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        WiFi.mode(WIFI_STA);
        this->connectWiFi(ssid, pass);
        Base::init();
        this->config(auth, host, port, username, password);
        if (this->connected()) {
            CopyToArray(ssid, ERaConfig.ssid);
            CopyToArray(pass, ERaConfig.pass);
            CopyToArray(auth, ERaConfig.token);
            CopyToArray(host, ERaConfig.host);
            ERaConfig.port = port;
            CopyToArray(username, ERaConfig.username);
            CopyToArray(password, ERaConfig.password);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            if (Base::connect()) {
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

    void begin(const char* ssid,
                const char* pass) {
        this->begin(ERA_AUTHENTICATION_TOKEN, ssid, pass,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
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
    void connectNetwork();
    void connectCloud();
    void connectWiFi(const char* ssid, const char* pass);
    void switchToAP();
    void switchToSTA();
    void switchToAPSTA();

    template <int size>
    void getWiFiName(char(&ptr)[size], bool withPrefix = true);
    template <int size>
    void getImeiChip(char(&ptr)[size]);

    bool connected() const {
        return (WiFi.status() == WL_CONNECTED);
    }

    static void onWiFiEvent(WiFiEvent_t event);

    const char* authToken;
};

template <class Transport>
void ERaPnP<Transport>::begin(const char* auth) {
    this->authToken = auth;
    WiFi.persistent(false);
    WiFi.enableSTA(true);

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
            if (this->connected()) {
                ERaState::set(StateT::STATE_CONNECTING_CLOUD);
            }
            else {
                this->connectNetwork();
            }
            break;
        case StateT::STATE_CONNECTING_CLOUD:
            this->connectCloud();
            break;
        case StateT::STATE_CONNECTED:
            ERaState::set(StateT::STATE_RUNNING);
            break;
        case StateT::STATE_RUNNING:
            if (this->connected()) {
                Base::run();
            }
            else {
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
    if (!ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_API) &&
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

    dnsServer.setTTL(300);
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

    server.on("/api/config", HTTP_POST, []() {
        ERA_LOG(ERA_PSTR("Config"), ERA_PSTR("Configuration..."));
        String ssid = server.arg("ssid");
        String pass = server.arg("pass");
        String backupSsid = server.arg("backup_ssid");
        String backupPass = server.arg("backup_pass");

        String token = server.arg("token");
        String host = server.arg("host");
        String port = server.arg("port");
        String username = server.arg("username");
        String password = server.arg("password");

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
        else {
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
        else {
            CopyToArray(imei, ERaConfig.username);
        }
        if (password.length()) {
            CopyToArray(password, ERaConfig.password);
        }
        else {
            CopyToArray(imei, ERaConfig.password);
        }

        content = ERA_F(R"json({"status":"ok","message":"Connecting wifi..."})json");
        server.send(200, "application/json", content);
        if (ssid.length()) {
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
            ERaState::set(StateT::STATE_SWITCH_TO_STA);
        }
    });

    server.on("/api/board_info.json", HTTP_POST, []() {
        ERA_LOG(ERA_PSTR("Info"), ERA_PSTR("Board info"));
        String content;
        cJSON* root = cJSON_CreateObject();
        if (root != nullptr) {
            cJSON_AddStringToObject(root, "board", ERA_BOARD_TYPE);
            cJSON_AddStringToObject(root, "model", ERA_MODEL_TYPE);
            cJSON_AddStringToObject(root, "imei", imei);
            cJSON_AddStringToObject(root, "firmware_version", ERA_FIRMWARE_VERSION);
            cJSON_AddStringToObject(root, "ssid", ssidAP);
            cJSON_AddStringToObject(root, "pass", WIFI_AP_PASS);
            cJSON_AddStringToObject(root, "bssid", WiFi.softAPmacAddress().c_str());
            cJSON_AddStringToObject(root, "mac", WiFi.macAddress().c_str());
            cJSON_AddStringToObject(root, "ip", WiFi.softAPIP().toString().c_str());
            char* ptr = cJSON_PrintUnformatted(root);
            if (ptr != nullptr) {
                content = ptr;
                free(ptr);
            }
            cJSON_Delete(root);
            root = nullptr;
            ptr = nullptr;
        }
        server.send(200, "application/json", content);
    });

    server.on("/api/wifi_scan.json", HTTP_POST, []() {
        ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Scanning wifi..."));
        String content;
        int nets = WiFi.scanNetworks(true, true);
        MillisTime_t tick = ERaMillis();
        while (nets < 0 && ERaRemainingTime(tick, WIFI_SCAN_TIMEOUT)) {
            ERaDelay(100);
            nets = WiFi.scanComplete();
        }
        ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Found %d wifi"), nets);

        if (nets <= 0) {
            server.send(200, "application/json", "[]");
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
                switch (WiFi.encryptionType(id)) {
                    case wl_enc_type::ENC_TYPE_WEP:
                        cJSON_AddStringToObject(item, "encryption", "WEP");
                        break;
                    case wl_enc_type::ENC_TYPE_TKIP:
                        cJSON_AddStringToObject(item, "encryption", "WPA/PSK");
                        break;
                    case wl_enc_type::ENC_TYPE_CCMP:
                        cJSON_AddStringToObject(item, "encryption", "WPA2/PSK");
                        break;
                    case wl_enc_type::ENC_TYPE_AUTO:
                        cJSON_AddStringToObject(item, "encryption", "WPA/WPA2/PSK");
                        break;
                    case wl_enc_type::ENC_TYPE_NONE:
                        cJSON_AddStringToObject(item, "encryption", "OPEN");
                        break;
                    default:
                        cJSON_AddStringToObject(item, "encryption", "unknown");
                        break;
                }
                cJSON_AddNumberToObject(item, "channel", WiFi.channel(id));
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
        WiFi.scanDelete();
        server.send(200, "application/json", content);
    });

    server.on("/api/reset", HTTP_POST, []() {
        String content = ERA_F(R"json({"status":"ok","message":"Configuration reset"})json");
        server.send(200, "application/json", content);
        ERaState::set(StateT::STATE_RESET_CONFIG);
    });

    server.on("/api/reboot", HTTP_POST, []() {
        String content = ERA_F(R"json({"status":"ok","message":"Rebooting..."})json");
        server.send(200, "application/json", content);
        ERaState::set(StateT::STATE_REBOOT);
    });

    server.on("/", []() {
        String content = ERA_FPSTR(webIndex);
        content += ERA_FPSTR(webScript);
        content += ERA_FPSTR(webStyle);
        webProcessor(content);
        server.send(200, "text/html", content);
    });

    server.on("/wifi", []() {
        ERA_LOG(ERA_PSTR("Config"), ERA_PSTR("Configuration..."));
        String ssid = server.arg("ssid");
        String pass = server.arg("pass");
        String backupSsid = server.arg("backup_ssid");
        String backupPass = server.arg("backup_pass");
        bool scanWifi  = server.hasArg("scan");
        bool hasConnect = server.hasArg("connect");

        String content;
        int nets {0};
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

        if (scanWifi) {
            nets = WiFi.scanNetworks(true, true);
            MillisTime_t tick = ERaMillis();
            while (nets < 0 && ERaRemainingTime(tick, WIFI_SCAN_TIMEOUT)) {
                ERaDelay(100);
                nets = WiFi.scanComplete();
            }
            ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Found %d wifi"), nets);
        }

        content = ERA_F("<meta charset='utf-8'>");
        content += ERA_F("<form name=wifiForm method=post>");
        content += ERA_F("<h1>WIFI</h1>");
        content += ERA_F("<table border=1 width='100%'>");
        content += ERA_F("<tr><th width='50%' height='30px'>Wifi</th>");
        content += ERA_F("<th width='50%'>Wifi 2</th></tr>");
        content += ERA_F("<tr><td style='text-align:center'>") + String(strlen(ERaConfig.ssid) ? ERaConfig.ssid : "--");
        content += ERA_F("</td><td style='text-align:center'>") + String(strlen(ERaConfig.backupSSID) ? ERaConfig.backupSSID : "--");
        content += ERA_F("</td></tr></table>");

        if (nets > 0) {
            content += ERA_F("<select name=ssid id=ssid width='100%'>");
            content += ERA_F("<option value='' disabled selected>%SELECT_SSID%</option>");
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
                content += ((WiFi.encryptionType(i) == wl_enc_type::ENC_TYPE_NONE) ? "" : "*");
                content += ERA_F("</option>");
                if (++limit >= 20) {
                    break;
                }
            }
            content += ERA_F("</select>");
        }
        else {
            content += ERA_F("<input name=ssid id=ssid placeholder='%SSID%'>");
        }
        content += ERA_F("<input name=pass id=pass placeholder='%PASSWORD%' type=password>");

        if (nets > 0) {
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
                content += ((WiFi.encryptionType(i) == wl_enc_type::ENC_TYPE_NONE) ? "" : "*");
                content += ERA_F("</option>");
                if (++limit >= 20) {
                    break;
                }
            }
            content += ERA_F("</select>");
        }
        else {
            content += ERA_F("<input name=backup_ssid id=backup_ssid placeholder='%BACKUP_SSID%'>");
        }
        content += ERA_F("<input name=backup_pass id=backup_pass placeholder='%BACKUP_PASS%' type=password>");
        content += ERA_F("<input type=checkbox onclick=togglePass() class=checkbox> %SHOW_PASSWORD%");
        if (!scanWifi) {
            content += ERA_F("<input type=submit name=scan id=scan class=btn value='%SCAN_WIFI%'>");
        }
        content += ERA_F("<input type=submit onclick=clicked(event) name=connect id=connect class=btn value='%CONNECT%'>");
        content += ERA_F("<input type=submit formaction='/' class=btn value='%BACK%'></form>");

        content += ERA_FPSTR(webScript);
        content += ERA_FPSTR(webStyle);

        webProcessor(content);
        server.send(200, "text/html", content);
        if (ssid.length() && hasConnect) {
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
            ERaState::set(StateT::STATE_SWITCH_TO_STA);
        }
    });

    server.on("/wifi/reset", []() {
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", "");
        ERaState::set(StateT::STATE_RESET_CONFIG);
    });

    server.on("/network/status", HTTP_POST, [=]() {
        String content;
        if (this->connected()) {
            content += LANGUAGE_CONNECTED(LANGUAGE_EN);
            content += ERA_F(" ");
            content += WiFi.SSID();
        }
        else {
            content += ERA_F("--");
        }
        server.send(200, "text/plane", content);
    });

    server.on("/network/ip", HTTP_POST, [=]() {
        String content;
        if (this->connected()) {
            content += WiFi.localIP().toString();
        }
        else {
            content += ERA_F("--");
        }
        server.send(200, "text/plane", content);
    });

    server.on("/network/ssid", HTTP_POST, [=]() {
        String content;
        if (this->connected()) {
            content += WiFi.SSID();
        }
        else {
            content += ERA_F("--");
        }
        server.send(200, "text/plane", content);
    });

    server.on("/network/rssi", HTTP_POST, [=]() {
        String content;
        if (this->connected()) {
            content += WiFi.RSSI();
        }
        else {
            content += ERA_F("--");
        }
        server.send(200, "text/plane", content);
    });

    server.on("/network/mac", HTTP_POST, [=]() {
        String content;
        content += WiFi.macAddress();
        server.send(200, "text/plane", content);
    });

    server.onNotFound([]() {
        server.send(404, "text/plain", "Not found");
    });

    eraUdp.on("connect/wifi", []() {
        ERA_LOG(ERA_PSTR("Config"), ERA_PSTR("Configuration..."));
        String ssid = eraUdp.arg("ssid").c_str();
        String pass = eraUdp.arg("pass").c_str();
        String backupSsid = eraUdp.arg("backup_ssid").c_str();
        String backupPass = eraUdp.arg("backup_pass").c_str();

        String token = eraUdp.arg("token").c_str();
        String host = eraUdp.arg("host").c_str();
        String port = eraUdp.arg("port").c_str();
        String username = eraUdp.arg("username").c_str();
        String password = eraUdp.arg("password").c_str();

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
        else {
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
        else {
            CopyToArray(imei, ERaConfig.username);
        }
        if (password.length()) {
            CopyToArray(password, ERaConfig.password);
        }
        else {
            CopyToArray(imei, ERaConfig.password);
        }

        content = ERA_F(R"json({"status":"ok","message":"Connecting wifi..."})json");
        eraUdp.send(content.c_str());
        if (ssid.length()) {
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
            ERaState::set(StateT::STATE_SWITCH_TO_AP_STA);
        }
    });

    eraUdp.on("scan/wifi", []() {
        ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Scanning wifi..."));
        String content;
        int nets = WiFi.scanNetworks(true, true);
        MillisTime_t tick = ERaMillis();
        while (nets < 0 && ERaRemainingTime(tick, WIFI_SCAN_TIMEOUT)) {
            ERaDelay(100);
            nets = WiFi.scanComplete();
        }
        ERA_LOG(ERA_PSTR("WiFi"), ERA_PSTR("Found %d wifi"), nets);

        if (nets <= 0) {
            eraUdp.send("[]");
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
            eraUdp.send("[]");
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
        WiFi.scanDelete();
        eraUdp.send(content.c_str());
    });

    eraUdp.on("system/reset", []() {
        String content = ERA_F(R"json({"status":"ok","message":"Configuration reset"})json");
        eraUdp.send(content.c_str());
        ERaState::set(StateT::STATE_RESET_CONFIG);
    });

    eraUdp.on("system/reboot", []() {
        String content = ERA_F(R"json({"status":"ok","message":"Rebooting..."})json");
        eraUdp.send(content.c_str());
        ERaState::set(StateT::STATE_REBOOT);
    });
}

template <class Transport>
void ERaPnP<Transport>::configInit() {
    this->configLoad();
    if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
        ERaState::set(StateT::STATE_CONNECTING_NETWORK);
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
    ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
    this->configSave();
    ERaState::set(StateT::STATE_SWITCH_TO_AP);
    ERA_LOG(TAG, ERA_PSTR("Resetting configuration"));
}

template <class Transport>
void ERaPnP<Transport>::configMode() {
    this->configApi();

    ERA_LOG(TAG, ERA_PSTR("Config mode"));

    dnsServer.start(DNS_PORT, CONFIG_AP_URL, WiFi.softAPIP());
    ERA_LOG(TAG, ERA_PSTR("AP URL: %s"), CONFIG_AP_URL);

    server.begin();
    eraUdp.begin(this->authToken);

    MillisTime_t tick = ERaMillis();
    while (ERaState::is(StateT::STATE_WAIT_CONFIG) || ERaState::is(StateT::STATE_CONFIGURING)) {
        ERaDelay(10);
        eraUdp.run();
        server.handleClient();
        dnsServer.processNextRequest();
        if (ERaState::is(StateT::STATE_CONFIGURING) && !WiFi.softAPgetStationNum()) {
            ERaState::set(StateT::STATE_WAIT_CONFIG);
        }
        if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
            if (!ERaRemainingTime(tick, WIFI_NET_CHECK_TIMEOUT)) {
                ERaState::set(StateT::STATE_SWITCH_TO_STA);
                break;
            }
        }
    }

    ERaDelay(1000);
    server.stop();
    dnsServer.stop();
}

template <class Transport>
void ERaPnP<Transport>::connectNetwork() {
    char ssidAP[64] {0};
    this->getWiFiName(ssidAP);
    WiFi.setHostname(ssidAP);

    this->connectWiFi(ERaConfig.ssid, ERaConfig.pass);
    if (this->connected()) {
        /* Udp */
        eraUdp.sendBoardInfo();
        eraUdp.stop();
        ERaDelay(100);
        WiFi.mode(WIFI_STA);
        /* Udp */
        ERaState::set(StateT::STATE_CONNECTING_CLOUD);
    }
    else {
        ERaState::set(StateT::STATE_SWITCH_TO_AP);
        ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
    }
}

template <class Transport>
void ERaPnP<Transport>::connectCloud() {
    ERA_LOG(TAG, ERA_PSTR("Connecting cloud..."));
    this->config(ERaConfig.token, ERaConfig.host, ERaConfig.port, ERaConfig.username, ERaConfig.password);
    if (Base::connect()) {
        this->configSave();
        ERaState::set(StateT::STATE_CONNECTED);
    }
    else {
        ERaState::set(StateT::STATE_CONNECTING_NETWORK);
    }
}

template <class Transport>
void ERaPnP<Transport>::connectWiFi(const char* ssid, const char* pass) {
    int status = WiFi.status();

    MillisTime_t started = ERaMillis();
    while (status != WL_CONNECTED) {
        ERA_LOG(TAG, ERA_PSTR("Connecting to %s..."), ssid);
        if (pass && strlen(pass)) {
            WiFi.begin(ssid, pass);
        } else {
            WiFi.begin(ssid);
        }

        MillisTime_t startMillis = ERaMillis();
        while (status != WL_CONNECTED) {
            ERaDelay(500);
            status = WiFi.status();
            if (!ERaRemainingTime(started, WIFI_NET_CONNECT_TIMEOUT)) {
                WiFi.disconnect();
                return;
            }
            else if (!ERaRemainingTime(startMillis, 20000)) {
                WiFi.disconnect();
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
void ERaPnP<Transport>::switchToAP() {
    ERA_LOG(TAG, ERA_PSTR("Switching to AP..."));

    char ssidAP[64] {0};
    this->getWiFiName(ssidAP);

    WiFi.mode(WIFI_OFF);
    ERaDelay(100);
    WiFi.mode(WIFI_AP);
    ERaDelay(2000);
    WiFi.softAPConfig(WIFI_AP_IP, WIFI_AP_IP, WIFI_AP_Subnet);
    WiFi.softAP(ssidAP, WIFI_AP_PASS);
    ERaDelay(500);

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
    
    ERaState::set(StateT::STATE_CONNECTING_NETWORK);
}

template <class Transport>
void ERaPnP<Transport>::switchToAPSTA() {
    ERA_LOG(TAG, ERA_PSTR("Switching to AP STA..."));

    ERaDelay(100);
    WiFi.mode(WIFI_AP_STA);
    
    ERaState::set(StateT::STATE_CONNECTING_NETWORK);
}

template <class Transport>
template <int size>
void ERaPnP<Transport>::getWiFiName(char(&ptr)[size], bool withPrefix) {
	String macAddr = WiFi.macAddress();
	macAddr.replace(":", "");
	macAddr.toLowerCase();
    ClearArray(ptr);
    if (withPrefix) {
        FormatString(ptr, "eoh.era.%s", macAddr.c_str());
    } else {
        FormatString(ptr, "era.%s", macAddr.c_str());
    }
}

template <class Transport>
template <int size>
void ERaPnP<Transport>::getImeiChip(char(&ptr)[size]) {
	String macAddr = WiFi.macAddress();
	macAddr.replace(":", "");
	macAddr.toLowerCase();
    ClearArray(ptr);
#ifdef ERA_AUTH_TOKEN
    FormatString(ptr, ERA_AUTH_TOKEN);
#else
    if (this->authToken != nullptr) {
        FormatString(ptr, this->authToken);
    }
    else {
        FormatString(ptr, "ERA-%s", macAddr.c_str());
    }
#endif
}

#include <Network/ERaWiFiEsp8266.hpp>

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_BOARD_ID, this->thisProto().getBoardID());
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().getAuth());
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, WiFi.SSID().c_str());
    cJSON_AddStringToObject(root, INFO_BSSID, WiFi.BSSIDstr().c_str());
    cJSON_AddNumberToObject(root, INFO_RSSI, WiFi.RSSI());
    cJSON_AddStringToObject(root, INFO_MAC, WiFi.macAddress().c_str());
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, WiFi.localIP().toString().c_str());
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().getTransp().getPing());
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, WiFi.RSSI());
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, WiFi.SSID().c_str());
}

#endif /* INC_ERA_PNP_ESP8266_HPP_ */
