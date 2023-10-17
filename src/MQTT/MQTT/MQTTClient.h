#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

// include functional API if possible. remove min and max macros for some
// platforms as they will be defined again by Arduino later
#if defined(ESP8266) || defined(ESP32)
  #include <string>
  #include <functional>
  #define MQTT_HAS_STRING       1
  #define MQTT_HAS_FUNCTIONAL   1
#elif defined(ARDUINO_ARCH_NRF5) ||   \
  defined(RTL8722DM) || defined(ARDUINO_AMEBA)
  #define MQTT_HAS_STRING       0
  #define MQTT_HAS_FUNCTIONAL   0
#elif defined(__has_include)
  #if __has_include(<string>)
    #include <string>
    #define MQTT_HAS_STRING     1
  #else
    #define MQTT_HAS_STRING     0
  #endif
  #if __has_include(<functional>)
    #if defined(min)
      #undef min
    #endif
    #if defined(max)
      #undef max
    #endif
    #include <functional>
    #define MQTT_HAS_FUNCTIONAL 1
  #else
    #define MQTT_HAS_FUNCTIONAL 0
  #endif
#else
  #define MQTT_HAS_STRING       0
  #define MQTT_HAS_FUNCTIONAL   0
#endif

#if defined(PARTICLE) || defined(SPARK)
  #include "application.h"
  #undef retained
#elif defined(ARDUINO) && \
  defined(ARDUINO_ARCH_ARM)
  #include <Arduino.h>
#else
  #include <Arduino.h>
  #include <Client.h>
  #include <Stream.h>
#endif

extern "C" {
  #include "lwmqtt/lwmqtt.h"
}

#if defined(ESP8266) && defined(ARDUINO_ESP8266_MAJOR) &&           \
    defined(ARDUINO_ESP8266_MINOR) && defined(ARDUINO_ESP8266_REVISION)
  #define ESP8266_VERSION_NUMBER ((ARDUINO_ESP8266_MAJOR * 10000) + \
                                  (ARDUINO_ESP8266_MINOR * 100) +   \
                                  (ARDUINO_ESP8266_REVISION))
  #if (ESP8266_VERSION_NUMBER >= 30101)
    #include <coredecls.h>
    #define mqtt_yield_fix() esp_yield()
  #else
    #define mqtt_yield_fix() delay(1)
  #endif
#elif defined (PARTICLE) || defined(SPARK)
  #define mqtt_yield_fix()   Particle.process()
#else
  #define mqtt_yield_fix()   delay(1)
#endif

typedef uint32_t (*MQTTClientClockSource)();

typedef struct {
  uint32_t start;
  uint32_t timeout;
  MQTTClientClockSource millis;
} lwmqtt_arduino_timer_t;

typedef struct {
  Client *client;
} lwmqtt_arduino_network_t;

class MQTTClient;

typedef void (*MQTTClientCallbackSimple)(const char *topic, const char *payload);
typedef void (*MQTTClientCallbackAdvanced)(MQTTClient *client, char topic[], char bytes[], int length);
#if MQTT_HAS_FUNCTIONAL
typedef std::function<void(const char *topic, const char *payload)> MQTTClientCallbackSimpleFunction;
typedef std::function<void(MQTTClient *client, char topic[], char bytes[], int length)>
    MQTTClientCallbackAdvancedFunction;
#endif

typedef struct {
  MQTTClient *client = nullptr;
  MQTTClientCallbackSimple simple = nullptr;
  MQTTClientCallbackAdvanced advanced = nullptr;
#if MQTT_HAS_FUNCTIONAL
  MQTTClientCallbackSimpleFunction functionSimple = nullptr;
  MQTTClientCallbackAdvancedFunction functionAdvanced = nullptr;
#endif
} MQTTClientCallback;

class MQTTClient {
 private:
  size_t readBufSize = 0;
  size_t writeBufSize = 0;
  uint8_t *readBuf = nullptr;
  uint8_t *writeBuf = nullptr;

  uint16_t keepAlive = 60;
  bool cleanSession = true;
  uint32_t timeout = 2000;
  bool _sessionPresent = false;
  bool skipACK = true;

  Client *netClient = nullptr;
  const char *hostname = nullptr;
  IPAddress address;
  int port = 0;
  lwmqtt_will_t *will = nullptr;
  MQTTClientCallback callback;

  lwmqtt_arduino_network_t network = {nullptr};
  lwmqtt_arduino_timer_t timer1 = {0, 0, nullptr};
  lwmqtt_arduino_timer_t timer2 = {0, 0, nullptr};
  lwmqtt_client_t client = lwmqtt_client_t();

  bool _connected = false;
  uint16_t nextDupPacketID = 0;
  lwmqtt_return_code_t _returnCode = (lwmqtt_return_code_t)0;
  lwmqtt_err_t _lastError = (lwmqtt_err_t)0;
  uint32_t _droppedMessages = 0;

