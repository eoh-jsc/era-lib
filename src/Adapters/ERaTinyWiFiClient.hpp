#ifndef INC_ERA_TINY_WIFI_CLIENT_HPP_
#define INC_ERA_TINY_WIFI_CLIENT_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "WiFi"
#endif

#if !defined(ERA_AUTH_TOKEN)
    #error "Please specify your ERA_AUTH_TOKEN"
#endif

#if !defined(ERA_TINY_GSM)
    #define ERA_TINY_GSM
#endif

#include <TinyGsmClient.hpp>
#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqtt.hpp>

#define WIFI_NET_CONNECT_TIMEOUT      3 * 60000

typedef struct __ERaConfig_t {
    char ssid[64];
    char pass[64];
} ERA_ATTR_PACKED ERaConfig_t;

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaConfig_t ERaConfig {};
#else
    #define ERaConfig  StaticHelper<ERaConfig_t>::instance()
#endif

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
        , invertPin(false)
        , softRestart(false)
        , prevMillisSignal(0UL)
    {}
    ~ERaWiFi()
    {}

    bool connectNetwork(const char* ssid, const char* pass) {
        MillisTime_t startMillis = ERaMillis();
        while (!this->connectWiFi(ssid, pass)) {
            ERaDelay(500);
            Base::appLoop();
            ERaWatchdogFeed();
            if (!ERaRemainingTime(startMillis, WIFI_NET_CONNECT_TIMEOUT)) {
                return false;
            }
        }
        this->getTransp().setSSID(ssid);
        return true;
    }

    bool connectWiFi(const char* ssid, const char* pass) {
        if (!strlen(ssid)) {
            return false;
        }

        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Restarting modem..."));
        this->restart();

        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Modem init"));
        if (!this->modem->begin()) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Can't init modem"));
            return false;
        }

        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Connecting to %s..."), ssid);
        if (!this->modem->networkConnect(ssid, pass)) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Connect %s failed"), ssid);
            return false;
        }

        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Waiting for network..."));
        if (!this->modem->waitForNetwork()) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Connect %s failed"), ssid);
            return false;
        }

        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Connected to %s"), ssid);
        return true;
    }

    void restartOption(bool soft) {
        this->softRestart = soft;
    }

    void config(TinyGsm& gsm,
                const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::begin(auth);
        this->modem = &gsm;
        this->client.init(this->modem);
        this->getTransp().setClient(&this->client);
        this->getTransp().config(host, port, username, password);
    }

    void begin(const char* auth,
                TinyGsm& gsm,
                const char* ssid,
                const char* pass,
                int rstPin,
                bool invert,
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(gsm, auth, host, port, username, password);
        this->setResetPin(rstPin, invert);
        this->connectNetwork(ssid, pass);
        this->setNetwork(ssid, pass);
    }

    void begin(const char* auth,
                TinyGsm& gsm,
                const char* ssid,
                const char* pass,
                int rstPin = -1,
                bool invert = false,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, gsm, ssid,
                    pass, rstPin, invert,
                    host, port, auth, auth);
    }

    void begin(TinyGsm& gsm,
                const char* ssid,
                const char* pass,
                int rstPin = -1,
                bool invert = false) {
        this->begin(ERA_AUTHENTICATION_TOKEN, gsm, ssid, pass,
                    rstPin, invert, ERA_MQTT_HOST, ERA_MQTT_PORT,
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
                this->getSignalQuality();
                break;
            default:
                if (this->netConnected() ||
                    this->connectNetwork(ERaConfig.ssid, ERaConfig.pass)) {
                    ERaWatchdogFeed();

                    this->getTransp().setSignalQuality(this->modem->getSignalQuality());

                    ERaWatchdogFeed();

                    ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                }
                break;
        }
    }

protected:
private:
    void getSignalQuality() {
        unsigned long currentMillis = ERaMillis();
        if ((currentMillis - this->prevMillisSignal) < ERA_GET_SIGNAL_TIMEOUT) {
            return;
        }
        unsigned long skipTimes = ((currentMillis - this->prevMillisSignal) / ERA_GET_SIGNAL_TIMEOUT);
        // update time
        this->prevMillisSignal += (ERA_GET_SIGNAL_TIMEOUT * skipTimes);

        ERaWatchdogFeed();

        this->getTransp().setSignalQuality(this->modem->getSignalQuality());

        ERaWatchdogFeed();
    }

    void setNetwork(const char* ssid, const char* pass) {
        CopyToArray(ssid, ERaConfig.ssid);
        CopyToArray(pass, ERaConfig.pass);
    }

    bool netConnected() const {
        return this->modem->isNetworkConnected();
    }

    void setResetPin(int pin, bool invert) {
        if (pin < 0) {
            return;
        }
        this->resetPin = pin;
        this->invertPin = invert;
        pinMode(this->resetPin, OUTPUT);
    }

    void restart() {
        if (this->resetPin < 0) {
            return;
        }

        ERaWatchdogFeed();

        this->powerOff();
        ERaDelay(2000);
        this->powerOn();
        ERaDelay(500);

        ERaWatchdogFeed();

        if (this->softRestart) {
            this->modem->restart();
        }
    }

    void powerOn() {
        if (this->resetPin < 0) {
            return;
        }
        ::digitalWrite(this->resetPin, !this->invertPin ? HIGH : LOW);
    }

    void powerOff() {
        if (this->resetPin < 0) {
            return;
        }
        ::digitalWrite(this->resetPin, !this->invertPin ? LOW : HIGH);
    }

    TinyGsm* modem;
#if defined(ERA_USE_SSL)
    TinyGsmClientSecure client;
#else
    TinyGsmClient client;
#endif
    const char* authToken;
    int resetPin;
    bool invertPin;
    bool softRestart;

    unsigned long prevMillisSignal;
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

#endif /* INC_ERA_TINY_WIFI_CLIENT_HPP_ */
