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

static ESP8266WebServer server(80);
static DNSServer dnsServer;
static const uint16_t DNS_PORT = 53;
static ERaConfig_t eraConfig{};
static const ERaConfig_t eraDefault = {
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

static WiFiUDP Udp;
static ERaUdp<WiFiUDP> eraUdp(Udp);

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
        this->transp.config(host, port, username, password);
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
            CopyToArray(ssid, eraConfig.ssid);
            CopyToArray(pass, eraConfig.pass);
            CopyToArray(auth, eraConfig.token);
            CopyToArray(host, eraConfig.host);
            eraConfig.port = port;
            CopyToArray(username, eraConfig.username);
            CopyToArray(password, eraConfig.password);
            eraConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
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
        this->begin(ERA_MQTT_CLIENT_ID, ssid, pass,
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
    void getWiFiName(char* ptr, size_t size, bool withPrefix = true);
    void getImeiChip(char* ptr, size_t size);

    bool connected() {
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
    if (!eraConfig.getFlag(ConfigFlagT::CONFIG_FLAG_API) &&
        configured) {
        return;
    }
    configured = true;
    eraConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, false);

    ERA_LOG(TAG, ERA_PSTR("Config api"));

    char imei[64] {0};
    char ssidAP[64] {0};
    this->getWiFiName(ssidAP, sizeof(ssidAP));
    this->getImeiChip(imei, sizeof(imei));

    dnsServer.setTTL(300);
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

    server.on("/api/config", HTTP_POST, [imei]() {
        ERA_LOG(ERA_PSTR("Config"), ERA_PSTR("Configuration..."));
        String ssid = server.arg("ssid");
        String pass = server.arg("pass");
        String backupSsid = server.arg("backup_ssid");
        String backupPass = server.arg("backup_pass");

        String token = server.arg("token");
        String host = server.arg("host");
        String port = server.arg("port");
        String user = server.arg("user");
        String password = server.arg("password");

        String content;

        if (ssid.length()) {
            CopyToArray(ssid, eraConfig.ssid);
            CopyToArray(pass, eraConfig.pass);
        }

        if (backupSsid.length()) {
            eraConfig.hasBackup = true;
            CopyToArray(backupSsid, eraConfig.backupSSID);
            CopyToArray(backupPass, eraConfig.backupPass);
        } else {
            eraConfig.hasBackup = false;
        }

        if (token.length()) {
            CopyToArray(token, eraConfig.token);
        }
        else {
            CopyToArray(imei, eraConfig.token);
        }
        if (host.length()) {
            CopyToArray(host, eraConfig.host);
        }
        if (port.length()) {
            eraConfig.port = port.toInt();
        }
        if (user.length()) {
            CopyToArray(user, eraConfig.username);
        }
        else {
            CopyToArray(imei, eraConfig.username);
        }
        if (password.length()) {
            CopyToArray(password, eraConfig.password);
        }
        else {
            CopyToArray(imei, eraConfig.password);
        }

        content = ERA_F(R"json({"status":"ok","message":"Connecting wifi..."})json");
        server.send(200, "application/json", content);
        if (ssid.length()) {
            eraConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            eraConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
            ERaState::set(StateT::STATE_SWITCH_TO_STA);
        }
    });

    server.on("/api/board_info.json", HTTP_POST, [imei, ssidAP]() {
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
        String content = ERA_FPSTR(webIndex) + String(ERA_FPSTR(webStyle));
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
            CopyToArray(ssid, eraConfig.ssid);
            CopyToArray(pass, eraConfig.pass);
        }

        if (backupSsid.length()) {
            eraConfig.hasBackup = true;
            CopyToArray(backupSsid, eraConfig.backupSSID);
            CopyToArray(backupPass, eraConfig.backupPass);
        } else {
            eraConfig.hasBackup = false;
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
        content += ERA_F("<tr><td style='text-align:center'>") + String(strlen(eraConfig.ssid) ? eraConfig.ssid : "--");
        content += ERA_F("</td><td style='text-align:center'>") + String(strlen(eraConfig.backupSSID) ? eraConfig.backupSSID : "--");
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
            eraConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            eraConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
            ERaState::set(StateT::STATE_SWITCH_TO_STA);
        }
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

    eraUdp.on("connect/wifi", [imei]() {
        ERA_LOG(ERA_PSTR("Config"), ERA_PSTR("Configuration..."));
        String ssid = eraUdp.arg("ssid").c_str();
        String pass = eraUdp.arg("pass").c_str();
        String backupSsid = eraUdp.arg("backup_ssid").c_str();
        String backupPass = eraUdp.arg("backup_pass").c_str();

        String token = eraUdp.arg("token").c_str();
        String host = eraUdp.arg("host").c_str();
        String port = eraUdp.arg("port").c_str();
        String user = eraUdp.arg("user").c_str();
        String password = eraUdp.arg("password").c_str();

        String content;

        if (ssid.length()) {
            CopyToArray(ssid, eraConfig.ssid);
            CopyToArray(pass, eraConfig.pass);
        }

        if (backupSsid.length()) {
            eraConfig.hasBackup = true;
            CopyToArray(backupSsid, eraConfig.backupSSID);
            CopyToArray(backupPass, eraConfig.backupPass);
        } else {
            eraConfig.hasBackup = false;
        }

        if (token.length()) {
            CopyToArray(token, eraConfig.token);
        }
        else {
            CopyToArray(imei, eraConfig.token);
        }
        if (host.length()) {
            CopyToArray(host, eraConfig.host);
        }
        if (port.length()) {
            eraConfig.port = port.toInt();
        }
        if (user.length()) {
            CopyToArray(user, eraConfig.username);
        }
        else {
            CopyToArray(imei, eraConfig.username);
        }
        if (password.length()) {
            CopyToArray(password, eraConfig.password);
        }
        else {
            CopyToArray(imei, eraConfig.password);
        }

        content = ERA_F(R"json({"status":"ok","message":"Connecting wifi..."})json");
        eraUdp.send(content.c_str());
        if (ssid.length()) {
            eraConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            eraConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
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
    if (eraConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
        ERaState::set(StateT::STATE_CONNECTING_NETWORK);
    }
    else {
        ERaState::set(StateT::STATE_SWITCH_TO_AP);
        eraConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
    }
}

template <class Transport>
void ERaPnP<Transport>::configLoad() {
    if (eraConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
        return;
    }
    memset(&eraConfig, 0, sizeof(eraConfig));
    Base::ERaApi::flash.readFlash("config", &eraConfig, sizeof(eraConfig));
    ERA_LOG(TAG, ERA_PSTR("Configuration loaded from flash"));
    if (eraConfig.magic != eraDefault.magic) {
        this->configLoadDefault();
    }
}

template <class Transport>
void ERaPnP<Transport>::configLoadDefault() {
    char imei[64] {0};
    eraConfig = eraDefault;
    this->getImeiChip(imei, sizeof(imei));
    CopyToArray(imei, eraConfig.token);
    CopyToArray(imei, eraConfig.username);
    CopyToArray(imei, eraConfig.password);
    ERA_LOG(TAG, ERA_PSTR("Using default config"));
}

template <class Transport>
void ERaPnP<Transport>::configSave() {
    if (!eraConfig.getFlag(ConfigFlagT::CONFIG_FLAG_STORE)) {
        return;
    }
    eraConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, false);
    Base::ERaApi::flash.writeFlash("config", &eraConfig, sizeof(eraConfig));
    ERA_LOG(TAG, ERA_PSTR("Configuration stored to flash"));
}

template <class Transport>
void ERaPnP<Transport>::configReset() {
    this->configLoadDefault();
    eraConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
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
        if (eraConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
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
    this->getWiFiName(ssidAP, sizeof(ssidAP));
    WiFi.setHostname(ssidAP);

    this->connectWiFi(eraConfig.ssid, eraConfig.pass);
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
        eraConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
    }
}

template <class Transport>
void ERaPnP<Transport>::connectCloud() {
    ERA_LOG(TAG, ERA_PSTR("Connecting cloud..."));
    this->config(eraConfig.token, eraConfig.host, eraConfig.port, eraConfig.username, eraConfig.password);
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
    this->transp.setSSID(ssid);
    ERA_LOG(TAG, ERA_PSTR("Connected to WiFi"));

    IPAddress localIP = WiFi.localIP();
    ERA_FORCE_UNUSED(localIP);
    ERA_LOG(TAG, ERA_PSTR("IP: %s"), localIP.toString().c_str());
}

template <class Transport>
void ERaPnP<Transport>::switchToAP() {
    ERA_LOG(TAG, ERA_PSTR("Switching to AP..."));

    char ssidAP[64] {0};
    this->getWiFiName(ssidAP, sizeof(ssidAP));

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
void ERaPnP<Transport>::getWiFiName(char* ptr, size_t size, bool withPrefix) {
	String macAddr = WiFi.macAddress();
	macAddr.replace(":", "");
	macAddr.toLowerCase();
    if (withPrefix) {
        snprintf(ptr, size, "eoh.era.%s", macAddr.c_str());
    } else {
        snprintf(ptr, size, "era.%s", macAddr.c_str());
    }
}

template <class Transport>
void ERaPnP<Transport>::getImeiChip(char* ptr, size_t size) {
	String macAddr = WiFi.macAddress();
	macAddr.replace(":", "");
	macAddr.toLowerCase();
#ifdef ERA_AUTH_TOKEN
    snprintf(ptr, size, ERA_AUTH_TOKEN);
#else
    if (this->authToken != nullptr) {
        snprintf(ptr, size, this->authToken);
    }
    else {
        snprintf(ptr, size, "ERA-%s", macAddr.c_str());
    }
#endif
}

#include <Network/ERaWiFiEsp8266.hpp>

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().ERA_AUTH);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, WiFi.SSID().c_str());
    cJSON_AddStringToObject(root, INFO_BSSID, WiFi.BSSIDstr().c_str());
    cJSON_AddNumberToObject(root, INFO_RSSI, WiFi.RSSI());
    cJSON_AddStringToObject(root, INFO_MAC, WiFi.macAddress().c_str());
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, WiFi.localIP().toString().c_str());
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().transp.getPing());
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, WiFi.RSSI());
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, WiFi.SSID().c_str());
}

#endif /* INC_ERA_PNP_ESP8266_HPP_ */
