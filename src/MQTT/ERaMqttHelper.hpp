#ifndef INC_ERA_MQTT_HELPER_HPP_
#define INC_ERA_MQTT_HELPER_HPP_

class ERaMqttHelper
{
protected:
    enum QoST {
        QOS0 = 0x00,
        QOS1 = 0x01,
        QOS2 = 0x02,
        SUBFAIL = 0x80
    };

public:
    ERaMqttHelper()
    {}
    virtual ~ERaMqttHelper()
    {}

    virtual bool subscribeTopic(const char* baseTopic, const char* topic,
                            QoST qos = (QoST)ERA_MQTT_SUBSCRIBE_QOS) = 0;
    virtual bool unsubscribeTopic(const char* baseTopic, const char* topic) = 0;
    virtual bool publishData(const char* topic, const char* payload,
                            bool retained = ERA_MQTT_PUBLISH_RETAINED,
                            QoST qos = (QoST)ERA_MQTT_PUBLISH_QOS) = 0;

private:
};

#endif /* INC_ERA_MQTT_HELPER_HPP_ */
