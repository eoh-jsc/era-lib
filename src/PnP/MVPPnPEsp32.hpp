#ifndef INC_MVP_PNP_ESP32_HPP_
#define INC_MVP_PNP_ESP32_HPP_

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <MVP/MVPProtocol.hpp>
#include <MVP/MVPApiEsp32.hpp>
#include <MQTT/MVPMqtt.hpp>
#include <Modbus/MVPModbusEsp32.hpp>
#include <Utility/MVPFlashEsp32.hpp>
#include <PnP/MVPState.hpp>

#define MVP_MODEL_TYPE                "MVP"
#define MVP_BOARD_TYPE                "ESP32"
#define CONFIG_AP_URL                 "mvp.setup"

#define WIFI_SCAN_MAX                 20
#define WIFI_SCAN_TIMEOUT             20000
#define WIFI_NET_CONNECT_TIMEOUT      60000
#define WIFI_CLOUD_CONNECT_TIMEOUT    60000
#define WIFI_NET_CHECK_TIMEOUT        WIFI_NET_CONNECT_TIMEOUT * 3
#define WIFI_AP_PASS                  "Eoh@2021"
#define WIFI_AP_IP                    IPAddress(192, 168, 27, 1)
#define WIFI_AP_Subnet                IPAddress(255, 255, 255, 0)

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

typedef struct __MVPConfig_t {
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
    char user[64];
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
} MVP_ATTR_PACKED MVPConfig_t;

static WebServer server(80);
static DNSServer dnsServer;
static const uint16_t DNS_PORT = 53;
static MVPConfig_t mvpConfig{};
static const MVPConfig_t mvpDefault = {
    .magic = 0x27061995,
    .flags = 0x00,
    "",
    "",
    "",
    "",
    false,
    "invalid",
    MVP_MQTT_HOST,
    MVP_MQTT_PORT,
    MVP_MQTT_USERNAME,
    MVP_MQTT_PASSWORD
};

enum ConfigFlagT {
    CONFIG_FLAG_VALID = 0x01,
    CONFIG_FLAG_STORE = 0x02,
    CONFIG_FLAG_API = 0x04
};

#include <Network/MVPUdpEsp32.hpp>

static WiFiUDP Udp;
static MVPUdp<WiFiUDP> mvpUdp(Udp);

class MVPPnP
    : public MVPProto< MVPMqtt<WiFiClient, MQTTClient>, MVPFlash >
{
    typedef std::function<void(WiFiEvent_t, WiFiEventInfo_t)> WiFiEventCallback_t;

    const char* TAG = "WiFi";
    const char* HOSTNAME = "MVP";
    friend class MVPProto< MVPMqtt<WiFiClient, MQTTClient>, MVPFlash >;
    typedef MVPProto< MVPMqtt<WiFiClient, MQTTClient>, MVPFlash > Base;

public:
    MVPPnP(MVPMqtt<WiFiClient, MQTTClient>& _transp, MVPFlash& _flash)
        : Base(_transp, _flash)
        , authToken(nullptr)
    {}
    ~MVPPnP()
    {}

    void config(const char* auth,
                const char* host = MVP_MQTT_HOST,
                uint16_t port = MVP_MQTT_PORT,
                const char* username = MVP_MQTT_USERNAME,
                const char* password = MVP_MQTT_PASSWORD) {
        Base::begin(auth);
        this->transp.config(host, port, username, password);
    }

    void begin(const char* auth,
                const char* ssid,
                const char* pass,
                const char* host = MVP_MQTT_HOST,
                uint16_t port = MVP_MQTT_PORT,
                const char* username = MVP_MQTT_USERNAME,
                const char* password = MVP_MQTT_PASSWORD) {
        WiFi.onEvent(this->wifiCb);
        WiFi.mode(WIFI_STA);
        this->connectWiFi(ssid, pass);
        Base::init();
        this->config(auth, host, port, username, password);
        if (Base::connect()) {
            MVPState::set(StateT::STATE_CONNECTED);
        }
        else {
            MVPState::set(StateT::STATE_SWITCH_TO_AP);
        }
    }

    void begin(const char* ssid,
                const char* pass) {
        this->begin(MQTT_CLIENT_ID, ssid, pass,
                    MVP_MQTT_HOST, MVP_MQTT_PORT,
                    MVP_MQTT_USERNAME, MVP_MQTT_PASSWORD);
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

    void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);

	WiFiEventCallback_t wifiCb = [=](WiFiEvent_t event, WiFiEventInfo_t info) {
		this->onWiFiEvent(event, info);
	};

    const char* authToken;
};

