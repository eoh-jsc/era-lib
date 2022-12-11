#ifndef INC_ERA_MQTT_SECURE_HPP_
#define INC_ERA_MQTT_SECURE_HPP_

#include <MQTT/ERaMqtt.hpp>

template <class Client, class MQTT>
class ERaMqttSecure
    : public ERaMqtt<Client, MQTT>
{
    typedef ERaMqtt<Client, MQTT> Base;

public:
    ERaMqttSecure(Client& _client)
        : Base(_client)
    {
#if defined(ESP32)
        _client.setHandshakeTimeout(30);
#elif defined(ESP8266)
        _client.setBufferSizes(ERA_MQTT_BUFFER_SIZE, ERA_MQTT_BUFFER_SIZE / 2);
#endif
        _client.setInsecure();
    }
};

#endif /* INC_ERA_MQTT_SECURE_HPP_ */
