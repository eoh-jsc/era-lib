#ifndef INC_ERA_MQTT_HPP_
#define INC_ERA_MQTT_HPP_

#include <string>
#include <Utility/ERaUtility.hpp>
#include <Utility/ERacJSON.hpp>
#include <ERa/ERaDefine.hpp>
#include <ERa/ERaConfig.hpp>
#include <ERa/ERaApi.hpp>
#include "MQTT/MQTT.h"

using namespace std;

template <class Client, class MQTT>
class ERaMqtt
{
    enum QoST {
        QOS0 = 0x00,
        QOS1 = 0x01,
        QOS2 = 0x02,
        SUBFAIL = 0x80
    };
    typedef std::function<void(std::string&, std::string&)> MessageCallback;

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
        , host(ERA_MQTT_HOST)
        , port(ERA_MQTT_PORT)
        , username(ERA_MQTT_USERNAME)
        , password(ERA_MQTT_PASSWORD)
        , ssid(NULL)
        , ping(0L)
        , askConfig(false)
        , mutex(NULL)
    {
        memset(this->willTopic, 0, sizeof(this->willTopic));
    }
    ERaMqtt(Client& _client)
        : client(NULL)
        , host(ERA_MQTT_HOST)
        , port(ERA_MQTT_PORT)
        , username(ERA_MQTT_USERNAME)
        , password(ERA_MQTT_PASSWORD)
        , ssid(NULL)
        , ping(0L)
        , askConfig(false)
        , mutex(NULL)
    {
        this->setClient(&_client);
        memset(this->willTopic, 0, sizeof(this->willTopic));
    }
    ~ERaMqtt()
    {}

    void setClient(Client* _client) {
        this->client = _client;
    }

    void config(const char* _host, uint16_t _port, const char* _user, const char* _password);
    bool connect();
    bool run();
    bool publishData(const char* topic, const char* payload,
                    QoST qos = (QoST)ERA_MQTT_PUBLISH_QOS, bool retained = ERA_MQTT_PUBLISH_RETAINED);
    bool syncConfig();

    void setTimeout(uint32_t timeout) {
        this->mqtt.setTimeout(timeout);
    }

    void setTopic(const char* topic) {
        this->ERaTopic = topic;
    }

    void setAuth(const char* auth) {
        this->ERaAuth = auth;
    }

    void setSSID(const char* _ssid) {
        this->ssid = _ssid;
    }

    const char* getSSID() {
        return this->ssid;
    }

    MillisTime_t getPing() {
        return this->ping;
    }

    void setAskConfig(bool enable) {
        this->askConfig = enable;
    }

    bool getAskConfig() {
#if defined(ERA_ASK_CONFIG_WHEN_RESTART)
        return true;
#else
        return this->askConfig;
#endif
    }

    const char* getTag() {
        return this->TAG;
    }

    void onMessage(MessageCallback cb) {
        this->mqtt.onMessage(cb);
    }

protected:
private:
    bool subscribeTopic(const char* baseTopic, const char* topic, QoST qos);
    bool publishLWT(bool sync = false);

    Client* client;
    MQTT mqtt{ERA_MQTT_BUFFER_SIZE};
    const char* host;
    uint16_t port;
    const char* username;
    const char* password;
    const char* ERaTopic;
    const char* ERaAuth;
    const char* ssid;
    MillisTime_t ping;
    bool askConfig;
    char willTopic[MAX_TOPIC_LENGTH];
    ERaMutex_t mutex;
};

template <class Client, class MQTT>
void ERaMqtt<Client, MQTT>::config(const char* _host, uint16_t _port, const char* _user, const char* _password) {
    this->host = _host;
    this->port = _port;
    this->username = _user;
    this->password = _password;
    ClearArray(this->willTopic);
    FormatString(this->willTopic, this->ERaTopic);
    FormatString(this->willTopic, LWT_TOPIC);
    this->mqtt.setKeepAlive(ERA_MQTT_KEEP_ALIVE);
    this->mqtt.setWill(this->willTopic, OFFLINE_MESSAGE, LWT_RETAINED, LWT_QOS);
    this->mqtt.begin(this->host, this->port, *this->client);
}