void MVPPnP::begin(const char* auth) {
    this->authToken = auth;
    WiFi.persistent(false);
    WiFi.enableSTA(true);
    WiFi.onEvent(this->wifiCb);

    Base::init();
    this->configInit();
}

void MVPPnP::run() {
    switch (MVPState::get()) {
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
            this->connectNetwork();
            break;
        case StateT::STATE_CONNECTING_CLOUD:
            this->connectCloud();
            break;
        case StateT::STATE_CONNECTED:
            MVPState::set(StateT::STATE_RUNNING);
            break;
        case StateT::STATE_RUNNING:
            if (this->connected()) {
                Base::run();
            }
            else {
                MVPState::set(StateT::STATE_CONNECTING_NETWORK);
            }
            break;
        case StateT::STATE_OTA_UPGRADE:
            MVPState::set(StateT::STATE_RUNNING);
            break;
        case StateT::STATE_RESET_CONFIG:
            this->configReset();
            break;
        case StateT::STATE_RESET_CONFIG_REBOOT:
            this->configReset();
            MVPState::set(StateT::STATE_REBOOT);
            break;
        case StateT::STATE_REBOOT:
            MVPDelay(1000);
            MVPRestart(false);
            break;
        default:
            break;
    }
}

void MVPPnP::configApi() {
    if (!mvpConfig.getFlag(ConfigFlagT::CONFIG_FLAG_API)) {
        return;
    }
    mvpConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, false);

    MVP_LOG(TAG, "Config api");

    char imei[64] {0};
    char ssidAP[64] {0};
    this->getWiFiName(ssidAP, sizeof(ssidAP));
    this->getImeiChip(imei, sizeof(imei));

    dnsServer.setTTL(300);
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

    server.on("/api/config", HTTP_POST, [imei]() {
        MVP_LOG("Config", "Configuration...");
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
            CopyToArray(ssid, mvpConfig.ssid);
            CopyToArray(pass, mvpConfig.pass);
        }

        if (backupSsid.length()) {
            mvpConfig.hasBackup = true;
            CopyToArray(backupSsid, mvpConfig.backupSSID);
            CopyToArray(backupPass, mvpConfig.backupPass);
        } else {
            mvpConfig.hasBackup = false;
        }

        if (token.length()) {
            CopyToArray(token, mvpConfig.token);
        }
        else {
            CopyToArray(imei, mvpConfig.token);
        }
        if (host.length()) {
            CopyToArray(host, mvpConfig.host);
        }
        if (port.length()) {
            mvpConfig.port = port.toInt();
        }
        if (user.length()) {
            CopyToArray(user, mvpConfig.user);
        }
        else {
            CopyToArray(imei, mvpConfig.user);
        }
        if (password.length()) {
            CopyToArray(password, mvpConfig.password);
        }
        else {
            CopyToArray(imei, mvpConfig.password);
        }

        content = R"json({"status":"ok","message":"Connecting wifi..."})json";
        server.send(200, "application/json", content);
        if (ssid.length()) {
            mvpConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            mvpConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
            MVPState::set(StateT::STATE_SWITCH_TO_STA);
        }
    });

    server.on("/api/board_info.json", HTTP_POST, [imei, ssidAP]() {
        MVP_LOG("Info", "Board info");
        String content;
        cJSON* root = cJSON_CreateObject();
        if (root != nullptr) {
            cJSON_AddStringToObject(root, "board", MVP_BOARD_TYPE);
            cJSON_AddStringToObject(root, "model", MVP_MODEL_TYPE);
            cJSON_AddStringToObject(root, "imei", imei);
            cJSON_AddStringToObject(root, "firmware_version", MVP_FIRMWARE_VERSION);
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
        MVP_LOG("WiFi", "Scanning wifi...");
        String content;
        int nets = WiFi.scanNetworks(true, true);
        unsigned long tick = MVPMillis();
        while (nets < 0 && MVPRemainingTime(tick, WIFI_SCAN_TIMEOUT)) {
            MVPDelay(100);
            nets = WiFi.scanComplete();
        }
        MVP_LOG("WiFi", "Found %d wifi", nets);

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
        String content = R"json({"status":"ok","message":"Configuration reset"})json";
        server.send(200, "application/json", content);
        MVPState::set(StateT::STATE_RESET_CONFIG);
    });

    server.on("/api/reboot", HTTP_POST, []() {
        String content = R"json({"status":"ok","message":"Rebooting..."})json";
        server.send(200, "application/json", content);
        MVPState::set(StateT::STATE_REBOOT);
    });

    mvpUdp.on("connect/wifi", [imei]() {
        MVP_LOG("Config", "Configuration...");
        String ssid = mvpUdp.arg("ssid").c_str();
        String pass = mvpUdp.arg("pass").c_str();
        String backupSsid = mvpUdp.arg("backup_ssid").c_str();
        String backupPass = mvpUdp.arg("backup_pass").c_str();

        String token = mvpUdp.arg("token").c_str();
        String host = mvpUdp.arg("host").c_str();
        String port = mvpUdp.arg("port").c_str();
        String user = mvpUdp.arg("user").c_str();
        String password = mvpUdp.arg("password").c_str();

        String content;

        if (ssid.length()) {
            CopyToArray(ssid, mvpConfig.ssid);
            CopyToArray(pass, mvpConfig.pass);
        }

        if (backupSsid.length()) {
            mvpConfig.hasBackup = true;
            CopyToArray(backupSsid, mvpConfig.backupSSID);
            CopyToArray(backupPass, mvpConfig.backupPass);
        } else {
            mvpConfig.hasBackup = false;
        }

        if (token.length()) {
            CopyToArray(token, mvpConfig.token);
        }
        else {
            CopyToArray(imei, mvpConfig.token);
        }
        if (host.length()) {
            CopyToArray(host, mvpConfig.host);
        }
        if (port.length()) {
            mvpConfig.port = port.toInt();
        }
        if (user.length()) {
            CopyToArray(user, mvpConfig.user);
        }
        else {
            CopyToArray(imei, mvpConfig.user);
        }
        if (password.length()) {
            CopyToArray(password, mvpConfig.password);
        }
        else {
            CopyToArray(imei, mvpConfig.password);
        }

        content = R"json({"status":"ok","message":"Connecting wifi..."})json";
        mvpUdp.send(content.c_str());
        if (ssid.length()) {
            mvpConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
            mvpConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
            MVPState::set(StateT::STATE_SWITCH_TO_AP_STA);
        }
    });

    mvpUdp.on("scan/wifi", []() {
        MVP_LOG("WiFi", "Scanning wifi...");
        String content;
        int nets = WiFi.scanNetworks(true, true);
        unsigned long tick = MVPMillis();
        while (nets < 0 && MVPRemainingTime(tick, WIFI_SCAN_TIMEOUT)) {
            MVPDelay(100);
            nets = WiFi.scanComplete();
        }
        MVP_LOG("WiFi", "Found %d wifi", nets);

        if (nets <= 0) {
            mvpUdp.send("[]");
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
            mvpUdp.send("[]");
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
        mvpUdp.send(content.c_str());
    });

    mvpUdp.on("system/reset", []() {
        String content = R"json({"status":"ok","message":"Configuration reset"})json";
        mvpUdp.send(content.c_str());
        MVPState::set(StateT::STATE_RESET_CONFIG);
    });

    mvpUdp.on("system/reboot", []() {
        String content = R"json({"status":"ok","message":"Rebooting..."})json";
        mvpUdp.send(content.c_str());
        MVPState::set(StateT::STATE_REBOOT);
    });
}

void MVPPnP::configInit() {
    this->configLoad();
    if (mvpConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
        MVPState::set(StateT::STATE_CONNECTING_NETWORK);
    }
    else {
        MVPState::set(StateT::STATE_SWITCH_TO_AP);
    }
}

void MVPPnP::configLoad() {
    memset(&mvpConfig, 0, sizeof(mvpConfig));
    Base::MVPApi::flash.readFlash("config", &mvpConfig, sizeof(mvpConfig));
    MVP_LOG(TAG, "Configuration loaded from flash");
    if (mvpConfig.magic != mvpDefault.magic) {
        this->configLoadDefault();
    }
    mvpConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
}

void MVPPnP::configLoadDefault() {
    char imei[64] {0};
    mvpConfig = mvpDefault;
    this->getImeiChip(imei, sizeof(imei));
    CopyToArray(imei, mvpConfig.token);
    CopyToArray(imei, mvpConfig.user);
    CopyToArray(imei, mvpConfig.password);
    MVP_LOG(TAG, "Using default config");
}

void MVPPnP::configSave() {
    if (!mvpConfig.getFlag(ConfigFlagT::CONFIG_FLAG_STORE)) {
        return;
    }
    mvpConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, false);
    Base::MVPApi::flash.writeFlash("config", &mvpConfig, sizeof(mvpConfig));
    MVP_LOG(TAG, "Configuration stored to flash");
}

