#ifndef INC_ERA_ARDUINO_WIFI_CLIENT_HPP_
#define INC_ERA_ARDUINO_WIFI_CLIENT_HPP_

#include <ERa/ERaApiArduinoDef.hpp>
#include <ERa/ERaProtocol.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <MQTT/ERaMqtt.hpp>

#define WIFI_NET_CONNECT_TIMEOUT      20000

class ERaFlash;

class ERaWiFi
    : public ERaProto< ERaMqtt<WiFiClient, MQTTClient>, ERaFlash >
{
    const char* TAG = "WiFi";
    friend class ERaProto< ERaMqtt<WiFiClient, MQTTClient>, ERaFlash >;
    typedef ERaProto< ERaMqtt<WiFiClient, MQTTClient>, ERaFlash > Base;

public:
    ERaWiFi(ERaMqtt<WiFiClient, MQTTClient>& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , authToken(nullptr)
    {}
    ~ERaWiFi()
    {}

    bool connectWiFi(const char* ssid, const char* pass) {
        int status = WiFi.status();

        if (status == WL_NO_SHIELD) {
            ERA_LOG(TAG, "WiFi module not found");
        }

        while (status != WL_CONNECTED) {
            ERA_LOG(TAG, "Connecting to %s...", ssid);
            if (pass && strlen(pass)) {
                status = WiFi.begin(ssid, pass);
            } else {
                status = WiFi.begin(ssid);
            }

            unsigned long startMillis = ERaMillis();
            while (status != WL_CONNECTED) {
                ERaDelay(500);
                status = WiFi.status();
                if (!ERaRemainingTime(startMillis, WIFI_NET_CONNECT_TIMEOUT)) {
                    WiFi.disconnect();
                    break;
                }
            }
        }
        ERA_LOG(TAG, "Connected to WiFi");

        IPAddress localIP = WiFi.localIP();
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
                const char* ssid,
                const char* pass,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectWiFi(ssid, pass);
        Base::connect();
    }

    void begin(const char* ssid,
                const char* pass) {
        Base::init();
        this->config(ERA_MQTT_CLIENT_ID,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
        this->connectWiFi(ssid, pass);
        Base::connect();
    }

protected:
private:
    const char* authToken;

};

#endif /* INC_ERA_ARDUINO_WIFI_CLIENT_HPP_ */
