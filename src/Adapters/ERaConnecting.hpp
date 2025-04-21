#ifndef INC_ERA_CONNECTING_HPP_
#define INC_ERA_CONNECTING_HPP_

#if defined(ESP32)
    #include <ESPmDNS.h>
    #define ERA_HAS_MDNS_H
#elif defined(ESP8266)
    #include <ESP8266mDNS.h>
    #define ERA_HAS_MDNS_H
#elif defined(RTL8722DM) && \
    defined(ARDUINO_AMEBA)
    #include <AmebaMDNS.h>
    #define ERA_HAS_AMEBA_MDNS_H
#endif

#if defined(ERA_HAS_MDNS_H)
#elif defined(ERA_HAS_AMEBA_MDNS_H)
#elif defined(__has_include)
    #if __has_include(<ArduinoMDNS.h>)
        #include <ArduinoMDNS.h>
        #define ERA_HAS_ARDUINO_MDNS_H
    #endif
#endif

#include <ERa/ERaProtocol.hpp>

#if defined(ERA_HAS_AMEBA_MDNS_H)
    #define MDNS_SERVICE              "_plugandplay"
    #define MDNS_PROTOCOL             "_tcp"
#elif defined(ERA_HAS_ARDUINO_MDNS_H)
    #define MDNS_SERVICE              "_plugandplay"
    #define MDNS_PROTOCOL             MDNSServiceTCP
#else
    #define MDNS_SERVICE              "plugandplay"
    #define MDNS_PROTOCOL             "tcp"
#endif

#define MDNS_PORT                     80

