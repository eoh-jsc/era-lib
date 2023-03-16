#ifndef INC_ERA_MULTI_CLIENT_HPP_
#define INC_ERA_MULTI_CLIENT_HPP_

#if !defined(ERA_PROTO_TYPE)
    #define ERA_PROTO_TYPE            "Multi"
#endif

#define ERA_MAX_CLIENT  3

#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqtt.hpp>

#if defined(__has_include) &&       \
    __has_include(<functional>) &&  \
    !defined(ERA_IGNORE_STD_FUNCTIONAL_STRING)
    #include <functional>
    #define ADAPTER_HAS_FUNCTIONAL_H
#endif

class ERaFlash;

template <class Transport>
class ERaMulti
    : public ERaProto<Transport, ERaFlash>
{
#if defined(TIMER_HAS_FUNCTIONAL_H)
    typedef std::function<int16_t(void)> GetSignalCallback_t;
#else
    typedef int16_t (*GetSignalCallback_t)(void);
#endif

    const char* TAG = "Multi";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;

    typedef struct __EntryClient_t {
        String name;
        Client* client;
        GetSignalCallback_t cb;
    } EntryClient_t;

public:
    ERaMulti(Transport& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , numClient(0)
        , authToken(nullptr)
    {}
    ~ERaMulti()
    {}

    bool addClient(const String& name, Client& client,
                    GetSignalCallback_t cb = nullptr) {
        if (this->numClient >= ERA_MAX_CLIENT) {
            return false;
        }
        EntryClient_t& e = this->eClient[this->numClient++];
        e.name = name;
        e.client = &client;
        e.cb = cb;
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
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::init();
        this->config(auth, host, port, username, password);
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
            if (this->eClient[i].cb != nullptr) {
                this->getTransp().setSignalQuality(this->eClient[i].cb());
            }
            else {
                this->getTransp().setSignalQuality(100);
            }
            this->getTransp().setClient(this->eClient[i].client);
            this->getTransp().setSSID(this->eClient[i].name.c_str());
            if (!Base::connect()) {
                ERA_LOG(TAG, ERA_PSTR("Switch to %s client failed"), this->eClient[i].name.c_str());
                continue;
            }
            ERaOptConnected(this);
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
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_BOARD_ID, this->thisProto().getBoardID());
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().getAuth());
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, ((this->thisProto().getTransp().getSSID() == nullptr) ?
                                            ERA_PROTO_TYPE : this->thisProto().getTransp().getSSID()));
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_PROTO_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, this->thisProto().getTransp().getSignalQuality());
    cJSON_AddStringToObject(root, INFO_MAC, ERA_PROTO_TYPE);
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ERA_PROTO_TYPE);
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().getTransp().getPing());
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
#if defined(ESP32)
    cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, static_cast<uint16_t>(temperatureRead() * 100.0f));
#else
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
#endif
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, this->thisProto().getTransp().getSignalQuality());
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ((this->thisProto().getTransp().getSSID() == nullptr) ?
                                                    ERA_PROTO_TYPE : this->thisProto().getTransp().getSSID()));
}

#endif /* INC_ERA_MULTI_CLIENT_HPP_ */
