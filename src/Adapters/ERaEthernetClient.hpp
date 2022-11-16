#ifndef INC_ERA_ETHERNET_CLIENT_HPP_
#define INC_ERA_ETHERNET_CLIENT_HPP_

#include <ERa/ERaApiArduinoDef.hpp>
#include <ERa/ERaProtocol.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <MQTT/ERaMqtt.hpp>

class ERaFlash;

class ERaEthernet
    : public ERaProto< ERaMqtt<EthernetClient, MQTTClient>, ERaFlash >
{
    const char* TAG = "Ethernet";
    friend class ERaProto< ERaMqtt<EthernetClient, MQTTClient>, ERaFlash >;
    typedef ERaProto< ERaMqtt<EthernetClient, MQTTClient>, ERaFlash > Base;

public:
    ERaEthernet(ERaMqtt<EthernetClient, MQTTClient>& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , authToken(nullptr)
    {}
    ~ERaEthernet()
    {}

    bool connectNetwork(const char* auth) {
        ERA_LOG(TAG, "Connecting network...");
        this->getMacAddress(auth);
        if (!Ethernet.begin(this->macAddress)) {
            ERA_LOG(TAG, "Connect failed");
            return false;
        }

        ERaDelay(1000);
        IPAddress localIP = Ethernet.localIP();
        ERA_FORCE_UNUSED(localIP);
        ERA_LOG(TAG, "IP: %s", localIP.toString().c_str());
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

        ERA_LOG(TAG, "Get MAC: %02X-%02X-%02X-%02X-%02X-%02X",
                this->macAddress[0], this->macAddress[1],
                this->macAddress[2], this->macAddress[3],
                this->macAddress[4], this->macAddress[5]);
    }

    const char* authToken;
    uint8_t macAddress[6];
};

#endif /* INC_ERA_ETHERNET_CLIENT_HPP_ */