template <class Client, class MQTT>
bool ERaMqtt<Client, MQTT>::connect() {
    size_t count {0};
    this->mqtt.disconnect();
    while (this->mqtt.connect(this->ERaAuth, this->username, this->password) == false) {
        ERA_LOG(TAG, ERA_PSTR("MQTT: connect failed, retrying in 5 seconds"));
        ERaDelay(5000);
        if (++count >= LIMIT_CONNECT_BROKER_MQTT) {
            return false;
        }
    }

#if defined(ERA_ZIGBEE)
    subscribeTopic(this->ERaTopic, "/zigbee/+/down", QoST::QOS0);
    subscribeTopic(this->ERaTopic, "/zigbee/permit_to_join", QoST::QOS0);
    subscribeTopic(this->ERaTopic, "/zigbee/remove_device", QoST::QOS0);
#endif

    subscribeTopic(this->ERaTopic, "/arduino_pin/+", QoST::QOS0);
    subscribeTopic(this->ERaTopic, "/virtual_pin/+", QoST::QOS0);
    subscribeTopic(this->ERaTopic, "/pin/down", QoST::QOS0);
    subscribeTopic(this->ERaTopic, "/down", QoST::QOS0);

    if (!this->publishLWT(this->getAskConfig())) {
        return false;
    }
    ERaOnConnected();
    return true;
}

template <class Client, class MQTT>
bool ERaMqtt<Client, MQTT>::run() {
    if (!this->mqtt.loop()) {
        ERaOnDisconnected();
        return this->connect();
    }
    return true;
}

template <class Client, class MQTT>
bool ERaMqtt<Client, MQTT>::subscribeTopic(const char* baseTopic, const char* topic, QoST qos) {
    bool status {false};
    char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, baseTopic);
	FormatString(topicName, topic);

    ERaGuardLock(this->mutex);
    if (this->mqtt.connected()) {
        ERA_LOG(TAG, ERA_PSTR("Subscribe: %s, QoS: %d"), topicName, qos);
        status = this->mqtt.subscribe(topicName, qos);
    }
    ERaGuardUnlock(this->mutex);

    return status;
}

template <class Client, class MQTT>
bool ERaMqtt<Client, MQTT>::publishData(const char* topic, const char* payload,
                                        QoST qos, bool retained) {
    bool status {false};

    ERaGuardLock(this->mutex);
    if (this->mqtt.connected()) {
        ERA_LOG(TAG, ERA_PSTR("Publish %s: %s"), topic, payload);
        status = this->mqtt.publish(topic, payload, retained, qos);
    }
    ERaGuardUnlock(this->mutex);

    return status;
}

template <class Client, class MQTT>
bool ERaMqtt<Client, MQTT>::syncConfig() {
    return this->publishLWT(true);
}

template <class Client, class MQTT>
bool ERaMqtt<Client, MQTT>::publishLWT(bool sync) {
    bool status {false};
    char wifiInfo[50] {0};
    char payload[100] {0};

    if (this->getSSID() != nullptr) {
        FormatString(wifiInfo, WIFI_INFO, this->getSSID());
    }

    FormatString(payload, ONLINE_MESSAGE, wifiInfo, sync ? ASK_CONFIG_INFO : "");

    ERaGuardLock(this->mutex);
    if (this->mqtt.connected()) {
        ERA_LOG(TAG, ERA_PSTR("Publish %s: %s"), this->willTopic, payload);
        this->ping = ERaMillis();
        status = this->mqtt.publish(this->willTopic, payload, LWT_RETAINED, LWT_QOS);
        this->ping = ERaMillis() - this->ping;
    }
    ERaGuardUnlock(this->mutex);

    return status;
}

#endif /* INC_ERA_MQTT_HPP_ */
