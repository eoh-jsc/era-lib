#ifndef INC_ERA_MQTT_LINUX_HPP_
#define INC_ERA_MQTT_LINUX_HPP_

#include <Utility/ERaUtility.hpp>
#include <Utility/ERacJSON.hpp>
#include <ERa/ERaDefine.hpp>
#include <ERa/ERaConfig.hpp>
#include <ERa/ERaApi.hpp>
#include "MQTT/MQTT.hpp"
#include "ERaMqttHelper.hpp"

#define ERA_MQTT_PUB_LOG(status, errorCode)                                                                                                             \
    if (status) {                                                                                                                                       \
        ERA_LOG(TAG, ERA_PSTR("Publish (ok #%d) %s (% 3d): %s"), this->mqtt.lastPacketID(), topic, strlen(payload), payload);                           \
    }                                                                                                                                                   \
    else {                                                                                                                                              \
        ERA_LOG_ERROR(TAG, ERA_PSTR("Publish (error %d #%d) %s (% 3d): %s"), errorCode, this->mqtt.lastPacketID(), topic, strlen(payload), payload);    \
    }

#define ERA_MQTT_SUB_LOG(status, errorCode)                                                                                                             \
    if (status) {                                                                                                                                       \
        ERA_LOG(TAG, ERA_PSTR("Subscribe (ok #%d): %s, QoS: %d"), this->mqtt.lastPacketID(), topicName, qos);                                           \
    }                                                                                                                                                   \
    else {                                                                                                                                              \
        ERA_LOG_ERROR(TAG, ERA_PSTR("Subscribe (error %d #%d): %s, QoS: %d"), errorCode, this->mqtt.lastPacketID(), topicName, qos);                    \
    }

#define ERA_MQTT_UNSUB_LOG(status, errorCode)                                                                                                           \
    if (status) {                                                                                                                                       \
        ERA_LOG(TAG, ERA_PSTR("Unsubscribe (ok #%d): %s"), this->mqtt.lastPacketID(), topicName);                                                       \
    }                                                                                                                                                   \
    else {                                                                                                                                              \
        ERA_LOG_ERROR(TAG, ERA_PSTR("Unsubscribe (error %d #%d): %s"), errorCode, this->mqtt.lastPacketID(), topicName);                                \
    }

using namespace std;

template <class MQTT>
class ERaMqttLinux
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
    const QoST LWT_QOS = QoST::QOS1;

