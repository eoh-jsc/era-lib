#ifndef INC_ERA_WIFI_CLIENT_HPP_
#define INC_ERA_WIFI_CLIENT_HPP_

#include <TinyGsmClient.h>
#include <ERa/ERaApiArduinoDef.hpp>
#include <ERa/ERaProtocol.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <MQTT/ERaMqtt.hpp>

#define WIFI_NET_CONNECT_TIMEOUT      3 * 60000

class ERaFlash;

class ERaWiFi
    : public ERaProto< ERaMqtt<TinyGsmClient, MQTTClient>, ERaFlash >
{
    const char* TAG = "WiFi";
    friend class ERaProto< ERaMqtt<TinyGsmClient, MQTTClient>, ERaFlash >;
    typedef ERaProto< ERaMqtt<TinyGsmClient, MQTTClient>, ERaFlash > Base;

public:
    ERaWiFi(ERaMqtt<TinyGsmClient, MQTTClient>& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , modem(nullptr)
        , authToken(nullptr)
        , resetPin(-1)
    {}
    ~ERaWiFi()
    {}

    bool connectNetwork(const char* ssid, const char* pass) {
        unsigned long startMillis = ERaMillis();
        while (!this->connectWiFi(ssid, pass)) {
            ERaDelay(500);
            if (!ERaRemainingTime(startMillis, WIFI_NET_CONNECT_TIMEOUT)) {
                return false;
            }
        }
        return true;
    }

    bool connectWiFi(const char* ssid, const char* pass) {
        ERA_LOG(TAG, "Restarting modem...");
        this->restart();

        ERA_LOG(TAG, "Modem init");
        if (!this->modem->begin()) {
            ERA_LOG(TAG, "Can't init modem");
            return false;
        }

        ERA_LOG(TAG, "Connecting to %s...", ssid);
        if (!this->modem->networkConnect(ssid, pass)) {
            ERA_LOG(TAG, "Connect %s failed", ssid);
            return false;
        }

        ERA_LOG(TAG, "Waiting for network...");
        if (!this->modem->waitForNetwork()) {
            ERA_LOG(TAG, "Connect %s failed", ssid);
            return false;
        }

        ERA_LOG(TAG, "Connected to %s", ssid);
        return true;
    }

    void config(TinyGsm& gsm,
                const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::begin(auth);
        this->modem = &gsm;
        this->client.init(modem);
        this->transp.setClient(&this->client);
        this->transp.config(host, port, username, password);
    }

    void begin(const char* auth,
                TinyGsm& gsm,
                const char* ssid,
                const char* pass,
                int rstPin = -1,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::init();
        this->config(gsm, auth, host, port, username, password);
        this->setPower(rstPin);
        this->connectNetwork(ssid, pass);
        Base::connect();
    }

    void begin(TinyGsm& gsm,
                const char* ssid,
                const char* pass,
                int rstPin = -1) {
        Base::init();
        this->config(gsm, ERA_MQTT_CLIENT_ID,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
        this->setPower(rstPin);
        this->connectNetwork(ssid, pass);
        Base::connect();
    }

protected:
private:
    void setPower(int pin) {
        this->resetPin = pin;
    }

    void restart() {
        if (this->resetPin <= 0) {
            return;
        }
        pinMode(this->resetPin, OUTPUT);
        this->powerOff();
        ERaDelay(2000);
        this->powerOn();
        ERaDelay(500);
    }

    void powerOn() {
        if (this->resetPin <= 0) {
            return;
        }
        ::digitalWrite(this->resetPin, HIGH);
    }

    void powerOff() {
        if (this->resetPin <= 0) {
            return;
        }
        ::digitalWrite(this->resetPin, LOW);
    }

    TinyGsm* modem;
    TinyGsmClient client;
    const char* authToken;
    int resetPin;
};

#endif /* INC_ERA_WIFI_CLIENT_HPP_ */
