#ifndef INC_ERA_MQTT_HPP_
#define INC_ERA_MQTT_HPP_

#include <Utility/ERaUtility.hpp>
#include <Utility/ERacJSON.hpp>
#include <ERa/ERaDefine.hpp>
#include <ERa/ERaConfig.hpp>
#include <ERa/ERaApi.hpp>
#include "MQTT/MQTT.h"
#include "ERaMqttHelper.hpp"

#define ERA_MQTT_PUB_LOG(status, errorCode)                                                                                             \
    if (status) {                                                                                                                       \
        ERA_LOG(TAG, ERA_PSTR("Publish (ok #%d) %s: %s"), this->mqtt.lastPacketID(), topic, payload);                                   \
    }                                                                                                                                   \
    else {                                                                                                                              \
        ERA_LOG_ERROR(TAG, ERA_PSTR("Publish (error %d #%d) %s: %s"), errorCode, this->mqtt.lastPacketID(), topic, payload);            \
    }

#define ERA_MQTT_SUB_LOG(status, errorCode)                                                                                             \
    if (status) {                                                                                                                       \
        ERA_LOG(TAG, ERA_PSTR("Subscribe (ok #%d): %s, QoS: %d"), this->mqtt.lastPacketID(), topicName, qos);                           \
    }                                                                                                                                   \
    else {                                                                                                                              \
        ERA_LOG_ERROR(TAG, ERA_PSTR("Subscribe (error %d #%d): %s, QoS: %d"), errorCode, this->mqtt.lastPacketID(), topicName, qos);    \
    }

#define ERA_MQTT_UNSUB_LOG(status, errorCode)                                                                                           \
    if (status) {                                                                                                                       \
        ERA_LOG(TAG, ERA_PSTR("Unsubscribe (ok #%d): %s"), this->mqtt.lastPacketID(), topicName);                                       \
    }                                                                                                                                   \
    else {                                                                                                                              \
        ERA_LOG_ERROR(TAG, ERA_PSTR("Unsubscribe (error %d #%d): %s"), errorCode, this->mqtt.lastPacketID(), topicName);                \
    }

using namespace std;

template <class Client, class MQTT>
class ERaMqtt
    : public ERaMqttHelper
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> StateCallback_t;
    typedef std::function<void(void)> FunctionCallback_t;
    typedef std::function<void(const char*, const char*)> MessageCallback_t;
#else
    typedef void (*StateCallback_t)(void);
    typedef void (*FunctionCallback_t)(void);
    typedef void (*MessageCallback_t)(const char*, const char*);
#endif

    const char* TAG = "MQTT";
    const char* ONLINE_MESSAGE = R"json({"ol":1%s%s})json";
    const char* OFFLINE_MESSAGE = R"json({"ol":0})json";
    const char* WIFI_INFO = R"json(,"wifi_ssid":"%s")json";
    const char* ASK_CONFIG_INFO = R"json(,"ask_configuration":1)json";
    const char* LWT_TOPIC = ERA_PREFIX_LWT_TOPIC;
    const bool LWT_RETAINED = true;
    const int LWT_QOS = QoST::QOS1;