typedef struct __ERaConfig_t {
    uint32_t magic;
    uint8_t flags;

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

static const ERaConfig_t ERaDefault = {
    0x27061995,
    0x00,

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

template <class Server, class Client,
          class UDP, class Flash>
class ERaConnecting
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> FunctionCallback_t;
#else
    typedef void (*FunctionCallback_t)(void);
#endif
    enum PnPRequestT {
        PNP_REQ_BOARD_INFO = 0,
        PNP_REQ_SCAN_WIFI = 1,
        PNP_REQ_CONFIG = 2,
        PNP_REQ_RESET = 3,
        PNP_REQ_REBOOT = 4,
        PNP_REQ_IDENTIFY = 5,
        PNP_REQ_MAX = 255,
    };

    const char* TAG = "Connecting";

public:
    ERaConnecting(Flash& flash)
        : mFlash(flash)
        , mServer(80U)
        , mIP(0, 0, 0, 0)
        , mDeviceName(ERA_DEVICE_NAME)
        , mDeviceType(ERA_DEVICE_TYPE)
        , mDeviceSecret(ERA_DEVICE_SECRET)
#if defined(ERA_HAS_AMEBA_MDNS_H)
        , mService(nullptr)
#elif defined(ERA_HAS_ARDUINO_MDNS_H)
        , mMDNS(mUDP)
#endif
    {}

    void setIdentifyCallbacks(FunctionCallback_t callback) {
        this->mIdentifyCb = callback;
    }

protected:
    void assignIPAddress(const IPAddress& ip) {
        this->mIP = ip;
    }

    void assignDeviceName(const char* name) {
        this->mDeviceName = name;
    }

    void assignDeviceType(const char* type) {
        this->mDeviceType = type;
    }

    void assignDeviceSecretKey(const char* key) {
        this->mDeviceSecret = key;
    }

    void setConnected(bool connected) {
        ERaConfig.connected = connected;
    }

    void configMode() {
        ERA_LOG(TAG, ERA_PSTR("Config mode"));

        ERaWatchdogFeed();

        this->mServer.begin();

#if defined(ERA_HAS_MDNS_H)
        if (MDNS.begin(this->mDeviceName)) {
            MDNS.addService(MDNS_SERVICE, MDNS_PROTOCOL, MDNS_PORT);
            MDNS.addServiceTxt(MDNS_SERVICE, MDNS_PROTOCOL,
                            "path", "/");
            MDNS.addServiceTxt(MDNS_SERVICE, MDNS_PROTOCOL,
                            INFO_DEVICE_NAME, this->mDeviceName);
            MDNS.addServiceTxt(MDNS_SERVICE, MDNS_PROTOCOL,
                            INFO_DEVICE_TYPE, this->mDeviceType);
            MDNS.addServiceTxt(MDNS_SERVICE, MDNS_PROTOCOL,
                            INFO_DEVICE_SECRET, this->mDeviceSecret);
        }
#elif defined(ERA_HAS_AMEBA_MDNS_H)
        this->mService = new MDNSService((char*)this->mDeviceName,
                                        MDNS_SERVICE "." MDNS_PROTOCOL, "local",
                                        MDNS_PORT, 1024);
        if (this->mService != nullptr) {
            this->mService->addTxtRecord("path", strlen("/"), "/");
            this->mService->addTxtRecord(INFO_DEVICE_NAME, strlen(this->mDeviceName),
                                        (char*)this->mDeviceName);
            this->mService->addTxtRecord(INFO_DEVICE_TYPE, strlen(this->mDeviceType),
                                        (char*)this->mDeviceType);
            this->mService->addTxtRecord(INFO_DEVICE_SECRET, strlen(this->mDeviceSecret),
                                        (char*)this->mDeviceSecret);
            MDNS.begin();
            MDNS.registerService(*this->mService);
        }
#elif defined(ERA_HAS_ARDUINO_MDNS_H)
        if (this->mMDNS.begin(this->mIP, this->mDeviceName)) {
            /* this->mMDNS.addServiceRecord(this->getMdnsService().c_str(), MDNS_PORT,
                                         MDNS_PROTOCOL); */
            this->mMDNS.addServiceRecord(this->getMdnsService().c_str(), MDNS_PORT,
                                         MDNS_PROTOCOL, this->getTxtRecords().c_str());
        }
#endif

        while (ERaState::is(StateT::STATE_SCAN_DEVICE)) {
            ERaDelay(10);
            this->runServer();
            ERaWatchdogFeed();
            this->handleScanDevice();
        }

        ERaDelay(1000);

#if defined(ERA_HAS_MDNS_H)
        MDNS.end();
#elif defined(ERA_HAS_AMEBA_MDNS_H)
        if (this->mService != nullptr) {
            MDNS.deregisterService(*this->mService);
            MDNS.end();
            delete this->mService;
            this->mService = nullptr;
        }
#elif defined(ERA_HAS_ARDUINO_MDNS_H)
        this->mMDNS.removeAllServiceRecords();
#endif
    }

    void runServer() {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        Client client = this->mServer.available();
#pragma GCC diagnostic pop
        if (!client) {
            return;
        }

        PnPRequestT req = PnPRequestT::PNP_REQ_MAX;
        String currentLine = "";
        String configLine = "";
        String contentConfig = "";
        int contentLength {0};
        bool hasBody {false};
        while (client.connected()) {
            if (!client.available() && !hasBody) {
                ERaDelay(10);
                this->handleScanDevice();
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
                        case PnPRequestT::PNP_REQ_CONFIG: {
                            ERA_LOG(ERA_PSTR("Config"), ERA_PSTR("Configuration..."));

                            String token = this->findArg(configLine, contentConfig, "token");
                            String host = this->findArg(configLine, contentConfig, "host");
                            String port = this->findArg(configLine, contentConfig, "port");
                            String username = this->findArg(configLine, contentConfig, "username");
                            String password = this->findArg(configLine, contentConfig, "password");

                            String save = this->findArg(configLine, contentConfig, "save");
                            ERaConfig.forceSave = !!save.toInt();

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

                            contentType = ERA_F("application/json");
                            if (!token.length()) {
                                response = ERA_F("400 Bad Request");
                                content = ERA_F(R"json({"status":"error","message":"Invalid token"})json");
                                break;
                            }
                            if (ERaConfig.forceSave) {
                                content = ERA_F(R"json({"status":"ok","message":"Configuration saved"})json");
                            }
                            else {
                                content = ERA_F(R"json({"status":"ok","message":"Connecting cloud..."})json");
                            }

                            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_PNP, true);
                            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_VALID, true);
                            ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
                            ERaState::set(StateT::STATE_SWITCH_TO_STA);
                            if (ERaConfig.forceSave) {
                                this->configSave();
                            }
                        }
                            break;
                        case PnPRequestT::PNP_REQ_BOARD_INFO: {
                            ERA_LOG(ERA_PSTR("Info"), ERA_PSTR("Board info"));
                            cJSON* root = cJSON_CreateObject();
                            if (root != nullptr) {
                                cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
                                cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
                                cJSON_AddStringToObject(root, INFO_DEVICE_NAME, this->mDeviceName);
                                cJSON_AddStringToObject(root, INFO_DEVICE_TYPE, this->mDeviceType);
                                cJSON_AddStringToObject(root, INFO_DEVICE_SECRET, this->mDeviceSecret);
                                cJSON_AddStringToObject(root, INFO_BUILD_DATE, BUILD_DATE_TIME);
                                cJSON_AddStringToObject(root, INFO_VERSION, ERA_VERSION);
                                cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
                                cJSON_AddStringToObject(root, INFO_LOCAL_IP, this->getLocalIP().c_str());
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
                                this->mIdentifyCb();
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
                            content = ERA_F("[]");
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
                            this->mIdentifyCb();
                        }
                            break;
                        default:
                            break;
                    }

                    hasBody = false;

                    client.println(ERA_F("HTTP/1.1 ") + response);
                    client.println(ERA_F("Content-Type:") + contentType);
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
            else if (currentLine.indexOf(" /api/identify") >= 0) {
                req = PnPRequestT::PNP_REQ_IDENTIFY;
            }
        }

        client.stop();
    }

    void configLoad() {
        if (ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
            return;
        }
        memset(&ERaConfig, 0, sizeof(ERaConfig));
        this->mFlash.readFlash("config", &ERaConfig, sizeof(ERaConfig));
        ERA_LOG(TAG, ERA_PSTR("Configuration loaded from flash"));
        if (ERaConfig.magic != ERaDefault.magic) {
            this->configLoadDefault();
        }
        if (!ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_VALID)) {
            ERaState::set(StateT::STATE_SCAN_DEVICE);
        }
    }

    void configLoadDefault() {
        ERaConfig = ERaDefault;
        ERA_LOG(TAG, ERA_PSTR("Using default config"));
    }

    void configSave() {
        if (!ERaConfig.getFlag(ConfigFlagT::CONFIG_FLAG_STORE)) {
            return;
        }
        ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, false);
        this->mFlash.writeFlash("config", &ERaConfig, sizeof(ERaConfig));
        ERA_LOG(TAG, ERA_PSTR("Configuration stored to flash"));
    }

    void configReset() {
        this->configLoadDefault();
        ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_API, true);
        ERaConfig.setFlag(ConfigFlagT::CONFIG_FLAG_STORE, true);
        this->configSave();
        ERaState::set(StateT::STATE_SCAN_DEVICE);
        ERA_LOG(TAG, ERA_PSTR("Resetting configuration"));
    }

