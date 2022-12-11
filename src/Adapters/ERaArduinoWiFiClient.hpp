#ifndef INC_ERA_ARDUINO_WIFI_CLIENT_HPP_
#define INC_ERA_ARDUINO_WIFI_CLIENT_HPP_

#if !defined(ERA_PROTO_TYPE)
    #define ERA_PROTO_TYPE            "WiFi"
#endif

#include <ERa/ERaApiArduinoDef.hpp>
#include <ERa/ERaProtocol.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <MQTT/ERaMqtt.hpp>

#define WIFI_NET_CONNECT_TIMEOUT      20000

typedef struct __ERaConfig_t {
    char ssid[64];
    char pass[64];
} ERA_ATTR_PACKED ERaConfig_t;

static ERaConfig_t eraConfig{0};

class ERaFlash;

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
                status = WiFi.begin(ssid, pass);
            } else {
                status = WiFi.begin(ssid);
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
        this->transp.setSSID(ssid);
        ERA_LOG(TAG, ERA_PSTR("Connected to WiFi"));

        IPAddress localIP = WiFi.localIP();
        ERA_FORCE_UNUSED(localIP);
        ERA_LOG(TAG, ERA_PSTR("IP: %s"), localIP.toString().c_str());
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
        this->setWiFi(ssid, pass);
    }

    void begin(const char* ssid,
                const char* pass) {
        this->begin(ERA_MQTT_CLIENT_ID, ssid, pass,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

    void run() {
        switch (ERaState::get()) {
            case StateT::STATE_CONNECTING_CLOUD:
                if (Base::connect()) {
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
                    this->connectWiFi(eraConfig.ssid, eraConfig.pass)) {
                    ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                }
                break;
        }
    }

protected:
private:
    void setWiFi(const char* ssid, const char* pass) {
        CopyToArray(ssid, eraConfig.ssid);
        CopyToArray(pass, eraConfig.pass);
    }

    bool connected() {
        return (WiFi.status() == WL_CONNECTED);
    }

    const char* authToken;

};

#endif /* INC_ERA_ARDUINO_WIFI_CLIENT_HPP_ */
