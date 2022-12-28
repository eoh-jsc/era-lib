#ifndef INC_ERA_ETHERNET_CLIENT_HPP_
#define INC_ERA_ETHERNET_CLIENT_HPP_

#if !defined(ERA_PROTO_TYPE)
    #define ERA_PROTO_TYPE            "Ethernet"
#endif

#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqtt.hpp>

class ERaFlash;

template <class Transport>
class ERaEthernet
    : public ERaProto<Transport, ERaFlash>
{
    const char* TAG = "Ethernet";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;

public:
    ERaEthernet(Transport& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , authToken(nullptr)
    {}
    ~ERaEthernet()
    {}

    bool connectNetwork(const char* auth) {
        ERA_LOG(TAG, ERA_PSTR("Connecting network..."));
        this->getMacAddress(auth);
        if (!Ethernet.begin(this->macAddress)) {
            ERA_LOG(TAG, ERA_PSTR("Connect failed"));
            return false;
        }

        ERaDelay(1000);
        IPAddress localIP = Ethernet.localIP();
        ERA_FORCE_UNUSED(localIP);
        ERA_LOG(TAG, ERA_PSTR("IP: %d.%d.%d.%d"), localIP[0], localIP[1],
                                                localIP[2], localIP[3]);
        return true;
    }

    void config(const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::begin(auth);
        this->transp.config(host, port, username, password);
    }

    void begin(const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectNetwork(auth);
        Base::connect();
    }

    void begin() {
        this->begin(ERA_MQTT_CLIENT_ID,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

protected:
private:
    void getMacAddress(const char* auth) {
        this->macAddress[0] = 0x27;
        this->macAddress[1] = 0x06;
        this->macAddress[2] = 0x19;
        this->macAddress[3] = 0x95;
        this->macAddress[4] = 0xFE;
        this->macAddress[5] = 0xED;

        if (auth == nullptr) {
            return;
        }

        size_t index {0};
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
    }

    const char* authToken;
    uint8_t macAddress[6];
};

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    char ip[20] {0};
    IPAddress localIP = Ethernet.localIP();
    FormatString(ip, "%d.%d.%d.%d", localIP[0], localIP[1],
                                    localIP[2], localIP[3]);
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().ERA_AUTH);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, ERA_PROTO_TYPE);
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_PROTO_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddStringToObject(root, INFO_MAC, ERA_PROTO_TYPE);
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ip);
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().transp.getPing());
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
    cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ERA_PROTO_TYPE);
}

#endif /* INC_ERA_ETHERNET_CLIENT_HPP_ */
