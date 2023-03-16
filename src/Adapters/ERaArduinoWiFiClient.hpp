#ifndef INC_ERA_ARDUINO_WIFI_CLIENT_HPP_
#define INC_ERA_ARDUINO_WIFI_CLIENT_HPP_

#if !defined(ERA_PROTO_TYPE)
    #define ERA_PROTO_TYPE            "WiFi"
#endif

#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqtt.hpp>

#define WIFI_NET_CONNECT_TIMEOUT      20000

typedef struct __ERaConfig_t {
    char ssid[64];
    char pass[64];
} ERA_ATTR_PACKED ERaConfig_t;

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaConfig_t ERaConfig {};
#else
    #define ERaConfig  StaticHelper<ERaConfig_t>::instance()
#endif

class ERaFlash;

#if defined(ESP32) || defined(ESP8266)

    static inline
    String getSSID() {
        return WiFi.SSID();
    }

    static inline
    String getBSSID() {
        return WiFi.BSSIDstr();
    }

    static inline
    String getMAC() {
        return WiFi.macAddress();
    }

    static inline
    String getLocalIP() {
        return WiFi.localIP().toString();
    }

#else

    static inline
    String getSSID() {
        return String(WiFi.SSID());
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

#endif

template <class Transport>
class ERaWiFi
    : public ERaProto<Transport, ERaFlash>
{
    const char* TAG = "WiFi";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;

public:
    ERaWiFi(Transport& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , authToken(nullptr)
    {}
    ~ERaWiFi()
    {}

    bool connectWiFi(const char* ssid, const char* pass) {
        int status = WiFi.status();

        if (status == WL_NO_SHIELD) {
            ERA_LOG(TAG, ERA_PSTR("WiFi module not found"));
        }

        while (status != WL_CONNECTED) {
            ERA_LOG(TAG, ERA_PSTR("Connecting to %s..."), ssid);
            if (pass && strlen(pass)) {
                status = WiFi.begin((char*)ssid, (char*)pass);
            } else {
                status = WiFi.begin((char*)ssid);
            }

            MillisTime_t startMillis = ERaMillis();
            while (status != WL_CONNECTED) {
                ERaDelay(500);
                status = WiFi.status();
                if (!ERaRemainingTime(startMillis, WIFI_NET_CONNECT_TIMEOUT)) {
                    WiFi.disconnect();
                    break;
                }
            }
        }
        this->getTransp().setSSID(ssid);
        ERA_LOG(TAG, ERA_PSTR("Connected to WiFi"));
        ERA_LOG(TAG, ERA_PSTR("IP: %s"), ::getLocalIP().c_str());
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

    void begin(const char* auth,
                const char* ssid,
                const char* pass,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectWiFi(ssid, pass);
        this->setWiFi(ssid, pass);
    }

    void begin(const char* ssid,
                const char* pass) {
        this->begin(ERA_AUTHENTICATION_TOKEN, ssid, pass,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

    void run() {
        switch (ERaState::get()) {
            case StateT::STATE_CONNECTING_CLOUD:
                if (Base::connect()) {
                    ERaOptConnected(this);
                    ERaState::set(StateT::STATE_CONNECTED);
                }
                break;
            case StateT::STATE_CONNECTED:
                ERaState::set(StateT::STATE_RUNNING);
                break;
            case StateT::STATE_RUNNING:
                Base::run();
                break;
            default:
                if (this->connected() ||
                    this->connectWiFi(ERaConfig.ssid, ERaConfig.pass)) {
                    ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                }
                break;
        }
    }

protected:
private:
    void setWiFi(const char* ssid, const char* pass) {
        CopyToArray(ssid, ERaConfig.ssid);
        CopyToArray(pass, ERaConfig.pass);
    }

    bool connected() {
        return (WiFi.status() == WL_CONNECTED);
    }

    const char* authToken;
};

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_BOARD_ID, this->thisProto().getBoardID());
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().getAuth());
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, ::getSSID().c_str());
    cJSON_AddStringToObject(root, INFO_BSSID, ::getBSSID().c_str());
    cJSON_AddNumberToObject(root, INFO_RSSI, WiFi.RSSI());
    cJSON_AddStringToObject(root, INFO_MAC, ::getMAC().c_str());
    cJSON_AddStringToObject(root, INFO_LOCAL_IP,::getLocalIP().c_str());
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().getTransp().getPing());
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
#if defined(ESP32)
    cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, static_cast<uint16_t>(temperatureRead() * 100.0f));
#else
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
#endif
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, WiFi.RSSI());
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ::getSSID().c_str());
}

#endif /* INC_ERA_ARDUINO_WIFI_CLIENT_HPP_ */
