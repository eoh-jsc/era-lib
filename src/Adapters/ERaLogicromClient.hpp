#ifndef INC_ERA_LOGICROM_CLIENT_HPP_
#define INC_ERA_LOGICROM_CLIENT_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "GSM"
#endif

#if !defined(ERA_AUTH_TOKEN)
    #error "Please specify your ERA_AUTH_TOKEN"
#endif

#include <Net.h>
#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqtt.hpp>

#define GSM_NET_CONNECT_TIMEOUT       3 * 60000

typedef struct __ERaConfig_t {
    char apn[64];
    char user[64];
    char pass[64];
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
    {}
    ~ERaGsm()
    {}

    bool connectNetwork(const char* apn, const char* user, const char* pass) {
        MillisTime_t startMillis = ERaMillis();
        while (!this->connectGPRS(apn, user, pass)) {
            ERaDelay(500);
            Base::appLoop();
            ERaWatchdogFeed();
            if (!ERaRemainingTime(startMillis, GSM_NET_CONNECT_TIMEOUT)) {
                return false;
            }
        }
        this->getTransp().setSSID(strlen(apn) ? apn : ERA_NETWORK_TYPE);
        return true;
    }

    bool connectGPRS(const char* apn, const char* user, const char* pass) {
        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Modem init"));
        if (!Net.begin()) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Can't init modem"));
            return false;
        }

        ERaWatchdogFeed();

        int status = Net.getSimStatus();
        switch (status) {
            case simstate_e::SIM_STAT_READY:
                break;
            case simstate_e::SIM_STAT_NOT_INSERTED:
                ERA_LOG_ERROR(TAG, ERA_PSTR("SIM is missing"));
                break;
            default:
                ERA_LOG_ERROR(TAG, ERA_PSTR("SIM error %d"), status);
                break;
        }

        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Connecting to network..."));
        if (Net.waitForRegistration()) {
            ERA_LOG(TAG, ERA_PSTR("Network: %s"), Net.getOperator());
        }
        else {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Register in network failed"));
            return false;
        }

        ERaWatchdogFeed();

        Net.setAPN(apn, user, pass);
        ERA_LOG(TAG, ERA_PSTR("Connecting to %s..."), apn);
        if (!Net.waitDataReady()) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Connect GPRS failed"));
            return false;
        }

        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Connected to GPRS"));
        return true;
    }

    void config(const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::begin(auth);
        this->getTransp().config(host, port, username, password);
    }

    void begin(const char* auth,
                const char* apn,
                const char* user,
                const char* pass,
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectNetwork(apn, user, pass);
        this->setNetwork(apn, user, pass);
    }

    void begin(const char* auth,
                const char* apn,
                const char* user,
                const char* pass,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, apn, user,
                    pass, host, port,
                    auth, auth);
    }

    void begin(const char* apn,
                const char* user,
                const char* pass) {
        this->begin(ERA_AUTHENTICATION_TOKEN, apn, user, pass,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
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
            default:
                if (Net.isDataReady() ||
                    this->connectNetwork(ERaConfig.apn,
                                        ERaConfig.user, ERaConfig.pass)) {
                    ERaWatchdogFeed();

                    this->getTransp().setSignalQuality(Net.isDataReady());

                    ERaWatchdogFeed();

                    ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                }
                break;
        }
    }

protected:
private:
    void setNetwork(const char* apn, const char* user, const char* pass) {
        CopyToArray(apn, ERaConfig.apn);
        CopyToArray(user, ERaConfig.user);
        CopyToArray(pass, ERaConfig.pass);
    }

    const char* authToken;
};

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    int16_t signal = this->thisProto().getTransp().getSignalQuality();

    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
    cJSON_AddStringToObject(root, INFO_BOARD_ID, this->thisProto().getBoardID());
    cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().getAuth());
    cJSON_AddStringToObject(root, INFO_BUILD_DATE, BUILD_DATE_TIME);
    cJSON_AddStringToObject(root, INFO_VERSION, ERA_VERSION);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, 0);
    cJSON_AddStringToObject(root, INFO_NETWORK_PROTOCOL, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_SSID, ((this->thisProto().getTransp().getSSID() == nullptr) ?
                                            ERA_NETWORK_TYPE : this->thisProto().getTransp().getSSID()));
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, signal);
    cJSON_AddNumberToObject(root, INFO_SIGNAL_STRENGTH, SignalToPercentage(signal));
    cJSON_AddStringToObject(root, INFO_MAC, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_SSL, ERaInfoSSL());
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().getTransp().getPing());
    cJSON_AddNumberToObject(root, INFO_FREE_RAM, ERaFreeRam());

#if defined(ERA_RESET_REASON)
    cJSON_AddStringToObject(root, INFO_RESET_REASON, SystemGetResetReason().c_str());
#endif

    /* Override info */
    ERaInfo(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
    int16_t signal = this->thisProto().getTransp().getSignalQuality();

    cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
    cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
    cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
    cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
    cJSON_AddNumberToObject(root, INFO_MB_RSSI, signal);
    cJSON_AddNumberToObject(root, INFO_MB_SIGNAL_STRENGTH, SignalToPercentage(signal));
    cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ((this->thisProto().getTransp().getSSID() == nullptr) ?
                                                    ERA_NETWORK_TYPE : this->thisProto().getTransp().getSSID()));

    /* Override modbus info */
    ERaModbusInfo(root);
}

#endif /* INC_ERA_LOGICROM_CLIENT_HPP_ */
