#ifndef INC_ERA_PARTICLE_CLIENT_HPP_
#define INC_ERA_PARTICLE_CLIENT_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "Particle"
#endif

#if !defined(ERA_AUTH_TOKEN)
    #error "Please specify your ERA_AUTH_TOKEN"
#endif

#include "application.h"

#undef retained

#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqtt.hpp>

class ERaFlash;

template <class Transport>
class ERaParticle
    : public ERaProto<Transport, ERaFlash>
{
    const char* TAG = "Particle";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;

public:
    ERaParticle(Transport& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , authToken(nullptr)
    {}
    ~ERaParticle()
    {}

    void config(const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::begin(auth);
        this->getTransp().setClient(&this->client);
        this->getTransp().config(host, port, username, password);
    }

    void begin(const char* auth,
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->getTransp().setSSID(ERA_NETWORK_TYPE);
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
                if (Base::connect()) {
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
            case StateT::STATE_CONNECTING_NEW_NETWORK:
                Base::connectNewNetworkResult();
                break;
            case StateT::STATE_REQUEST_LIST_WIFI:
                Base::responseListWiFi();
                break;
            default:
                ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                break;
        }
    }

protected:
private:
    TCPClient client;
    const char* authToken;
};

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, 0);
    cJSON_AddStringToObject(root, INFO_NETWORK_PROTOCOL, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_SSID, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddNumberToObject(root, INFO_SIGNAL_STRENGTH, 100);
    cJSON_AddStringToObject(root, INFO_MAC, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_SSL, ERaInfoSSL());

    /* Override info */
    ERaInfo(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addSelfInfo(cJSON* root) {
    cJSON_AddNumberToObject(root, SELF_CHIP_TEMPERATURE, 5000);
    cJSON_AddNumberToObject(root, SELF_SIGNAL_STRENGTH, 100);

    /* Override self info */
    ERaSelfInfo(root);
}

#if defined(ERA_MODBUS)
    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
        cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
        cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
        cJSON_AddNumberToObject(root, INFO_MB_SIGNAL_STRENGTH, 100);
        cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ERA_NETWORK_TYPE);

        /* Override modbus info */
        ERaModbusInfo(root);
    }
#endif

#endif /* INC_ERA_PARTICLE_CLIENT_HPP_ */
