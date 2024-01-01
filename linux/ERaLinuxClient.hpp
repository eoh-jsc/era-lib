#ifndef INC_ERA_LINUX_CLIENT_HPP_
#define INC_ERA_LINUX_CLIENT_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "Socket"
#endif

#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqttLinux.hpp>
#include <Storage/ERaFlashLinux.hpp>

template <class Transport>
class ERaLinux
    : public ERaProto<Transport, ERaFlashLinux>
{
    friend class ERaProto<Transport, ERaFlashLinux>;
    typedef ERaProto<Transport, ERaFlashLinux> Base;

public:
    ERaLinux(Transport& _transp, ERaFlashLinux& _flash)
        : Base(_transp, _flash)
    {}
    ~ERaLinux()
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
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(auth, host, port, username, password);
    }

    void begin(const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, host, port,
                    auth, auth);
    }

    void begin() {
        this->begin(ERA_AUTHENTICATION_TOKEN,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

    void run() {
        switch (ERaState::get()) {
            case StateT::STATE_CONNECTING_CLOUD:
                if (Base::connect([&, this]() {
                        this->networkInfo();
                    })) {
                    ERaOptConnected(this);
                    ERaState::set(StateT::STATE_CONNECTED);
                }
                else {
                    ERaState::set(StateT::STATE_CONNECTING_NETWORK);
                }
                break;
            case StateT::STATE_CONNECTED:
                ERaState::set(StateT::STATE_RUNNING);
                break;
            case StateT::STATE_RUNNING:
                Base::run();
                break;
            default:
                ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                break;
        }
    }

protected:
private:
    void networkInfo() {
        GetNetworkInfo();
        this->getTransp().setSSID(GetSSIDNetwork());
    }
};

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    int16_t signal = GetRSSINetwork();

    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
    cJSON_AddStringToObject(root, INFO_BOARD_ID, this->thisProto().getBoardID());
    cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().getAuth());
    cJSON_AddStringToObject(root, INFO_BUILD_DATE, BUILD_DATE_TIME);
    cJSON_AddStringToObject(root, INFO_VERSION, ERA_VERSION);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, 0);
    cJSON_AddStringToObject(root, INFO_NETWORK_PROTOCOL, GetNetworkProtocol());
    cJSON_AddStringToObject(root, INFO_SSID, GetSSIDNetwork());
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, signal);
    cJSON_AddNumberToObject(root, INFO_SIGNAL_STRENGTH, SignalToPercentage(signal));
    cJSON_AddStringToObject(root, INFO_MAC, GetMACAddress(NULL));
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, GetLocalIP(NULL));
    cJSON_AddNumberToObject(root, INFO_SSL, ERaInfoSSL());
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().getTransp().getPing());
    cJSON_AddNumberToObject(root, INFO_FREE_RAM, ERaFreeRam());
    cJSON_AddStringToObject(root, INFO_RESET_REASON, "UNKNOWN");

    /* Override info */
    ERaInfo(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
    int16_t signal = GetRSSINetwork();

    cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
    cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
    cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
    cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
    cJSON_AddNumberToObject(root, INFO_MB_RSSI, signal);
    cJSON_AddNumberToObject(root, INFO_MB_SIGNAL_STRENGTH, SignalToPercentage(signal));
    cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, GetSSIDNetwork());

    /* Override modbus info */
    ERaModbusInfo(root);
}

#endif /* INC_ERA_LINUX_CLIENT_HPP_ */