public:
    ERaMqtt()
        : client(NULL)
        , mqtt(0)
        , host(ERA_MQTT_HOST)
        , port(ERA_MQTT_PORT)
        , clientID(ERA_MQTT_CLIENT_ID)
        , username(ERA_MQTT_USERNAME)
        , password(ERA_MQTT_PASSWORD)
        , ERaTopic(ERA_MQTT_BASE_TOPIC)
        , ERaAuth(ERA_AUTHENTICATION_TOKEN)
        , ssid(NULL)
        , ping(0L)
        , lastPublish(0L)
        , signalQuality(0)
        , askConfig(false)
        , needPubState(false)
        , _connected(false)
        , mutex(NULL)
        , appCb(NULL)
        , connectedCb(NULL)
        , disconnectedCb(NULL)
    {
        this->mqtt.setKeepAlive(ERA_MQTT_KEEP_ALIVE);
        memset(this->willTopic, 0, sizeof(this->willTopic));
    }
    ERaMqtt(Client& _client)
        : client(NULL)
        , mqtt(0)
        , host(ERA_MQTT_HOST)
        , port(ERA_MQTT_PORT)
        , clientID(ERA_MQTT_CLIENT_ID)
        , username(ERA_MQTT_USERNAME)
        , password(ERA_MQTT_PASSWORD)
        , ERaTopic(ERA_MQTT_BASE_TOPIC)
        , ERaAuth(ERA_AUTHENTICATION_TOKEN)
        , ssid(NULL)
        , ping(0L)
        , signalQuality(0)
        , askConfig(false)
        , _connected(false)
        , mutex(NULL)
        , appCb(NULL)
        , connectedCb(NULL)
        , disconnectedCb(NULL)
    {
        this->setClient(&_client);
        this->mqtt.setKeepAlive(ERA_MQTT_KEEP_ALIVE);
        memset(this->willTopic, 0, sizeof(this->willTopic));
    }
    ~ERaMqtt()
    {}

    void setClient(Client* _client) {
        this->client = _client;
        this->mqtt.setClient(_client);
    }

    Client* getClient() {
        return this->client;
    }

    void begin(const char* _host, uint16_t _port,
            const char* _username, const char* _password) {
        this->config(_host, _port, _username, _password);
    }

    void init(int readBufSize = ERA_MQTT_RX_BUFFER_SIZE,
            int writeBufSize = ERA_MQTT_TX_BUFFER_SIZE) {
        this->mqtt.init(readBufSize, writeBufSize);
    }

    void config(const char* _host, uint16_t _port,
            const char* _username, const char* _password);
    bool connect(FunctionCallback_t fn = NULL);
    void disconnect();
    bool connected();
    bool run();
    bool subscribeTopic(const char* baseTopic, const char* topic,
                        QoST qos = (QoST)ERA_MQTT_SUBSCRIBE_QOS) override;
    bool unsubscribeTopic(const char* baseTopic, const char* topic) override;
    bool publishData(const char* topic, const char* payload,
                    bool retained = ERA_MQTT_PUBLISH_RETAINED,
                    QoST qos = (QoST)ERA_MQTT_PUBLISH_QOS) override;
    bool publishState(bool online);
    bool syncConfig();

    void setTimeout(uint32_t timeout) {
        this->mqtt.setTimeout(timeout);
    }

    void setSkipACK(bool skip = true) {
        this->mqtt.setSkipACK(skip);
    }

    void setKeepAlive(uint16_t keepAlive) {
        this->mqtt.setKeepAlive(keepAlive);
    }

    void setClientID(const char* id) {
        this->clientID = id;
    }

    const char* getClientID() const {
        if (this->clientID == NULL) {
            return "";
        }
        return this->clientID;
    }

    void setTopic(const char* topic) {
        this->ERaTopic = topic;
    }

    void setAuth(const char* auth) {
        this->ERaAuth = auth;
    }

    const char* getAuth() const {
        return this->ERaAuth;
    }

    void setSSID(const char* _ssid) {
        this->ssid = _ssid;
    }

    const char* getSSID() const {
        return this->ssid;
    }

    MillisTime_t getPing() const {
        return this->ping;
    }

    void setSignalQuality(int16_t signal) {
        this->signalQuality = signal;
    }

    int16_t getSignalQuality() const {
        return this->signalQuality;
    }

    void setAskConfig(bool enable) {
        this->askConfig = enable;
    }

    bool getAskConfig() const {
        return this->askConfig;
    }

    const char* getTag() const {
        return this->TAG;
    }

    const char* getLWTTopic() const {
        return this->LWT_TOPIC;
    }

    const char* getLWTPayload() const {
        return this->OFFLINE_MESSAGE;
    }

    void onMessage(MessageCallback_t cb) {
        this->mqtt.onMessage(cb);
    }

    void onAppLoop(FunctionCallback_t cb) {
        this->appCb = cb;
    }

    void onStateChange(StateCallback_t onCb,
                       StateCallback_t offCb) {
        this->connectedCb = onCb;
        this->disconnectedCb = offCb;
    }

    bool connectionDenied() {
        return ((this->mqtt.lastError() == lwmqtt_err_t::LWMQTT_MISSING_OR_WRONG_PACKET) ||
                (this->mqtt.lastError() == lwmqtt_err_t::LWMQTT_CONNECTION_DENIED));
    }

protected:
private:
    void delays(MillisTime_t ms);
    bool publishLWT(bool sync = false);
    void onConnected();
    void onDisconnected();
    void lockMQTT();
    void unlockMQTT();

    Client* client;
    MQTT mqtt;
    const char* host;
    uint16_t port;
    const char* clientID;
    const char* username;
    const char* password;
    const char* ERaTopic;
    const char* ERaAuth;
    const char* ssid;
    MillisTime_t ping;
    MillisTime_t lastPublish;
    int16_t signalQuality;
    bool askConfig;
    bool needPubState;
    bool _connected;
    char willTopic[MAX_TOPIC_LENGTH];
    ERaMutex_t mutex;
    FunctionCallback_t appCb;
    StateCallback_t connectedCb;
    StateCallback_t disconnectedCb;
};