void MVPPnP::configReset() {
    this->configLoadDefault();
    mvpConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
    this->configSave();
    MVPState::set(StateT::STATE_SWITCH_TO_AP);
    MVP_LOG(TAG, "Resetting configuration");
}

void MVPPnP::configMode() {
    this->configApi();

    MVP_LOG(TAG, "Config mode");

    dnsServer.start(DNS_PORT, CONFIG_AP_URL, WiFi.softAPIP());
    MVP_LOG(TAG, "AP URL: %s", CONFIG_AP_URL);

    server.begin();
    mvpUdp.begin(this->authToken);

    unsigned long tick = MVPMillis();
    while (MVPState::is(StateT::STATE_WAIT_CONFIG) || MVPState::is(StateT::STATE_CONFIGURING)) {
        MVPDelay(10);
        mvpUdp.run();
        server.handleClient();
        dnsServer.processNextRequest();
        if (MVPState::is(StateT::STATE_CONFIGURING) && !WiFi.softAPgetStationNum()) {
            MVPState::set(StateT::STATE_WAIT_CONFIG);
        }
        if (mvpConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
            if (!MVPRemainingTime(tick, WIFI_NET_CHECK_TIMEOUT)) {
                MVPState::set(StateT::STATE_SWITCH_TO_STA);
                break;
            }
        }
    }

    MVPDelay(1000);
    server.stop();
    dnsServer.stop();
}

