#ifndef INC_ERA_ARDUINO_GSM_CLIENT_HPP_
#define INC_ERA_ARDUINO_GSM_CLIENT_HPP_

#if !defined(ERA_PROTO_TYPE)
    #define ERA_PROTO_TYPE            "GSM"
#endif

#include <MKRGSM.h>
#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqtt.hpp>

#define GSM_NET_CONNECT_TIMEOUT      3 * 60000

typedef struct __ERaConfig_t {
    char apn[64];
    char user[64];
    char pass[64];
    char pin[64];
} ERA_ATTR_PACKED ERaConfig_t;

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaConfig_t ERaConfig {};
#else
    #define ERaConfig  StaticHelper<ERaConfig_t>::instance()
#endif

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
        , authToken(nullptr)
        , _connected(false)
    {}
    ~ERaGsm()
    {}

    bool connectNetwork(const char* apn, const char* user,
                        const char* pass, const char* pin) {
        MillisTime_t startMillis = ERaMillis();
        while (!this->connectGPRS(apn, user, pass, pin)) {
            ERaDelay(500);
            if (!ERaRemainingTime(startMillis, GSM_NET_CONNECT_TIMEOUT)) {
                return false;
            }
        }
        this->getTransp().setSSID(strlen(apn) ? apn : ERA_PROTO_TYPE);
        return true;
    }

    bool connectGPRS(const char* apn, const char* user,
                    const char* pass, const char* pin) {
        ERA_LOG(TAG, ERA_PSTR("Modem init"));
        if (this->modem.begin(pin) != GSM_READY) {
            ERA_LOG(TAG, ERA_PSTR("Can't init modem"));
            return false;
        }

        ERA_LOG(TAG, ERA_PSTR("Connecting to network..."));
        if (gprs.attachGPRS(apn, user, pass) != GPRS_READY) {
            ERA_LOG(TAG, ERA_PSTR("Connect GPRS failed"));
            return false;
        }

        ERA_LOG(TAG, ERA_PSTR("Connected to GPRS"));
        this->_connected = true;
        return true;
    }

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
                const char* apn,
                const char* user,
                const char* pass,
                const char* pin,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectNetwork(apn, user, pass, pin);
        this->setNetwork(apn, user, pass, pin);
    }

    void begin(const char* apn,
                const char* user,
                const char* pass,
                const char* pin) {
        this->begin(ERA_AUTHENTICATION_TOKEN, apn, user, pass,
                    pin, ERA_MQTT_HOST, ERA_MQTT_PORT,
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
                    this->connectNetwork(ERaConfig.apn, ERaConfig.user,
                                        ERaConfig.pass, ERaConfig.pin)) {
                    ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                }
                break;
        }
    }

protected:
private:
    void setNetwork(const char* apn, const char* user,
                    const char* pass, const char* pin) {
        CopyToArray(apn, ERaConfig.apn);
        CopyToArray(user, ERaConfig.user);
        CopyToArray(pass, ERaConfig.pass);
        CopyToArray(pin, ERaConfig.pin);
    }

    bool connected() {
        return (this->modem.isAccessAlive() && this->_connected);
    }

    GSM modem;
    GPRS gprs;
    GSMClient client;
    const char* authToken;
    bool _connected;
};

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_BOARD_ID, this->thisProto().getBoardID());
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().getAuth());
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, ((this->thisProto().getTransp().getSSID() == nullptr) ?
                                            ERA_PROTO_TYPE : this->thisProto().getTransp().getSSID()));
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_PROTO_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddStringToObject(root, INFO_MAC, ERA_PROTO_TYPE);
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ERA_PROTO_TYPE);
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().getTransp().getPing());
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ((this->thisProto().getTransp().getSSID() == nullptr) ?
                                                    ERA_PROTO_TYPE : this->thisProto().getTransp().getSSID()));
}

#endif /* INC_ERA_ARDUINO_GSM_CLIENT_HPP_ */