template <class Client, class MQTT>
inline
void ERaMqtt<Client, MQTT>::config(const char* _host, uint16_t _port,
                                const char* _username, const char* _password) {
    this->host = _host;
    this->port = _port;
    this->username = _username;
    this->password = _password;
    ClearArray(this->willTopic);
    FormatString(this->willTopic, this->ERaTopic);
    FormatString(this->willTopic, LWT_TOPIC);
    this->mqtt.init(ERA_MQTT_RX_BUFFER_SIZE,
                    ERA_MQTT_TX_BUFFER_SIZE);
    this->mqtt.setWill(this->willTopic, OFFLINE_MESSAGE, LWT_RETAINED, LWT_QOS);
    this->mqtt.begin(this->host, this->port, *this->client);
}

template <class Client, class MQTT>
inline
bool ERaMqtt<Client, MQTT>::connect(FunctionCallback_t fn) {
    size_t count {0};
    char _clientID[74] {0};
    if (this->clientID != NULL) {
        FormatString(_clientID, this->ERaAuth);
        if (!ERaStrCmp(this->clientID, this->ERaAuth) &&
            strlen(this->clientID)) {
            FormatString(_clientID, "_%s", this->clientID);
        }
    }

    ERaWatchdogFeed();

    this->mqtt.disconnect();
    this->_connected = false;

    ERaWatchdogFeed();

    while (this->mqtt.connect(_clientID, this->username, this->password) == false) {
        ERaWatchdogFeed();

        ERA_LOG_ERROR(TAG, ERA_PSTR("MQTT(%d): Connect failed (%d), retrying in 5 seconds"),
                                    (count + 1), this->mqtt.lastError());
        this->delays(5000);

        ERaWatchdogFeed();

        if (++count >= LIMIT_CONNECT_BROKER_MQTT) {
            return false;
        }
    }

    ERaWatchdogFeed();

#if defined(ERA_ZIGBEE) ||  \
    defined(ERA_SPECIFIC)
    this->subscribeTopic(this->ERaTopic, ERA_SUB_PREFIX_ZIGBEE_GET_TOPIC);
    this->subscribeTopic(this->ERaTopic, ERA_SUB_PREFIX_ZIGBEE_DOWN_TOPIC);
    this->subscribeTopic(this->ERaTopic, ERA_SUB_PREFIX_ZIGBEE_JOIN_TOPIC);
    this->subscribeTopic(this->ERaTopic, ERA_SUB_PREFIX_ZIGBEE_REMOVE_TOPIC);
#endif

    this->subscribeTopic(this->ERaTopic, ERA_SUB_PREFIX_ARDUINO_TOPIC);
    this->subscribeTopic(this->ERaTopic, ERA_SUB_PREFIX_VIRTUAL_TOPIC);
    this->subscribeTopic(this->ERaTopic, ERA_SUB_PREFIX_OP_DOWN_TOPIC);
    this->subscribeTopic(this->ERaTopic, ERA_SUB_PREFIX_DOWN_TOPIC);

    ERaWatchdogFeed();

    if (fn != NULL) {
        fn();
    }

    ERaWatchdogFeed();

#if defined(ERA_ASK_CONFIG_WHEN_RESTART)
    if (!this->publishLWT(true)) {
        return false;
    }
#else
    if (!this->publishLWT(this->getAskConfig())) {
        return false;
    }
#endif

    this->subscribeTopic(this->ERaTopic, LWT_TOPIC);
    ERA_LOG(TAG, ERA_PSTR(R"(Connected: "%s:%d", Client ID: "%s", Username: "%s", Password: "%s")"),
                            this->host, this->port, _clientID, this->username, this->password);

    ERaWatchdogFeed();

    this->onConnected();

    ERaWatchdogFeed();

    return true;
}

template <class Client, class MQTT>
inline
void ERaMqtt<Client, MQTT>::disconnect() {
    this->mqtt.disconnect();
}

template <class Client, class MQTT>
inline
bool ERaMqtt<Client, MQTT>::connected() {
    return this->mqtt.connected();
}

template <class Client, class MQTT>
inline
bool ERaMqtt<Client, MQTT>::run() {
    if (!this->_connected) {
        return false;
    }

    if (!this->mqtt.loop()) {
        ERaWatchdogFeed();

        this->onDisconnected();

        ERaWatchdogFeed();

        return this->connect();
    }
    else if (this->needPubState) {
        this->publishLWT(false);
        this->needPubState = false;
    }
    return true;
}

