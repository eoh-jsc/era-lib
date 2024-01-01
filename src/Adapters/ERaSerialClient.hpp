#ifndef INC_ERA_SERIAL_CLIENT_HPP_
#define INC_ERA_SERIAL_CLIENT_HPP_

#if !defined(ERA_NETWORK_TYPE)
    #define ERA_NETWORK_TYPE          "Stream"
#endif

#if !defined(ERA_AUTH_TOKEN)
    #error "Please specify your ERA_AUTH_TOKEN"
#endif

#include <Client.hpp>
#include <ERa/ERaProtocol.hpp>
#include <MQTT/ERaMqtt.hpp>

class ERaFlash;

class ERaClient
    : public Client
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
        ERA_FORCE_UNUSED(ip);
        ERA_FORCE_UNUSED(port);
        this->stream->flush();
        this->_connected = true;
        return 1;
    }

    int connect(const char* host, uint16_t port) override {
        ERA_FORCE_UNUSED(host);
        ERA_FORCE_UNUSED(port);
        this->stream->flush();
        this->_connected = true;
        return 1;
    }

    void setTimeout(unsigned long _timeout) {
        this->timeout = _timeout;
    }

    size_t write(uint8_t value) override {
        this->stream->write(value);
        return 1;
    }

    size_t write(const uint8_t* buf, size_t size) override {
        this->stream->write(buf, size);
        return size;
    }

    int available() override {
        return this->stream->available();
    }

    int read() override {
        return this->stream->read();
    }

    int read(uint8_t* buf, size_t size) override {
        uint8_t* begin = buf;
        uint8_t* end = buf + size;
        MillisTime_t startMillis = ERaMillis();
        while ((begin < end) && ((ERaMillis() - startMillis) < this->timeout)) {
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
        return this->_connected;
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
        this->getTransp().setClient(&this->client);
        this->getTransp().config(host, port, username, password);
    }

    void begin(const char* auth,
                Stream& stream,
                const char* host,
                uint16_t port,
                const char* username,
                const char* password) {
        Base::init();
        this->config(stream, auth, host, port, username, password);
        this->getTransp().setSSID(ERA_NETWORK_TYPE);
    }

    void begin(const char* auth,
                Stream& stream,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT) {
        this->begin(auth, stream, host,
                    port, auth, auth);
    }

    void begin(Stream& stream) {
        this->begin(ERA_AUTHENTICATION_TOKEN, stream,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

    void run() {
        switch (ERaState::get()) {
            case StateT::STATE_CONNECTING_CLOUD:
                if (Base::connect()) {
                    ERaOptConnected(this);
                    ERaState::set(StateT::STATE_CONNECTED);
                }
                else {
                    ERaState::set(StateT::STATE_CONNECTING_NETWORK);
                }
                break;
            case StateT::STATE_CONNECTED:
                ERaState::set(StateT::STATE_RUNNING);
                break;
            case StateT::STATE_RUNNING:
                Base::run();
                break;
            default:
                ERaState::set(StateT::STATE_CONNECTING_CLOUD);
                break;
        }
    }

protected:
private:
    ERaClient client;
    const char* authToken;
};

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
    cJSON_AddStringToObject(root, INFO_BOARD_ID, this->thisProto().getBoardID());
    cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().getAuth());
    cJSON_AddStringToObject(root, INFO_BUILD_DATE, BUILD_DATE_TIME);
    cJSON_AddStringToObject(root, INFO_VERSION, ERA_VERSION);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddNumberToObject(root, INFO_PLUG_AND_PLAY, 0);
    cJSON_AddStringToObject(root, INFO_NETWORK_PROTOCOL, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_SSID, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddNumberToObject(root, INFO_SIGNAL_STRENGTH, 100);
    cJSON_AddStringToObject(root, INFO_MAC, ERA_NETWORK_TYPE);
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ERA_NETWORK_TYPE);
    cJSON_AddNumberToObject(root, INFO_SSL, ERaInfoSSL());
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().getTransp().getPing());
    cJSON_AddNumberToObject(root, INFO_FREE_RAM, ERaFreeRam());

#if defined(ERA_RESET_REASON)
    cJSON_AddStringToObject(root, INFO_RESET_REASON, SystemGetResetReason().c_str());
#endif

    /* Override info */
    ERaInfo(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
    cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
    cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
    cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
    cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
    cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
    cJSON_AddNumberToObject(root, INFO_MB_SIGNAL_STRENGTH, 100);
    cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ERA_NETWORK_TYPE);

    /* Override modbus info */
    ERaModbusInfo(root);
}

#endif /* INC_ERA_SERIAL_CLIENT_HPP_ */
