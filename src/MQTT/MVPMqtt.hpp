#ifndef INC_MVP_MQTT_HPP_
#define INC_MVP_MQTT_HPP_

#include <string>
#include <Utility/MVPUtility.hpp>
#include <Utility/MVPcJSON.hpp>
#include <MVP/MVPDefine.hpp>
#include <MVP/MVPApi.hpp>
#include <MVP/MVPConfig.hpp>
#include "MQTT/MQTT.h"

using namespace std;

template <class Client, class MQTT>
class MVPMqtt
{
    enum QoST {
        QOS0 = 0x00,
        QOS1 = 0x01,
        QOS2 = 0x02,
        SUBFAIL=0x80
    };
    typedef std::function<void(String&, String&)> MessageCallback;

    const char* TAG = "MQTT";
    const char* ONLINE_MESSAGE = R"json({"ol":1})json";
    const char* OFFLINE_MESSAGE = R"json({"ol":0})json";
    const char* LWT_TOPIC = "/is_online";
    const bool LWT_RETAINED = true;
    const int LWT_QOS = QoST::QOS1;

public:
    MVPMqtt()
        : client(NULL)
        , host(MVP_MQTT_HOST)
        , port(MVP_MQTT_PORT)
        , username(MVP_MQTT_USERNAME)
        , password(MVP_MQTT_PASSWORD)
        , bufferSize(MVP_BUFFER_SIZE)
        , mutex(NULL)
    {
        memset(this->willTopic, 0, sizeof(this->willTopic));
    }
    MVPMqtt(Client& _client)
        : client(NULL)
        , host(MVP_MQTT_HOST)
        , port(MVP_MQTT_PORT)
        , username(MVP_MQTT_USERNAME)
        , password(MVP_MQTT_PASSWORD)
        , bufferSize(MVP_BUFFER_SIZE)
        , mutex(NULL)
    {
        this->setClient(&_client);
        memset(this->willTopic, 0, sizeof(this->willTopic));
    }
    ~MVPMqtt()
    {}

    void setClient(Client* _client) {
        this->client = _client;
    }

    void config(const char* _host, uint16_t _port, const char* _user, const char* _password);
    bool connect();
    void run();
    bool publishData(const char* topic, const char* payload);

    void setBufferSize(int size) {
        this->bufferSize = size;
    }

    void setTopic(const char* topic) {
        this->mvpTopic = topic;
    }

    void setAuth(const char* auth) {
        this->mvpAuth = auth;
    }

    void onMessage(MessageCallback cb) {
        this->mqtt.onMessage(cb);
    }

protected:
private:
    bool subscribeTopic(const char* baseTopic, const char* topic, QoST qos);

    Client* client;
    MQTT mqtt{MVP_BUFFER_SIZE};
    const char* host;
    uint16_t port;
    const char* username;
    const char* password;
    const char* mvpTopic;
    const char* mvpAuth;
    int bufferSize;
    char willTopic[MAX_TOPIC_LENGTH];
    MVPMutex_t mutex;
};

template <class Client, class MQTT>
void MVPMqtt<Client, MQTT>::config(const char* _host, uint16_t _port, const char* _user, const char* _password) {
    this->host = _host;
    this->port = _port;
    this->username = _user;
    this->password = _password;
    ClearArray(this->willTopic);
    FormatString(this->willTopic, this->mvpTopic);
    FormatString(this->willTopic, LWT_TOPIC);
    this->mqtt.setKeepAlive(60);
    this->mqtt.setWill(this->willTopic, OFFLINE_MESSAGE, LWT_RETAINED, LWT_QOS);
    this->mqtt.begin(this->host, this->port, *this->client);
}

template <class Client, class MQTT>
bool MVPMqtt<Client, MQTT>::connect() {
    size_t count {0};
    this->mqtt.disconnect();
    while (this->mqtt.connect(this->mvpAuth, this->username, this->password) == false) {
        MVP_LOG(TAG, "MQTT: connect failed, retrying in 5 seconds");
        MVPDelay(5000);
        if (++count >= LIMIT_CONNECT_BROKER_MQTT) {
            return false;
        }
    }

    subscribeTopic(this->mvpTopic, "/arduino_pin/+", QoST::QOS0);
    subscribeTopic(this->mvpTopic, "/pin/down", QoST::QOS0);
    subscribeTopic(this->mvpTopic, "/down", QoST::QOS0);

    this->mqtt.publish(this->willTopic, ONLINE_MESSAGE, LWT_RETAINED, LWT_QOS);

    return true;
}

template <class Client, class MQTT>
void MVPMqtt<Client, MQTT>::run() {
    if (!this->mqtt.loop()) {
        mvpOnDisconnected();
        this->connect();
    }
}

template <class Client, class MQTT>
bool MVPMqtt<Client, MQTT>::subscribeTopic(const char* baseTopic, const char* topic, QoST qos) {
    char topicName[MAX_TOPIC_LENGTH] {0};
	FormatString(topicName, baseTopic);
	FormatString(topicName, topic);
    return this->mqtt.subscribe(topicName, qos);
}

template <class Client, class MQTT>
bool MVPMqtt<Client, MQTT>::publishData(const char* topic, const char* payload) {
    bool status {false};

    MVPGuardLock(this->mutex);
    if (this->mqtt.connected()) {
        MVP_LOG(TAG, "Publish %s: %s", topic, payload);
        status = this->mqtt.publish(topic, payload);
    }
    MVPGuardUnlock(this->mutex);

    return status;
}

#endif /* INC_MVP_MQTT_HPP_ */