void MVPPnP::connectNetwork() {
    char ssidAP[64] {0};
    this->getWiFiName(ssidAP, sizeof(ssidAP));
    WiFi.setHostname(ssidAP);

    this->connectWiFi(mvpConfig.ssid, mvpConfig.pass);
    if (this->connected()) {
        /* Udp */
        mvpUdp.sendBoardInfo();
        mvpUdp.stop();
        MVPDelay(100);
        WiFi.mode(WIFI_STA);
        /* Udp */
        MVPState::set(StateT::STATE_CONNECTING_CLOUD);
    }
    else {
        MVPState::set(StateT::STATE_SWITCH_TO_AP);
    }
}

void MVPPnP::connectCloud() {
    MVP_LOG(TAG, "Connecting cloud...");
    this->config(mvpConfig.token, mvpConfig.host, mvpConfig.port, mvpConfig.user, mvpConfig.password);
    if (Base::connect()) {
        this->configSave();
        MVPState::set(StateT::STATE_CONNECTED);
    }
    else {
        MVPState::set(StateT::STATE_CONNECTING_NETWORK);
    }
}

void MVPPnP::connectWiFi(const char* ssid, const char* pass) {
    MVP_LOG(TAG, "Connecting to %s...", ssid);
    if (pass && strlen(pass)) {
        WiFi.begin(ssid, pass);
    } else {
        WiFi.begin(ssid);
    }
    unsigned long prevMillis = MVPMillis();
    while (WiFi.status() != WL_CONNECTED) {
        MVPDelay(500);
        if (!MVPRemainingTime(prevMillis, WIFI_NET_CONNECT_TIMEOUT)) {
            WiFi.disconnect();
            return;
        }
    }
    MVP_LOG(TAG, "Connected to WiFi");

    IPAddress localIP = WiFi.localIP();
    MVP_FORCE_UNUSED(localIP);
    MVP_LOG(TAG, "IP: %s", localIP.toString().c_str());
}

