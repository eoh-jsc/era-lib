#ifndef INC_ERA_SERIAL_CLIENT_HPP_
#define INC_ERA_SERIAL_CLIENT_HPP_

#if !defined(ERA_PROTO_TYPE)
    #define ERA_PROTO_TYPE            "Stream"
#endif

#include <ERa/ERaApiArduinoDef.hpp>
#include <ERa/ERaProtocol.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <MQTT/ERaMqtt.hpp>

class ERaFlash;

class ERaClient : public Client
{
public:
    ERaClient()
        : stream(NULL)
        , timeout(1000L)
        , _connected(false)
    {}
    ~ERaClient()
    {}

    void begin(Stream& _stream) {
        this->stream = &_stream;
    }

    int connect(IPAddress ip, uint16_t port) override {
        this->stream->flush();
        return this->_connected = true;
    }

    int connect(const char* host, uint16_t port) override {
        this->stream->flush();
        return this->_connected = true;
    }

    void setTimeout(unsigned long _timeout) {
        this->timeout = _timeout;
    }

    size_t write(uint8_t value) override {
        this->stream->write(value);
        return 1;
    }

    size_t write(const uint8_t *buf, size_t size) override {
        this->stream->write(buf, size);
        return size;
    }

    int available() override {
        return this->stream->available();
    }

    int read() override {
        return this->stream->read();
    }

    int read(uint8_t *buf, size_t size) override {
        uint8_t* begin = buf;
        uint8_t* end = buf + size;
        MillisTime_t startMillis = ERaMillis();
        while ((begin < end) && (ERaMillis() - startMillis < this->timeout)) {
            int c = this->stream->read();
            if (c < 0) {
                continue;
            }
            *begin++ = (uint8_t)c;
        }
        return begin - buf;
    }

    int peek() override {
        return this->stream->peek();
    }

    void flush() override {
        this->stream->flush();
    }

    void stop() override {
        this->_connected = false;
    }

    uint8_t connected() override {
        return this->_connected;
    }

    operator bool() override {
        return this->connected();
    }

protected:
private:
    Stream* stream;
    unsigned long timeout;
    bool _connected;
};

template <class Transport>
class ERaStream
    : public ERaProto<Transport, ERaFlash>
{
    const char* TAG = "Serial";
    friend class ERaProto<Transport, ERaFlash>;
    typedef ERaProto<Transport, ERaFlash> Base;

public:
    ERaStream(Transport& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , authToken(nullptr)
    {}
    ~ERaStream()
    {}

    void config(Stream& stream,
                const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::begin(auth);
        this->client.begin(stream);
        this->transp.setClient(&this->client);
        this->transp.config(host, port, username, password);
    }

    void begin(const char* auth,
                Stream& stream,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::init();
        this->config(stream, auth, host, port, username, password);
        Base::connect();
    }

    void begin(Stream& stream) {
        this->begin(ERA_MQTT_CLIENT_ID, stream,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

protected:
private:
    ERaClient client;
    const char* authToken;
};

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().ERA_AUTH);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, ERA_PROTO_TYPE);
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_PROTO_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddStringToObject(root, INFO_MAC, ERA_PROTO_TYPE);
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ERA_PROTO_TYPE);
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().transp.getPing());
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ERA_PROTO_TYPE);
}

#endif /* INC_ERA_SERIAL_CLIENT_HPP_ */
