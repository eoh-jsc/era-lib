#ifndef INC_ERA_MQTT_HPP_
#define INC_ERA_MQTT_HPP_

#include <Utility/ERaUtility.hpp>
#include <Utility/ERacJSON.hpp>
#include <ERa/ERaDefine.hpp>
#include <ERa/ERaConfig.hpp>
#include <ERa/ERaApi.hpp>
#include "MQTT/MQTT.h"
#include "ERaMqttHelper.hpp"

#if defined(__has_include) &&       \
    __has_include(<functional>) &&  \
    !defined(ERA_IGNORE_STD_FUNCTIONAL_STRING)
    #include <functional>
    #define MQTT_HAS_FUNCTIONAL_H
#endif

#define ERA_MQTT_PUB_LOG(status, errorCode)                                                             \
    if (status) {                                                                                       \
        ERA_LOG(TAG, ERA_PSTR("Publish (ok) %s: %s"), topic, payload);                                  \
    }                                                                                                   \
    else {                                                                                              \
        ERA_LOG_ERROR(TAG, ERA_PSTR("Publish (error %d) %s: %s"), errorCode, topic, payload);           \
    }

#define ERR_MQTT_SUB_LOG(status, errorCode)                                                             \
    if (status) {                                                                                       \
        ERA_LOG(TAG, ERA_PSTR("Subscribe (ok): %s, QoS: %d"), topicName, qos);                          \
    }                                                                                                   \
    else {                                                                                              \
        ERA_LOG_ERROR(TAG, ERA_PSTR("Subscribe (error %d): %s, QoS: %d"), errorCode, topicName, qos);   \
    }

using namespace std;

template <class Client, class MQTT>
class ERaMqtt
    : public ERaMqttHelper
{
#if defined(MQTT_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> StateCallback_t;
    typedef std::function<void(const char*, const char*)> MessageCallback_t;
#else
    typedef void (*StateCallback_t)(void);
    typedef void (*MessageCallback_t)(const char*, const char*);
#endif

    const char* TAG = "MQTT";
    const char* ONLINE_MESSAGE = R"json({"ol":1%s%s})json";
    const char* OFFLINE_MESSAGE = R"json({"ol":0})json";
    const char* WIFI_INFO = R"json(,"wifi_ssid":"%s")json";
    const char* ASK_CONFIG_INFO = R"json(,"ask_configuration":1)json";
    const char* LWT_TOPIC = "/is_online";
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
        , signalQuality(0)
        , askConfig(false)
        , _connected(false)
        , mutex(NULL)
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
    bool connect();
    void disconnect();
    bool connected();
    bool run();
    bool subscribeTopic(const char* baseTopic, const char* topic,
                        QoST qos = (QoST)ERA_MQTT_SUBSCRIBE_QOS) override;
    bool publishData(const char* topic, const char* payload,
                    bool retained = ERA_MQTT_PUBLISH_RETAINED,
                    QoST qos = (QoST)ERA_MQTT_PUBLISH_QOS) override;
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

    void onMessage(MessageCallback_t cb) {
        this->mqtt.onMessage(cb);
    }

    void onStateChange(StateCallback_t onCb,
                       StateCallback_t offCb) {
        this->connectedCb = onCb;
        this->disconnectedCb = offCb;
    }

protected:
private:
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
    int16_t signalQuality;
    bool askConfig;
    bool _connected;
    char willTopic[MAX_TOPIC_LENGTH];
    ERaMutex_t mutex;
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
bool ERaMqtt<Client, MQTT>::connect() {
    size_t count {0};
    char _clientID[74] {0};
    if (this->clientID != nullptr) {
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

        ERA_LOG_ERROR(TAG, ERA_PSTR("MQTT(%d): Connect failed (%d), retrying in 5 seconds"), ++count, this->mqtt.lastError());
        ERaDelay(5000);

        ERaWatchdogFeed();

        if (count >= LIMIT_CONNECT_BROKER_MQTT) {
            return false;
        }
    }

    ERaWatchdogFeed();

#if defined(ERA_ZIGBEE) ||  \
    defined(ERA_SPECIFIC)
    subscribeTopic(this->ERaTopic, "/zigbee/+/get");
    subscribeTopic(this->ERaTopic, "/zigbee/+/down");
    subscribeTopic(this->ERaTopic, "/zigbee/permit_to_join");
    subscribeTopic(this->ERaTopic, "/zigbee/remove_device");
#endif

    subscribeTopic(this->ERaTopic, "/arduino_pin/+");
    subscribeTopic(this->ERaTopic, "/virtual_pin/+");
    subscribeTopic(this->ERaTopic, "/pin/down");
    subscribeTopic(this->ERaTopic, "/down");

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
    return true;
}

template <class Client, class MQTT>
inline
bool ERaMqtt<Client, MQTT>::subscribeTopic(const char* baseTopic, const char* topic,
                                            QoST qos) {
    bool status {false};
    char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, baseTopic);
	FormatString(topicName, topic);

    this->lockMQTT();
    if (this->connected()) {
        status = this->mqtt.subscribe(topicName, qos);
        ERR_MQTT_SUB_LOG(status, this->mqtt.lastError())
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
        status = this->mqtt.publish(topic, payload, retained, qos);
        ERA_MQTT_PUB_LOG(status, this->mqtt.lastError())
    }
    this->unlockMQTT();

    return status;
}

template <class Client, class MQTT>
inline
bool ERaMqtt<Client, MQTT>::syncConfig() {
    return this->publishLWT(true);
}

template <class Client, class MQTT>
inline
bool ERaMqtt<Client, MQTT>::publishLWT(bool sync) {
    bool status {false};
    char wifiInfo[50] {0};
    char payload[100] {0};
    char* topic = this->willTopic;

    if (this->getSSID() != nullptr) {
        FormatString(wifiInfo, WIFI_INFO, this->getSSID());
    }

    FormatString(payload, ONLINE_MESSAGE, wifiInfo, sync ? ASK_CONFIG_INFO : "");

    this->lockMQTT();
    if (this->connected()) {
        this->ping = ERaMillis();
        status = this->mqtt.publish(topic, payload, LWT_RETAINED, LWT_QOS);
        this->ping = (ERaMillis() - this->ping);
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
    if (this->connectedCb == NULL) {
        return;
    }

    this->_connected = true;
    this->connectedCb();
}

template <class Client, class MQTT>
inline
void ERaMqtt<Client, MQTT>::onDisconnected() {
    if (!this->_connected) {
        return;
    }
    if (this->disconnectedCb == NULL) {
        return;
    }

    this->_connected = false;
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