void MVPPnP::switchToAP() {
    MVP_LOG(TAG, "Switching to AP...");

    char ssidAP[64] {0};
    this->getWiFiName(ssidAP, sizeof(ssidAP));

    WiFi.mode(WIFI_OFF);
    MVPDelay(100);
    WiFi.mode(WIFI_AP);
    MVPDelay(2000);
    WiFi.softAPConfig(WIFI_AP_IP, WIFI_AP_IP, WIFI_AP_Subnet);
    WiFi.softAP(ssidAP, WIFI_AP_PASS);
    MVPDelay(500);

    IPAddress myIP = WiFi.softAPIP();
    MVP_LOG(TAG, "AP SSID: %s", ssidAP);
    MVP_LOG(TAG, "AP IP: %s", myIP.toString().c_str());

    MVPState::set(StateT::STATE_WAIT_CONFIG);
}

void MVPPnP::switchToSTA() {
    MVP_LOG(TAG, "Switching to STA...");

    MVPDelay(1000);
    WiFi.mode(WIFI_OFF);
    MVPDelay(100);
    WiFi.mode(WIFI_STA);
    
    MVPState::set(StateT::STATE_CONNECTING_NETWORK);
}

void MVPPnP::switchToAPSTA() {
    MVP_LOG(TAG, "Switching to AP STA...");

    MVPDelay(100);
    WiFi.mode(WIFI_AP_STA);
    
    MVPState::set(StateT::STATE_CONNECTING_NETWORK);
}

void MVPPnP::getWiFiName(char* ptr, size_t size, bool withPrefix) {
    const uint64_t chipId = ESP.getEfuseMac();
    uint64_t unique {0};
    for (int i = 0; i < 41; i = i + 8) {
        unique |= ((chipId >> (40 - i)) & 0xff) << i;
    }
    if (withPrefix) {
        snprintf(ptr, size, "eoh.mvp.%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    } else {
        snprintf(ptr, size, "mvp.%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    }
}

void MVPPnP::getImeiChip(char* ptr, size_t size) {
    const uint64_t chipId = ESP.getEfuseMac();
    uint64_t unique {0};
    for (int i = 0; i < 41; i = i + 8) {
        unique |= ((chipId >> (40 - i)) & 0xff) << i;
    }
#ifdef MVP_AUTH_TOKEN
    snprintf(ptr, size, MVP_AUTH_TOKEN);
#else
    if (this->authToken != nullptr) {
        snprintf(ptr, size, this->authToken);
    }
    else {
        snprintf(ptr, size, "MVP-%04x%08x", static_cast<uint16_t>(unique >> 32), static_cast<uint32_t>(unique));
    }
#endif
}

#include <Network/MVPWiFiEsp32.hpp>

template <class Proto, class Flash>
void MVPApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, "board", MVP_BOARD_TYPE);
    cJSON_AddStringToObject(root, "model", MVP_MODEL_TYPE);
	cJSON_AddStringToObject(root, "auth_token", static_cast<Proto*>(this)->MVP_AUTH);
    cJSON_AddStringToObject(root, "firmware_version", MVP_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, "ssid", WiFi.SSID().c_str());
    cJSON_AddStringToObject(root, "bssid", WiFi.BSSIDstr().c_str());
    cJSON_AddNumberToObject(root, "rssi", WiFi.RSSI());
    cJSON_AddStringToObject(root, "mac", WiFi.macAddress().c_str());
    cJSON_AddStringToObject(root, "ip", WiFi.localIP().toString().c_str());
}

template <class Proto, class Flash>
void MVPApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, "chip_temperature", static_cast<uint16_t>((temprature_sens_read() - 32) * 100.0f / 1.8f));
	cJSON_AddNumberToObject(root, "temperature", 0);
	cJSON_AddNumberToObject(root, "voltage", 999);
	cJSON_AddNumberToObject(root, "is_battery", 0);
	cJSON_AddNumberToObject(root, "rssi", WiFi.RSSI());
	cJSON_AddStringToObject(root, "wifi_is_using", WiFi.SSID().c_str());
}

#endif /* INC_MVP_PNP_ESP32_HPP_ */