 public:
  void *ref = nullptr;

  explicit MQTTClient(int bufSize = 128) : MQTTClient(bufSize, bufSize) {}
  explicit MQTTClient(int readBufSize, int writeBufSize);

  ~MQTTClient();

  void begin(Client &_client);
  void begin(const char _hostname[], Client &_client) { this->begin(_hostname, 1883, _client); }
  void begin(const char _hostname[], int _port, Client &_client) {
    this->begin(_client);
    this->setHost(_hostname, _port);
  }
  void begin(IPAddress _address, Client &_client) { this->begin(_address, 1883, _client); }
  void begin(IPAddress _address, int _port, Client &_client) {
    this->begin(_client);
    this->setHost(_address, _port);
  }

  void init(int readBufSize, int writeBufSize);

  void onMessage(MQTTClientCallbackSimple cb);
  void onMessageAdvanced(MQTTClientCallbackAdvanced cb);
#if MQTT_HAS_FUNCTIONAL
  void onMessage(MQTTClientCallbackSimpleFunction cb);
  void onMessageAdvanced(MQTTClientCallbackAdvancedFunction cb);
#endif

  void setClient(Client* _client) { this->netClient = _client; }

  void setClockSource(MQTTClientClockSource cb);

  void setSkipACK(bool skip);
  bool getSkipACK() const {
    return this->skipACK;
  }

  void setHost(const char _hostname[]) { this->setHost(_hostname, 1883); }
  void setHost(const char hostname[], int port);
  void setHost(IPAddress _address) { this->setHost(_address, 1883); }
  void setHost(IPAddress _address, int port);

  void setWill(const char topic[]) { this->setWill(topic, ""); }
  void setWill(const char topic[], const char payload[]) { this->setWill(topic, payload, false, 0); }
  void setWill(const char topic[], const char payload[], bool retained, int qos);
  void clearWill();

  void setKeepAlive(int keepAlive);
  void setCleanSession(bool cleanSession);
  void setTimeout(int timeout);

  void setOptions(int _keepAlive, bool _cleanSession, int _timeout) {
    this->setKeepAlive(_keepAlive);
    this->setCleanSession(_cleanSession);
    this->setTimeout(_timeout);
  }

  void dropOverflow(bool enabled);
  uint32_t droppedMessages() { return this->_droppedMessages; }

  bool connect(const char clientId[], bool skip = false) { return this->connect(clientId, nullptr, nullptr, skip); }
  bool connect(const char clientId[], const char username[], bool skip = false) {
    return this->connect(clientId, username, nullptr, skip);
  }
  bool connect(const char clientID[], const char username[], const char password[], bool skip = false);

#if MQTT_HAS_STRING
  bool publish(const std::string &topic) { return this->publish(topic.c_str(), ""); }
  bool publish(const std::string &topic, const std::string &payload) { return this->publish(topic.c_str(), payload.c_str()); }
  bool publish(const std::string &topic, const std::string &payload, bool retained, int qos) {
    return this->publish(topic.c_str(), payload.c_str(), retained, qos);
  }
  bool publish(const char topic[], const std::string &payload) { return this->publish(topic, payload.c_str()); }
  bool publish(const char topic[], const std::string &payload, bool retained, int qos) {
    return this->publish(topic, payload.c_str(), retained, qos);
  }
#endif
  bool publish(const char topic[]) { return this->publish(topic, ""); }
  bool publish(const char topic[], const char payload[]) {
    return this->publish(topic, (char *)payload, (int)strlen(payload));
  }
  bool publish(const char topic[], const char payload[], bool retained, int qos) {
    return this->publish(topic, (char *)payload, (int)strlen(payload), retained, qos);
  }
  bool publish(const char topic[], const char payload[], int length) {
    return this->publish(topic, payload, length, false, 0);
  }
  bool publish(const char topic[], const char payload[], int length, bool retained, int qos);

  uint16_t lastPacketID();
  void prepareDuplicate(uint16_t packetID);

#if MQTT_HAS_STRING
  bool subscribe(const std::string &topic) { return this->subscribe(topic.c_str()); }
  bool subscribe(const std::string &topic, int qos) { return this->subscribe(topic.c_str(), qos); }
#endif
  bool subscribe(const char topic[]) { return this->subscribe(topic, 0); }
  bool subscribe(const char topic[], int qos);

#if MQTT_HAS_STRING
  bool unsubscribe(const std::string &topic) { return this->unsubscribe(topic.c_str()); }
#endif
  bool unsubscribe(const char topic[]);

  bool loop();
  bool connected();
  bool sessionPresent() { return this->_sessionPresent; }

  lwmqtt_err_t lastError() { return this->_lastError; }
  lwmqtt_return_code_t returnCode() { return this->_returnCode; }

  bool disconnect();

 private:
  void close();
};

#endif /* MQTT_CLIENT_H */
