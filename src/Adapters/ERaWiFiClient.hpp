#ifndef INC_ERA_WIFI_CLIENT_HPP_
#define INC_ERA_WIFI_CLIENT_HPP_

#if !defined(ERA_PROTO_TYPE)
    #define ERA_PROTO_TYPE            "WiFi"
#endif

#include <TinyGsmClient.h>
#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqtt.hpp>

#define WIFI_NET_CONNECT_TIMEOUT      3 * 60000

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
        , modem(nullptr)
        , authToken(nullptr)
        , resetPin(-1)
    {}
    ~ERaWiFi()
    {}

    bool connectNetwork(const char* ssid, const char* pass) {
        MillisTime_t startMillis = ERaMillis();
        while (!this->connectWiFi(ssid, pass)) {
            ERaDelay(500);
            if (!ERaRemainingTime(startMillis, WIFI_NET_CONNECT_TIMEOUT)) {
                return false;
            }
        }
        this->transp.setSSID(ssid);
        return true;
    }

    bool connectWiFi(const char* ssid, const char* pass) {
        ERA_LOG(TAG, ERA_PSTR("Restarting modem..."));
        this->restart();

        ERA_LOG(TAG, ERA_PSTR("Modem init"));
        if (!this->modem->begin()) {
            ERA_LOG(TAG, ERA_PSTR("Can't init modem"));
            return false;
        }

        ERA_LOG(TAG, ERA_PSTR("Connecting to %s..."), ssid);
        if (!this->modem->networkConnect(ssid, pass)) {
            ERA_LOG(TAG, ERA_PSTR("Connect %s failed"), ssid);
            return false;
        }

        ERA_LOG(TAG, ERA_PSTR("Waiting for network..."));
        if (!this->modem->waitForNetwork()) {
            ERA_LOG(TAG, ERA_PSTR("Connect %s failed"), ssid);
            return false;
        }

        ERA_LOG(TAG, ERA_PSTR("Connected to %s"), ssid);
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
        this->setNetwork(ssid, pass);
    }

    void begin(TinyGsm& gsm,
                const char* ssid,
                const char* pass,
                int rstPin = -1) {
        this->begin(ERA_MQTT_CLIENT_ID, gsm, ssid, pass,
                    rstPin, ERA_MQTT_HOST, ERA_MQTT_PORT,
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
                if (this->modem->isNetworkConnected() ||
                    this->connectNetwork(eraConfig.ssid, eraConfig.pass)) {
                    ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                }
                break;
        }
    }

protected:
private:
    void setNetwork(const char* ssid, const char* pass) {
        CopyToArray(ssid, eraConfig.ssid);
        CopyToArray(pass, eraConfig.pass);
    }

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

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().ERA_AUTH);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, ((this->thisProto().transp.getSSID() == nullptr) ?
                                            ERA_PROTO_TYPE : this->thisProto().transp.getSSID()));
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_PROTO_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddStringToObject(root, INFO_MAC, ERA_PROTO_TYPE);
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ERA_PROTO_TYPE);
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().transp.getPing());
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ((this->thisProto().transp.getSSID() == nullptr) ?
                                                    ERA_PROTO_TYPE : this->thisProto().transp.getSSID()));
}

#endif /* INC_ERA_WIFI_CLIENT_HPP_ */
