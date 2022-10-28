#ifndef INC_ERA_LINUX_CLIENT_HPP_
#define INC_ERA_LINUX_CLIENT_HPP_

#include <ERaApiWiringPiDef.hpp>
#include <ERa/ERaProtocol.hpp>
#include <ERaApiWiringPi.hpp>
#include <MQTTLinux/ERaMqttLinux.hpp>
#include <UtilityLinux/ERaFlashLinux.hpp>

class ERaLinux
    : public ERaProto< ERaMqttLinux<MQTTLinuxClient>, ERaFlashLinux >
{
    friend class ERaProto< ERaMqttLinux<MQTTLinuxClient>, ERaFlashLinux >;
    typedef ERaProto< ERaMqttLinux<MQTTLinuxClient>, ERaFlashLinux > Base;

public:
    ERaLinux(ERaMqttLinux<MQTTLinuxClient>& _transp, ERaFlashLinux& _flash)
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
        this->transp.config(host, port, username, password);
    }

    void begin(const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::init();
        this->config(auth, host, port, username, password);
        Base::connect();
    }

    void begin() {
        this->begin(ERA_MQTT_CLIENT_ID,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

protected:
private:
};

#endif /* INC_ERA_LINUX_CLIENT_HPP_ */
