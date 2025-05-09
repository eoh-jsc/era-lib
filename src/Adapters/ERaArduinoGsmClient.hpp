#ifndef INC_ERA_ARDUINO_GSM_CLIENT_HPP_
#define INC_ERA_ARDUINO_GSM_CLIENT_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "GSM"
#endif

#if !defined(ERA_AUTH_TOKEN)
    #error "Please specify your ERA_AUTH_TOKEN"
#endif

#include <MKRGSM.h>
#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqtt.hpp>

#define GSM_NET_CONNECT_TIMEOUT       3 * 60000

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
            Base::appLoop();
            ERaWatchdogFeed();
            if (!ERaRemainingTime(startMillis, GSM_NET_CONNECT_TIMEOUT)) {
                return false;
            }
        }
        this->getTransp().setSSID(apn);
        return true;
    }

    bool connectGPRS(const char* apn, const char* user,
                    const char* pass, const char* pin) {
        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Modem init"));
        if (this->modem.begin(pin) != GSM_READY) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Can't init modem"));
            return false;
        }

        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Connecting to network..."));
        if (gprs.attachGPRS(apn, user, pass) != GPRS_READY) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Connect GPRS failed"));
            return false;
        }

        ERaWatchdogFeed();

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
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectNetwork(apn, user, pass, pin);
        this->setNetwork(apn, user, pass, pin);
    }

    void begin(const char* auth,
                const char* apn,
                const char* user,
                const char* pass,
                const char* pin,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, apn, user,
                    pass, pin, host,
                    port, auth, auth);
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
                    ERaOptConnected(this);
                    ERaState::set(StateT::STATE_CONNECTED);
                }
                else {
                    ERaState::set(StateT::STATE_CONNECTING_NETWORK);
                }
                break;
            case StateT::STATE_CONNECTED:
                ERaState::set(StateT::STATE_RUNNING);
                break;
            case StateT::STATE_RUNNING:
                Base::run();
                break;
            case StateT::STATE_CONNECTING_NEW_NETWORK:
                Base::connectNewNetworkResult();
                break;
            case StateT::STATE_REQUEST_LIST_WIFI:
                Base::responseListWiFi();
                break;
            default:
                if (this->netConnected() ||
                    this->connectNetwork(ERaConfig.apn, ERaConfig.user,
                                        ERaConfig.pass, ERaConfig.pin)) {
                    ERaWatchdogFeed();

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

    bool netConnected() {
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
    const char* ssid = this->thisProto().getTransp().getSSID();

    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, 0);
    cJSON_AddStringToObject(root, INFO_NETWORK_PROTOCOL, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_SSID, ((ssid != nullptr) ? ssid : ERA_NETWORK_TYPE));
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddNumberToObject(root, INFO_SIGNAL_STRENGTH, 100);
    cJSON_AddStringToObject(root, INFO_MAC, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_SSL, ERaInfoSSL());

    /* Override info */
    ERaInfo(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addSelfInfo(cJSON* root) {
    cJSON_AddNumberToObject(root, SELF_CHIP_TEMPERATURE, 5000);
    cJSON_AddNumberToObject(root, SELF_SIGNAL_STRENGTH, 100);

    /* Override self info */
    ERaSelfInfo(root);
}

#if defined(ERA_MODBUS)
    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
        const char* ssid = this->thisProto().getTransp().getSSID();

        cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
        cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
        cJSON_AddNumberToObject(root, INFO_MB_SIGNAL_STRENGTH, 100);
        cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ((ssid != nullptr) ? ssid : ERA_NETWORK_TYPE));

        /* Override modbus info */
        ERaModbusInfo(root);
    }
#endif

#endif /* INC_ERA_ARDUINO_GSM_CLIENT_HPP_ */
