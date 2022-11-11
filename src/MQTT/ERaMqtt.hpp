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
        SUBFAIL=0x80
    };
    typedef std::function<void(std::string&, std::string&)> MessageCallback;

    const char* TAG = "MQTT";
    const char* ONLINE_MESSAGE = R"json({"ol":1})json";
    const char* OFFLINE_MESSAGE = R"json({"ol":0})json";
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
    void run();
    bool publishData(const char* topic, const char* payload);

    void setTopic(const char* topic) {
        this->eraTopic = topic;
    }

    void setAuth(const char* auth) {
        this->eraAuth = auth;
    }

    void onMessage(MessageCallback cb) {
        this->mqtt.onMessage(cb);
    }

protected:
private:
    bool subscribeTopic(const char* baseTopic, const char* topic, QoST qos);
    bool publishLWT();

    Client* client;
    MQTT mqtt{ERA_MQTT_BUFFER_SIZE};
    const char* host;
    uint16_t port;
    const char* username;
    const char* password;
    const char* eraTopic;
    const char* eraAuth;
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
    FormatString(this->willTopic, this->eraTopic);
    FormatString(this->willTopic, LWT_TOPIC);
    this->mqtt.setKeepAlive(60);
    this->mqtt.setWill(this->willTopic, OFFLINE_MESSAGE, LWT_RETAINED, LWT_QOS);
    this->mqtt.begin(this->host, this->port, *this->client);
}

template <class Client, class MQTT>
bool ERaMqtt<Client, MQTT>::connect() {
    size_t count {0};
    this->mqtt.disconnect();
    while (this->mqtt.connect(this->eraAuth, this->username, this->password) == false) {
        ERA_LOG(TAG, "MQTT: connect failed, retrying in 5 seconds");
        ERaDelay(5000);
        if (++count >= LIMIT_CONNECT_BROKER_MQTT) {
            return false;
        }
    }

    subscribeTopic(this->eraTopic, "/arduino_pin/+", QoST::QOS0);
    subscribeTopic(this->eraTopic, "/pin/down", QoST::QOS0);
    subscribeTopic(this->eraTopic, "/down", QoST::QOS0);

    this->publishLWT();

    return true;
}

template <class Client, class MQTT>
void ERaMqtt<Client, MQTT>::run() {
    if (!this->mqtt.loop()) {
        eraOnDisconnected();
        this->connect();
    }
}

template <class Client, class MQTT>
bool ERaMqtt<Client, MQTT>::subscribeTopic(const char* baseTopic, const char* topic, QoST qos) {
    bool status {false};
    char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, baseTopic);
	FormatString(topicName, topic);

    ERaGuardLock(this->mutex);
    if (this->mqtt.connected()) {
        ERA_LOG(TAG, "Subscribe: %s, QoS: %d", topicName, qos);
        status = this->mqtt.subscribe(topicName, qos);
    }
    ERaGuardUnlock(this->mutex);

    return status;
}

template <class Client, class MQTT>
bool ERaMqtt<Client, MQTT>::publishData(const char* topic, const char* payload) {
    bool status {false};

    ERaGuardLock(this->mutex);
    if (this->mqtt.connected()) {
        ERA_LOG(TAG, "Publish %s: %s", topic, payload);
        status = this->mqtt.publish(topic, payload);
    }
    ERaGuardUnlock(this->mutex);

    return status;
}

template <class Client, class MQTT>
bool ERaMqtt<Client, MQTT>::publishLWT() {
    bool status {false};

    ERaGuardLock(this->mutex);
    if (this->mqtt.connected()) {
        ERA_LOG(TAG, "Publish %s: %s", this->willTopic, ONLINE_MESSAGE);
        status = this->mqtt.publish(this->willTopic, ONLINE_MESSAGE, LWT_RETAINED, LWT_QOS);
    }
    ERaGuardUnlock(this->mutex);

    return status;
}

#endif /* INC_ERA_MQTT_HPP_ */
