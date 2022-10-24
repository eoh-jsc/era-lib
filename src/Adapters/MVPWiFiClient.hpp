#ifndef INC_MVP_WIFI_CLIENT_HPP_
#define INC_MVP_WIFI_CLIENT_HPP_

#include <TinyGsmClient.h>
#include <MVP/MVPApiArduinoDef.hpp>
#include <MVP/MVPProtocol.hpp>
#include <MVP/MVPApiArduino.hpp>
#include <MQTT/MVPMqtt.hpp>
#include <Modbus/MVPModbusStm32.hpp>
#include <Utility/MVPFlashStm32.hpp>

#define WIFI_NET_CONNECT_TIMEOUT      60000

class MVPWiFi
    : public MVPProto< MVPMqtt<TinyGsmClient, MQTTClient>, MVPFlash >
{
    const char* TAG = "WiFi";
    friend class MVPProto< MVPMqtt<TinyGsmClient, MQTTClient>, MVPFlash >;
    typedef MVPProto< MVPMqtt<TinyGsmClient, MQTTClient>, MVPFlash > Base;

public:
    MVPWiFi(MVPMqtt<TinyGsmClient, MQTTClient>& _transp, MVPFlash& _flash)
        : Base(_transp, _flash)
        , modem(nullptr)
        , authToken(nullptr)
        , powerPin(-1)
    {}
    ~MVPWiFi()
    {}

    bool connectNetwork(const char* ssid, const char* pass) {
        unsigned long prevMillis = MVPMillis();
        while (!this->connectWiFi(ssid, pass)) {
            MVPDelay(500);
            if (!MVPRemainingTime(prevMillis, WIFI_NET_CONNECT_TIMEOUT)) {
                return false;
            }
        }
        return true;
    }

    bool connectWiFi(const char* ssid, const char* pass) {
        MVP_LOG(TAG, "Restarting modem...");
        this->restart();

        MVP_LOG(TAG, "Modem init");
        if (!this->modem->begin()) {
            MVP_LOG(TAG, "Can't init modem");
            return false;
        }

        MVP_LOG(TAG, "Connecting to %s...", ssid);
        if (!this->modem->networkConnect(ssid, pass)) {
            MVP_LOG(TAG, "Connect %s failed", ssid);
            return false;
        }

        MVP_LOG(TAG, "Connected to %s", ssid);
        return true;
    }

    void config(TinyGsm& gsm,
                const char* auth,
                const char* host = MVP_MQTT_HOST,
                uint16_t port = MVP_MQTT_PORT,
                const char* username = MVP_MQTT_USERNAME,
                const char* password = MVP_MQTT_PASSWORD) {
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
                int pwrPin = -1,
                const char* host = MVP_MQTT_HOST,
                uint16_t port = MVP_MQTT_PORT,
                const char* username = MVP_MQTT_USERNAME,
                const char* password = MVP_MQTT_PASSWORD) {
        Base::init();
        this->config(gsm, auth, host, port, username, password);
        this->setPower(pwrPin);
        this->connectNetwork(ssid, pass);
        Base::connect();
    }

    void begin(TinyGsm& gsm,
                const char* ssid,
                const char* pass,
                int pwrPin = -1) {
        Base::init();
        this->config(gsm, MQTT_CLIENT_ID,
                    MVP_MQTT_HOST, MVP_MQTT_PORT,
                    MVP_MQTT_USERNAME, MVP_MQTT_PASSWORD);
        this->setPower(pwrPin);
        this->connectNetwork(ssid, pass);
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
        MVPDelay(2000);
        this->powerOn();
        MVPDelay(500);
    }

    void powerOn() {
        if (this->powerPin <= 0) {
            return;
        }
        ::digitalWrite(this->powerPin, HIGH);
    }

    void powerOff() {
        if (this->powerPin <= 0) {
            return;
        }
        ::digitalWrite(this->powerPin, LOW);
    }

    TinyGsm* modem;
    TinyGsmClient client;
    const char* authToken;
    int powerPin;
};

#endif /* INC_MVP_WIFI_CLIENT_HPP_ */