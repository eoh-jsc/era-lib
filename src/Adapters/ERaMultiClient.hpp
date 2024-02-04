#ifndef INC_ERA_MULTI_CLIENT_HPP_
#define INC_ERA_MULTI_CLIENT_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "Multi"
#endif

#if !defined(ERA_AUTH_TOKEN)
    #error "Please specify your ERA_AUTH_TOKEN"
#endif

#if !defined(ERA_MAX_CLIENT)
    #define ERA_MAX_CLIENT            3
#endif

#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqtt.hpp>

class ERaFlash;

template <class Transport>
class ERaMulti
    : public ERaProto<Transport, ERaFlash>
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<int16_t(void)> ClientConnectCallback_t;
#else
    typedef int16_t (*ClientConnectCallback_t)(void);
#endif

    const char* TAG = "Multi";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;

    typedef struct __EntryClient_t {
        String name;
        Client* client;
        ClientConnectCallback_t cb;
    } EntryClient_t;

public:
    ERaMulti(Transport& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , numClient(0)
        , authToken(nullptr)
    {}
    ~ERaMulti()
    {}

    bool addClient(const String& name, Client* client,
                    ClientConnectCallback_t cb = nullptr) {
        if (this->numClient >= ERA_MAX_CLIENT) {
            return false;
        }
        EntryClient_t& e = this->eClient[this->numClient++];
        e.name = name;
        e.client = client;
        e.cb = cb;
        return true;
    }

    bool addClient(const String& name, Client& client,
                    ClientConnectCallback_t cb = nullptr) {
        return this->addClient(name, &client, cb);
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
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(auth, host, port, username, password);
    }

    void begin(const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, host, port,
                    auth, auth);
    }

    void begin() {
        this->begin(ERA_AUTHENTICATION_TOKEN,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

    void run() {
        switch (ERaState::get()) {
            case StateT::STATE_CONNECTED:
                ERaState::set(StateT::STATE_RUNNING);
                break;
            case StateT::STATE_RUNNING:
                Base::run();
                break;
            default:
                if (this->connect()) {
                    ERaState::set(StateT::STATE_CONNECTED);
                }
                break;
        }
    }

protected:
private:
    bool connect() {
        if (!this->numClient) {
            return false;
        }
        for (int i = 0; i < this->numClient; ++i) {
            ERaWatchdogFeed();

            if (this->eClient[i].cb != nullptr) {
                int16_t signal = this->eClient[i].cb();
                if (!signal) {
                    ERA_LOG_ERROR(TAG, ERA_PSTR("%s connect network failed"), this->eClient[i].name.c_str());
                    continue;
                }
                this->getTransp().setSignalQuality(signal);
            }
            else {
                this->getTransp().setSignalQuality(100);
            }

            ERaWatchdogFeed();

            this->getTransp().setClient(this->eClient[i].client);
            this->getTransp().setSSID(this->eClient[i].name.c_str());
            if (!Base::connect()) {
                ERA_LOG_ERROR(TAG, ERA_PSTR("Switch to %s client failed"), this->eClient[i].name.c_str());
                continue;
            }

            ERaWatchdogFeed();

            ERaOptConnected(this);

            ERaWatchdogFeed();

            ERA_LOG(TAG, ERA_PSTR("Switch to %s client OK"), this->eClient[i].name.c_str());
            return true;
        }
        return false;
    }

    int numClient;
    const char* authToken;
    EntryClient_t eClient[ERA_MAX_CLIENT];
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

#if defined(ESP32)
    cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, static_cast<uint16_t>(temperatureRead() * 100.0f));
#else
    cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
#endif
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

#endif /* INC_ERA_MULTI_CLIENT_HPP_ */
