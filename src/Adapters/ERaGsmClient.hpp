#ifndef INC_ERA_GSM_CLIENT_HPP_
#define INC_ERA_GSM_CLIENT_HPP_

#if !defined(ERA_PROTO_TYPE)
    #define ERA_PROTO_TYPE            "GSM"
#endif

#include <TinyGsmClient.h>
#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqtt.hpp>

#define GSM_NET_CONNECT_TIMEOUT      3 * 60000

typedef struct __ERaConfig_t {
    char apn[64];
    char user[64];
    char pass[64];
} ERA_ATTR_PACKED ERaConfig_t;

static ERaConfig_t eraConfig{0};

class ERaFlash;

template <class Transport>
class ERaGsm
    : public ERaProto<Transport, ERaFlash>
{
    const char* TAG = "GSM";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;

public:
    ERaGsm(Transport& _transp, ERaFlash& _flash)
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
        ERA_LOG(TAG, ERA_PSTR("Restarting modem..."));
        this->restart();

        ERA_LOG(TAG, ERA_PSTR("Modem init"));
        if (!this->modem->begin()) {
            ERA_LOG(TAG, ERA_PSTR("Can't init modem"));
            return false;
        }

        switch (this->modem->getSimStatus()) {
            case SimStatus::SIM_ERROR:
                ERA_LOG(TAG, ERA_PSTR("SIM is missing"));
                break;
            case SimStatus::SIM_LOCKED:
                ERA_LOG(TAG, ERA_PSTR("SIM is PIN-locked"));
                break;
            default:
                break;
        }

        ERA_LOG(TAG, ERA_PSTR("Connecting to network..."));
        if (this->modem->waitForNetwork()) {
            ERA_LOG(TAG, ERA_PSTR("Network: %s"), modem->getOperator().c_str());
        }
        else {
            ERA_LOG(TAG, ERA_PSTR("Register in network failed"));
        }

        ERA_LOG(TAG, ERA_PSTR("Connecting to %s..."), apn);
        if (!this->modem->gprsConnect(apn, user, pass)) {
            ERA_LOG(TAG, ERA_PSTR("Connect GPRS failed"));
            return false;
        }

        ERA_LOG(TAG, ERA_PSTR("Connected to GPRS"));
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
        this->setNetwork(apn, user, pass);
    }

    void begin(TinyGsm& gsm,
                const char* apn,
                const char* user,
                const char* pass,
                int pwrPin = -1) {
        this->begin(ERA_MQTT_CLIENT_ID, gsm, apn, user, pass,
                    pwrPin, ERA_MQTT_HOST, ERA_MQTT_PORT,
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
                    this->connectNetwork(eraConfig.apn,
                                        eraConfig.user, eraConfig.pass)) {
                    ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                }
                break;
        }
    }

protected:
private:
    void setNetwork(const char* apn, const char* user, const char* pass) {
        CopyToArray(apn, eraConfig.apn);
        CopyToArray(user, eraConfig.user);
        CopyToArray(pass, eraConfig.pass);
    }

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

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().ERA_AUTH);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, ERA_PROTO_TYPE);
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
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ERA_PROTO_TYPE);
}

#endif /* INC_ERA_GSM_CLIENT_HPP_ */