public:
    ERaMqttLinux()
        : mqtt(0)
        , host(ERA_MQTT_HOST)
        , port(ERA_MQTT_PORT)
        , rootCA(NULL)
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
        , nonBlocking(false)
        , reachedLimit(false)
        , _connected(false)
        , mutex(NULL)
        , appCb(NULL)
        , connectedCb(NULL)
        , disconnectedCb(NULL)
    {
#if defined(ERA_MQTT_SSL)
        this->setRootCA(
            #include <Certs/mqttRootCA.hpp>
        );
#endif
        this->mqtt.setKeepAlive(ERA_MQTT_KEEP_ALIVE);
        memset(this->willTopic, 0, sizeof(this->willTopic));
    }
    ~ERaMqttLinux()
    {}

    void setRootCA(const char* ca) {
        this->rootCA = ca;
        this->mqtt.setRootCA(ca);
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

    void setNonBlocking(bool enable) {
        this->nonBlocking = enable;
    }

    void setConnected(bool enable) {
        this->_connected = enable;
    }

    void setTimeout(uint32_t timeout) {
        this->mqtt.setTimeout(timeout);
    }

    void setSkipACK(bool skip = true) {
        this->mqtt.setSkipACK(skip);
    }

    void setKeepAlive(uint16_t keepAlive) {
        this->mqtt.setKeepAlive(keepAlive);
    }

    void setDropOverflow(bool enabled = false) {
        this->mqtt.dropOverflow(enabled);
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

    const char* getHost() const {
        return this->host;
    }

    uint16_t getPort() const {
        return this->port;
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

    bool reachedConnectionLimit() {
        return this->reachedLimit;
    }

    bool connectionDenied() {
        if (!this->reachedLimit) {
            return false;
        }
        return ((this->mqtt.lastError() == lwmqtt_err_t::LWMQTT_MISSING_OR_WRONG_PACKET) ||
                (this->mqtt.lastError() == lwmqtt_err_t::LWMQTT_CONNECTION_DENIED));
    }

protected:
private:
    bool connectBlocking(const char* id);
    bool connectNonBlocking(const char* id);
    void delays(MillisTime_t ms);
    bool publishLWT(bool sync);
    bool publishLWT(bool sync, bool retained);
    bool publishLWT(bool sync, bool retained, QoST qos);
    void onConnected();
    void onDisconnected();
    void lock();
    void unlock();

    MQTT mqtt;
    const char* host;
    uint16_t port;
    const char* rootCA;
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
    bool nonBlocking;
    bool reachedLimit;
    bool _connected;
    char willTopic[MAX_TOPIC_LENGTH];
    ERaMutex_t mutex;
    FunctionCallback_t appCb;
    StateCallback_t connectedCb;
    StateCallback_t disconnectedCb;
};

template <class MQTT>
inline
void ERaMqttLinux<MQTT>::config(const char* _host, uint16_t _port,
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
    this->mqtt.begin(this->host, this->port);
}

template <class MQTT>
inline
bool ERaMqttLinux<MQTT>::connect(FunctionCallback_t fn) {
    bool status {false};
    char id[74] {0};
    if (this->clientID != NULL) {
        FormatString(id, this->ERaAuth);
        if (!ERaStrCmp(this->clientID, this->ERaAuth) &&
            strlen(this->clientID)) {
            FormatString(id, "_%s", this->clientID);
        }
    }

    if (this->nonBlocking) {
        status = this->connectNonBlocking(id);
    }
    else {
        status = this->connectBlocking(id);
    }
    if (!status) {
        return false;
    }

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
    this->subscribeTopic(this->ERaTopic, ERA_SUB_PREFIX_ASK_WIFI_TOPIC);
    this->subscribeTopic(this->ERaTopic, ERA_SUB_PREFIX_CHANGE_WIFI_TOPIC);

#if defined(ERA_AUTOMATION)
    this->subscribeTopic(this->ERaTopic, ERA_SUB_PREFIX_AUTOMATION_TOPIC);
#endif

    if (fn != NULL) {
        fn();
    }

    if (!this->publishLWT(false)) {
        return false;
    }

    this->delays(500);

#if defined(ERA_ASK_CONFIG_WHEN_RESTART)
    if (!this->publishLWT(true, false)) {
        return false;
    }
#else
    if (this->askConfig && !this->publishLWT(this->askConfig, false)) {
        return false;
    }
#endif

    this->subscribeTopic(this->ERaTopic, LWT_TOPIC);
    ERA_LOG(TAG, ERA_PSTR(R"(Connected: "%s:%d", Client ID: "%s", Username: "%s", Password: "%s")"),
                            this->host, this->port, id, this->username, this->password);

    this->onConnected();
    return true;
}

template <class MQTT>
inline
bool ERaMqttLinux<MQTT>::connectBlocking(const char* id) {
    size_t connectCount {0};

    this->mqtt.disconnect();
    this->_connected = false;
    this->reachedLimit = false;

    while (!this->mqtt.connect(id, this->username, this->password)) {
        if (++connectCount >= LIMIT_CONNECT_BROKER_MQTT) {
            this->reachedLimit = true;
            return false;
        }

        ERA_LOG_ERROR(TAG, ERA_PSTR("MQTT(%d): Connect failed (%d), retrying in 5 seconds"),
                                    connectCount, this->mqtt.lastError());
        this->delays(5000);
        if (this->mqtt.lastError() == lwmqtt_err_t::LWMQTT_NETWORK_FAILED_CONNECT) {
            return false;
        }
    }

    return true;
}

template <class MQTT>
inline
bool ERaMqttLinux<MQTT>::connectNonBlocking(const char* id) {
    bool status {false};
    static size_t connectCount {0};
    static unsigned long lastMillis {0};
    if (!this->reachedLimit && ((ERaMillis() - lastMillis) < 5000)) {
        this->delays(10);
        return false;
    }

    if (!connectCount) {
        this->mqtt.disconnect();
        this->_connected = false;
        this->reachedLimit = false;
    }

    status = this->mqtt.connect(id, this->username, this->password);

    if (status) {
        connectCount = 0;
    }
    else if (++connectCount >= LIMIT_CONNECT_BROKER_MQTT) {
        connectCount = 0;
        this->reachedLimit = true;
    }
    else {
        ERA_LOG_ERROR(TAG, ERA_PSTR("MQTT(%d): Connect failed (%d), retrying in 5 seconds"),
                                    connectCount, this->mqtt.lastError());
    }

    lastMillis = ERaMillis();
    return status;
}

template <class MQTT>
inline
void ERaMqttLinux<MQTT>::disconnect() {
    this->mqtt.disconnect();
}

template <class MQTT>
inline
bool ERaMqttLinux<MQTT>::connected() {
    return this->mqtt.connected();
}

template <class MQTT>
inline
bool ERaMqttLinux<MQTT>::run() {
    if (!this->_connected) {
        return false;
    }

    if (!this->mqtt.loop()) {
        this->onDisconnected();
        return this->connect();
    }
    else if (this->needPubState) {
        this->publishLWT(false);
        this->needPubState = false;
    }
    return true;
}

template <class MQTT>
inline
bool ERaMqttLinux<MQTT>::subscribeTopic(const char* baseTopic, const char* topic,
                                        QoST qos) {
    bool status {false};
    char topicName[MAX_TOPIC_LENGTH] {0};
    if (baseTopic != NULL) {
        FormatString(topicName, baseTopic);
    }
    if (topic != NULL) {
        FormatString(topicName, topic);
    }

    this->lock();
    if (this->connected()) {
        status = this->mqtt.subscribe(topicName, qos);
        ERA_MQTT_SUB_LOG(status, this->mqtt.lastError())
    }
    this->unlock();

    return status;
}

template <class MQTT>
inline
bool ERaMqttLinux<MQTT>::unsubscribeTopic(const char* baseTopic, const char* topic) {
    bool status {false};
    char topicName[MAX_TOPIC_LENGTH] {0};
    if (baseTopic != NULL) {
        FormatString(topicName, baseTopic);
    }
    if (topic != NULL) {
        FormatString(topicName, topic);
    }

    this->lock();
    if (this->connected()) {
        status = this->mqtt.unsubscribe(topicName);
        ERA_MQTT_UNSUB_LOG(status, this->mqtt.lastError())
    }
    this->unlock();

    return status;
}

template <class MQTT>
inline
bool ERaMqttLinux<MQTT>::publishData(const char* topic, const char* payload,
                                    bool retained, QoST qos) {
    if (!this->_connected) {
        return false;
    }

    bool status {false};

    this->lock();
    if (this->connected()) {
        this->lastPublish = ERaMillis();
        status = this->mqtt.publish(topic, payload, retained, qos);
        this->ping = (ERaMillis() - this->lastPublish);
        ERA_MQTT_PUB_LOG(status, this->mqtt.lastError())
    }
    this->unlock();

    return status;
}

template <class MQTT>
inline
bool ERaMqttLinux<MQTT>::publishState(bool online) {
    this->needPubState = online;
    return true;
}

template <class MQTT>
inline
bool ERaMqttLinux<MQTT>::syncConfig() {
    return this->publishLWT(true, false);
}

template <class MQTT>
inline
void ERaMqttLinux<MQTT>::delays(MillisTime_t ms) {
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

template <class MQTT>
inline
bool ERaMqttLinux<MQTT>::publishLWT(bool sync) {
    return this->publishLWT(sync, LWT_RETAINED);
}

template <class MQTT>
inline
bool ERaMqttLinux<MQTT>::publishLWT(bool sync, bool retained) {
    return this->publishLWT(sync, retained, LWT_QOS);
}

template <class MQTT>
inline
bool ERaMqttLinux<MQTT>::publishLWT(bool sync, bool retained, QoST qos) {
    bool status {false};
    char wifiInfo[50] {0};
    char payload[100] {0};
    char* topic = this->willTopic;

    if (this->getSSID() != NULL) {
        FormatString(wifiInfo, WIFI_INFO, this->getSSID());
    }

    FormatString(payload, ONLINE_MESSAGE, wifiInfo, sync ? ASK_CONFIG_INFO : "");

    this->lock();
    if (this->connected()) {
        this->lastPublish = ERaMillis();
        status = this->mqtt.publish(topic, payload, LWT_RETAINED, LWT_QOS);
        this->ping = (ERaMillis() - this->lastPublish);
        ERA_MQTT_PUB_LOG(status, this->mqtt.lastError())
    }
    this->unlock();

    return status;
}

template <class MQTT>
inline
void ERaMqttLinux<MQTT>::onConnected() {
    if (this->_connected) {
        return;
    }
    this->_connected = true;
    if (this->connectedCb == NULL) {
        return;
    }
    this->connectedCb();
}

template <class MQTT>
inline
void ERaMqttLinux<MQTT>::onDisconnected() {
    if (!this->_connected) {
        return;
    }
    this->_connected = false;
    if (this->disconnectedCb == NULL) {
        return;
    }
    this->disconnectedCb();
}

template <class MQTT>
inline
void ERaMqttLinux<MQTT>::lock() {
    ERaGuardLock(this->mutex);
}

template <class MQTT>
inline
void ERaMqttLinux<MQTT>::unlock() {
    ERaGuardUnlock(this->mutex);
}

#define ERaMqtt ERaMqttLinux

#endif /* INC_ERA_MQTT_LINUX_HPP_ */
