#ifndef INC_ERA_TINY_GSM_CLIENT_HPP_
#define INC_ERA_TINY_GSM_CLIENT_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "GSM"
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
#include <Adapters/ERaSMSDef.hpp>

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
#if defined(TINY_GSM_MODEM_HAS_SMS)
    typedef struct __SMSInfo_t {
        char* to;
        char* message;
    } SMSInfo_t;
#endif

    const char* TAG = "GSM";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;

public:
    ERaGsm(Transport& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , modem(nullptr)
        , authToken(nullptr)
        , pinCode(nullptr)
        , powerPin(-1)
        , invertPin(false)
        , softRestart(false)
        , prevMillisSignal(0UL)
    {}
    ~ERaGsm()
    {}

    void setSIMPinCode(const char* code) {
        this->pinCode = code;
    }

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
        this->getTransp().setSSID(apn);
        return true;
    }

    bool connectGPRS(const char* apn, const char* user, const char* pass) {
        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Restarting modem..."));
        this->restart();

        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Modem init"));
        if (!this->modem->begin(this->pinCode)) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Can't init modem"));
            return false;
        }

        ERaWatchdogFeed();

        switch (this->modem->getSimStatus()) {
            case SimStatus::SIM_ERROR:
                ERA_LOG_ERROR(TAG, ERA_PSTR("SIM is missing"));
                break;
            case SimStatus::SIM_LOCKED:
                ERA_LOG_ERROR(TAG, ERA_PSTR("SIM is PIN-locked"));
                break;
            default:
                break;
        }

        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Connecting to network..."));
        if (this->modem->waitForNetwork()) {
            ERA_LOG(TAG, ERA_PSTR("Network: %s"), this->modem->getOperator().c_str());
        }
        else {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Register in network failed"));
            return false;
        }

        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Connecting to %s..."), apn);
        if (!this->modem->gprsConnect(apn, user, pass)) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Connect GPRS failed"));
            return false;
        }

        ERaWatchdogFeed();

        ERA_LOG(TAG, ERA_PSTR("Connected to GPRS"));
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
                const char* apn,
                const char* user,
                const char* pass,
                int pwrPin,
                bool invert,
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(gsm, auth, host, port, username, password);
        this->setPowerPin(pwrPin, invert);
        this->connectNetwork(apn, user, pass);
        this->setNetwork(apn, user, pass);
    }

    void begin(const char* auth,
                TinyGsm& gsm,
                const char* apn,
                const char* user,
                const char* pass,
                int pwrPin = -1,
                bool invert = false,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, gsm, apn,
                    user, pass, pwrPin, invert,
                    host, port, auth, auth);
    }

    void begin(TinyGsm& gsm,
                const char* apn,
                const char* user,
                const char* pass,
                int pwrPin = -1,
                bool invert = false) {
        this->begin(ERA_AUTHENTICATION_TOKEN, gsm, apn, user, pass,
                    pwrPin, invert, ERA_MQTT_HOST, ERA_MQTT_PORT,
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
#if defined(TINY_GSM_MODEM_HAS_SMS)
                this->sendSMS();
#endif
                break;
            case StateT::STATE_CONNECTING_NEW_NETWORK:
                Base::connectNewNetworkResult();
                break;
            case StateT::STATE_REQUEST_LIST_WIFI:
                Base::responseListWiFi();
                break;
            default:
                if (this->netConnected() ||
                    this->connectNetwork(ERaConfig.apn,
                                        ERaConfig.user, ERaConfig.pass)) {
                    ERaWatchdogFeed();

                    this->getTransp().setSignalQuality(this->modem->getSignalQuality());

                    ERaWatchdogFeed();

                    ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                }
                break;
        }
    }

protected:
#if defined(TINY_GSM_MODEM_HAS_SMS)
    void sendSMS(const char* to, const char* message) override {
        if ((to == nullptr) || (message == nullptr)) {
            return;
        }
        if (!this->queue.writeable()) {
            return;
        }
        SMSInfo_t req;
        req.to = ERaStrdup(to);
        req.message = ERaStrdup(message);
        this->queue += req;
    }

    void sendEachSMS(const char* to, const char* message) {
        if ((to == nullptr) || (message == nullptr)) {
            return;
        }
        ERaDataBuff arrayTo(to, strlen(to) + 1);
        arrayTo.split(",");
        size_t size = arrayTo.size();
        for (size_t i = 0; i < size; ++i) {
            if (arrayTo[i].isEmpty()) {
                continue;
            }
            const char* phone = arrayTo[i].trim_str();
            if (!ERaWidgetWriteSMS(phone, message)) {
                this->modem->sendSMS(phone, message);
            }
            ERaWatchdogFeed();
        }
    }

    void sendSMS() {
        if (!this->isRequest()) {
            return;
        }
        SMSInfo_t& req = this->getRequest();
        this->sendEachSMS(req.to, req.message);
        if (req.to != nullptr) {
            free(req.to);
            req.to = nullptr;
        }
        if (req.message != nullptr) {
            free(req.message);
            req.message = nullptr;
        }
    }
