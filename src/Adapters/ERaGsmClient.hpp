#ifndef INC_ERA_GSM_CLIENT_HPP_
#define INC_ERA_GSM_CLIENT_HPP_

#include <TinyGsmClient.h>
#include <ERa/ERaApiStm32Def.hpp>
#include <ERa/ERaProtocol.hpp>
#include <ERa/ERaApiStm32.hpp>
#include <MQTT/ERaMqtt.hpp>

#define GSM_NET_CONNECT_TIMEOUT      3 * 60000

class ERaFlash;

class ERaGsm
    : public ERaProto< ERaMqtt<TinyGsmClient, MQTTClient>, ERaFlash >
{
    const char* TAG = "GSM";
    friend class ERaProto< ERaMqtt<TinyGsmClient, MQTTClient>, ERaFlash >;
    typedef ERaProto< ERaMqtt<TinyGsmClient, MQTTClient>, ERaFlash > Base;

public:
    ERaGsm(ERaMqtt<TinyGsmClient, MQTTClient>& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , modem(nullptr)
        , authToken(nullptr)
        , powerPin(-1)
    {}
    ~ERaGsm()
    {}

    bool connectNetwork(const char* apn, const char* user, const char* pass) {
        MillisTime_t startMillis = ERaMillis();
        while (!this->connectGPRS(apn, user, pass)) {
            ERaDelay(500);
            if (!ERaRemainingTime(startMillis, GSM_NET_CONNECT_TIMEOUT)) {
                return false;
            }
        }
        return true;
    }

    bool connectGPRS(const char* apn, const char* user, const char* pass) {
        ERA_LOG(TAG, "Restarting modem...");
        this->restart();

        ERA_LOG(TAG, "Modem init");
        if (!this->modem->begin()) {
            ERA_LOG(TAG, "Can't init modem");
            return false;
        }

        switch (this->modem->getSimStatus()) {
            case SimStatus::SIM_ERROR:
                ERA_LOG(TAG, "SIM is missing");
                break;
            case SimStatus::SIM_LOCKED:
                ERA_LOG(TAG, "SIM is PIN-locked");
                break;
            default:
                break;
        }

        ERA_LOG(TAG, "Connecting to network...");
        if (this->modem->waitForNetwork()) {
            ERA_LOG(TAG, "Network: %s", modem->getOperator().c_str());
        }
        else {
            ERA_LOG(TAG, "Register in network failed");
        }

        ERA_LOG(TAG, "Connecting to %s...", apn);
        if (!this->modem->gprsConnect(apn, user, pass)) {
            ERA_LOG(TAG, "Connect GPRS failed");
            return false;
        }

        ERA_LOG(TAG, "Connected to GPRS");
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
                const char* apn,
                const char* user,
                const char* pass,
                int pwrPin = -1,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::init();
        this->config(gsm, auth, host, port, username, password);
        this->setPower(pwrPin);
        this->connectNetwork(apn, user, pass);
        Base::connect();
    }

    void begin(TinyGsm& gsm,
                const char* apn,
                const char* user,
                const char* pass,
                int pwrPin = -1) {
        Base::init();
        this->config(gsm, ERA_MQTT_CLIENT_ID,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
        this->setPower(pwrPin);
        this->connectNetwork(apn, user, pass);
        Base::connect();
    }

protected:
private:
    void setPower(int pin) {
        this->powerPin = pin;
    }

    void restart() {
        if (this->powerPin <= 0) {
            return;
        }
        pinMode(this->powerPin, OUTPUT);
        this->powerOff();
        ERaDelay(2000);
        this->powerOn();
        ERaDelay(500);
    }

    void powerOn() {
        if (this->powerPin <= 0) {
            return;
        }
        ::digitalWrite(this->powerPin, LOW);
    }

    void powerOff() {
        if (this->powerPin <= 0) {
            return;
        }
        ::digitalWrite(this->powerPin, HIGH);
    }

    TinyGsm* modem;
    TinyGsmClient client;
    const char* authToken;
    int powerPin;
};

#endif /* INC_ERA_GSM_CLIENT_HPP_ */