private:
#if defined(ERA_HAS_MDNS_H)
    void handleScanDevice() {
    #if defined(ESP8266)
        MDNS.update();
    #endif
    }
#elif defined(ERA_HAS_ARDUINO_MDNS_H)
    void handleScanDevice() {
        this->mMDNS.run();
    }
#else
    void handleScanDevice() {
    }
#endif

    String getLocalIP() {
        char ip[20] {0};
        FormatString(ip, "%u.%u.%u.%u", this->mIP[0], this->mIP[1],
                                        this->mIP[2], this->mIP[3]);
        return String(ip);
    }

#if defined(ERA_HAS_ARDUINO_MDNS_H)
    String getMdnsService() {
        String service = this->mDeviceName;
        service += ".";
        service += MDNS_SERVICE;
        return service;
    }

    String getTxtDeviceName() {
        String record;
        String name = INFO_DEVICE_NAME;
        name += "=";
        name += this->mDeviceName;
        record += (char)name.length();
        record += name;
        return record;
    }

    String getTxtDeviceType() {
        String record; 
        String type = INFO_DEVICE_TYPE;
        type += "=";
        type += this->mDeviceType;
        record += (char)type.length();
        record += type;
        return record;
    }

    String getTxtDeviceSecretKey() {
        String record;
        String secret = INFO_DEVICE_SECRET;
        secret += "=";
        secret += this->mDeviceSecret;
        record += (char)secret.length();
        record += secret;
        return record;
    }

    String getTxtRecords() {
        String records = "\x6path=/";
        records += this->getTxtDeviceName();
        records += this->getTxtDeviceType();
        records += this->getTxtDeviceSecretKey();
        return records;
    }
#endif

    String urlDecode(const String& text) {
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

    String findArg(const String& url, const String& body, const String& arg) {
        String value = this->urlFindArg(url, arg);
        if (!value.length()) {
            value = this->bodyFindArg(body, arg);
        }
        return value;
    }

    String urlFindArg(const String& url, const String& arg) {
        int s = url.indexOf("&" + arg + "=");
        if (s < 0) {
            return "";
        }
        int sLen = arg.length() + 2;
        int e = url.indexOf('&', s + sLen);
        return this->urlDecode(url.substring(s + sLen, e));
    }

    String bodyFindArg(const String& body, const String& arg) {
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

    static inline
    void functionCallbackDummy() {
    }

    FunctionCallback_t mIdentifyCb = this->functionCallbackDummy;

    Flash& mFlash;
    Server mServer;
    IPAddress mIP;

    const char* mDeviceName;
    const char* mDeviceType;
    const char* mDeviceSecret;

#if defined(ERA_HAS_AMEBA_MDNS_H)
    MDNSService* mService;
#elif defined(ERA_HAS_ARDUINO_MDNS_H)
    MDNS mMDNS;
    UDP mUDP;
#endif
};

#endif /* INC_ERA_CONNECTING_HPP_ */