#endif

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

    void setNetwork(const char* apn, const char* user, const char* pass) {
        CopyToArray(apn, ERaConfig.apn);
        CopyToArray(user, ERaConfig.user);
        CopyToArray(pass, ERaConfig.pass);
    }

    bool netConnected() const {
        return (this->modem->isNetworkConnected() &&
                this->modem->isGprsConnected());
    }

    void setPowerPin(int pin, bool invert) {
        if (pin < 0) {
            return;
        }
        this->powerPin = pin;
        this->invertPin = invert;
        pinMode(this->powerPin, OUTPUT);
    }

    void restart() {
        if (this->powerPin < 0) {
            this->softwareReset();
            return;
        }

        ERaWatchdogFeed();

        this->powerOff();
        ERaDelay(2000);
        this->powerOn();
        ERaDelay(500);

        ERaWatchdogFeed();

        this->softwareReset();
    }

    void softwareReset() {
        if (!this->softRestart) {
            return;
        }
        this->modem->restart();
    }

    void powerOn() {
        if (this->powerPin < 0) {
            return;
        }
        ::digitalWrite(this->powerPin, !this->invertPin ? LOW : HIGH);
    }

    void powerOff() {
        if (this->powerPin < 0) {
            return;
        }
        ::digitalWrite(this->powerPin, !this->invertPin ? HIGH : LOW);
    }

#if defined(TINY_GSM_MODEM_HAS_SMS)
    bool isRequest() {
        return this->queue.readable();
    }

    SMSInfo_t& getRequest() {
        return this->queue;
    }

    bool isEmptyRequest() {
        return this->queue.isEmpty();
    }

    ERaQueue<SMSInfo_t, ERA_MAX_SMS> queue;
#endif

    TinyGsm* modem;
#if defined(ERA_USE_SSL)
    TinyGsmClientSecure client;
#else
    TinyGsmClient client;
#endif
    const char* authToken;
    const char* pinCode;
    int powerPin;
    bool invertPin;
    bool softRestart;

    unsigned long prevMillisSignal;
};

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    const char* ssid = this->thisProto().getTransp().getSSID();
    int16_t signal = this->thisProto().getTransp().getSignalQuality();

    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, 0);
    cJSON_AddStringToObject(root, INFO_NETWORK_PROTOCOL, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_SSID, ((ssid != nullptr) ? ssid : ERA_NETWORK_TYPE));
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, signal);
    cJSON_AddNumberToObject(root, INFO_SIGNAL_STRENGTH, SignalToPercentage(signal));
    cJSON_AddStringToObject(root, INFO_MAC, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_SSL, ERaInfoSSL());

    /* Override info */
    ERaInfo(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addSelfInfo(cJSON* root) {
    int16_t signal = this->thisProto().getTransp().getSignalQuality();

#if defined(ESP32)
    cJSON_AddNumberToObject(root, SELF_CHIP_TEMPERATURE, static_cast<uint16_t>(temperatureRead() * 100.0f));
#else
    cJSON_AddNumberToObject(root, SELF_CHIP_TEMPERATURE, 5000);
#endif
    cJSON_AddNumberToObject(root, SELF_SIGNAL_STRENGTH, SignalToPercentage(signal));

    /* Override self info */
    ERaSelfInfo(root);
}

#if defined(ERA_MODBUS)
    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
        const char* ssid = this->thisProto().getTransp().getSSID();
        int16_t signal = this->thisProto().getTransp().getSignalQuality();

    #if defined(ESP32)
        cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, static_cast<uint16_t>(temperatureRead() * 100.0f));
    #else
        cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
    #endif
        cJSON_AddNumberToObject(root, INFO_MB_RSSI, signal);
        cJSON_AddNumberToObject(root, INFO_MB_SIGNAL_STRENGTH, SignalToPercentage(signal));
        cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ((ssid != nullptr) ? ssid : ERA_NETWORK_TYPE));

        /* Override modbus info */
        ERaModbusInfo(root);
    }
#endif

#endif /* INC_ERA_TINY_GSM_CLIENT_HPP_ */
