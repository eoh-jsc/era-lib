#ifndef INC_ERA_MQTT_SECURE_HPP_
#define INC_ERA_MQTT_SECURE_HPP_

#include <MQTT/ERaMqtt.hpp>

template <class ClientSecure, class MQTT>
class ERaMqttSecure
    : public ERaMqtt<Client, MQTT>
{
    typedef ERaMqtt<Client, MQTT> Base;

public:
    ERaMqttSecure(ClientSecure& clientSecure)
        : Base(clientSecure)
        , mClientSecure(clientSecure)
    {}

protected:
    void configRootCA() override {
        if (Base::rootCA != NULL) {
            this->setupRootCA();
        }
        else {
            this->setupInsecure();
        }
    }

private:
    void setupRootCA() {
#if defined(ESP32)
        this->mClientSecure.setHandshakeTimeout(30);
        this->mClientSecure.setCACert(Base::rootCA);
#elif defined(ESP8266)
        static X509List x509(Base::rootCA);
        this->mClientSecure.setBufferSizes(ERA_MQTT_RX_BUFFER_SIZE,
                                ERA_MQTT_TX_BUFFER_SIZE);
        this->mClientSecure.setTrustAnchors(&x509);
#elif defined(ARDUINO_ARCH_ARM) ||  \
      defined(ARDUINO_ARCH_OPENCPU)
        this->mClientSecure.setCACert(Base::rootCA);
#endif
    }

    void setupInsecure() {
#if defined(ESP32)
        this->mClientSecure.setHandshakeTimeout(30);
        this->mClientSecure.setInsecure();
#elif defined(ESP8266)
        this->mClientSecure.setBufferSizes(ERA_MQTT_RX_BUFFER_SIZE,
                                ERA_MQTT_TX_BUFFER_SIZE);
        this->mClientSecure.setInsecure();
#elif defined(ARDUINO_ARCH_ARM) ||  \
      defined(ARDUINO_ARCH_OPENCPU)
        this->mClientSecure.setInsecure();
#endif
    }

    ClientSecure& mClientSecure;
};

#endif /* INC_ERA_MQTT_SECURE_HPP_ */