template <class Client, class MQTT>
inline
bool ERaMqtt<Client, MQTT>::subscribeTopic(const char* baseTopic, const char* topic,
                                            QoST qos) {
    bool status {false};
    char topicName[MAX_TOPIC_LENGTH] {0};
    if (baseTopic != NULL) {
        FormatString(topicName, baseTopic);
    }
    if (topic != NULL) {
        FormatString(topicName, topic);
    }

    this->lockMQTT();
    if (this->connected()) {
        status = this->mqtt.subscribe(topicName, qos);
        ERA_MQTT_SUB_LOG(status, this->mqtt.lastError())
    }
    this->unlockMQTT();

    return status;
}

template <class Client, class MQTT>
inline
bool ERaMqtt<Client, MQTT>::unsubscribeTopic(const char* baseTopic, const char* topic) {
    bool status {false};
    char topicName[MAX_TOPIC_LENGTH] {0};
    if (baseTopic != NULL) {
        FormatString(topicName, baseTopic);
    }
    if (topic != NULL) {
        FormatString(topicName, topic);
    }

    this->lockMQTT();
    if (this->connected()) {
        status = this->mqtt.unsubscribe(topicName);
        ERA_MQTT_UNSUB_LOG(status, this->mqtt.lastError())
    }
    this->unlockMQTT();

    return status;
}

template <class Client, class MQTT>
inline
bool ERaMqtt<Client, MQTT>::publishData(const char* topic, const char* payload,
                                        bool retained, QoST qos) {
    if (!this->_connected) {
        return false;
    }

    bool status {false};

    this->lockMQTT();
    if (this->connected()) {
        this->lastPublish = ERaMillis();
        status = this->mqtt.publish(topic, payload, retained, qos);
        this->ping = (ERaMillis() - this->lastPublish);
        ERA_MQTT_PUB_LOG(status, this->mqtt.lastError())
    }
    this->unlockMQTT();

    return status;
}

template <class Client, class MQTT>
inline
bool ERaMqtt<Client, MQTT>::publishState(bool online) {
    this->needPubState = online;
    return true;
}

template <class Client, class MQTT>
inline
bool ERaMqtt<Client, MQTT>::syncConfig() {
    return this->publishLWT(true);
}

template <class Client, class MQTT>
inline
void ERaMqtt<Client, MQTT>::delays(MillisTime_t ms) {
    if (!ms) {
        return;
    }
    MillisTime_t startMillis = ERaMillis();
    while (ERaRemainingTime(startMillis, ms)) {
        if (this->appCb != NULL) {
            this->appCb();
        }
        ERaDelay(10);
    }
}

template <class Client, class MQTT>
inline
bool ERaMqtt<Client, MQTT>::publishLWT(bool sync) {
    bool status {false};
    char wifiInfo[50] {0};
    char payload[100] {0};
    char* topic = this->willTopic;

    if (this->getSSID() != NULL) {
        FormatString(wifiInfo, WIFI_INFO, this->getSSID());
    }

    FormatString(payload, ONLINE_MESSAGE, wifiInfo, sync ? ASK_CONFIG_INFO : "");

    this->lockMQTT();
    if (this->connected()) {
        this->lastPublish = ERaMillis();
        status = this->mqtt.publish(topic, payload, LWT_RETAINED, LWT_QOS);
        this->ping = (ERaMillis() - this->lastPublish);
        ERA_MQTT_PUB_LOG(status, this->mqtt.lastError())
    }
    this->unlockMQTT();

    return status;
}

template <class Client, class MQTT>
inline
void ERaMqtt<Client, MQTT>::onConnected() {
    if (this->_connected) {
        return;
    }
    this->_connected = true;
    if (this->connectedCb == NULL) {
        return;
    }
    this->connectedCb();
}

template <class Client, class MQTT>
inline
void ERaMqtt<Client, MQTT>::onDisconnected() {
    if (!this->_connected) {
        return;
    }
    this->_connected = false;
    if (this->disconnectedCb == NULL) {
        return;
    }
    this->disconnectedCb();
}

template <class Client, class MQTT>
inline
void ERaMqtt<Client, MQTT>::lockMQTT() {
    if (!this->mqtt.getSkipACK()) {
        return;
    }
    ERaGuardLock(this->mutex);
}

template <class Client, class MQTT>
inline
void ERaMqtt<Client, MQTT>::unlockMQTT() {
    if (!this->mqtt.getSkipACK()) {
        return;
    }
    ERaGuardUnlock(this->mutex);
}

#endif /* INC_ERA_MQTT_HPP_ */
