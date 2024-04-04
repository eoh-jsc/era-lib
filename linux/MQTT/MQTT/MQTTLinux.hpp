#ifndef INC_MQTT_LINUX_HPP_
#define INC_MQTT_LINUX_HPP_

#if defined(__has_include)
  #if __has_include(<string>)
    #include <string>
    #define MQTT_HAS_STRING     1
  #else
    #define MQTT_HAS_STRING     0
  #endif
  #if __has_include(<functional>)
    #include <functional>
    #define MQTT_HAS_FUNCTIONAL 1
  #else
    #define MQTT_HAS_FUNCTIONAL 0
  #endif
#else
  #define MQTT_HAS_STRING       0
  #define MQTT_HAS_FUNCTIONAL   0
#endif

#include "posix/posix.hpp"

#if defined(ERA_MQTT_SSL)
  #include "posix/posix_tls.hpp"
#endif

#include <ERa/ERaConfig.hpp>

using namespace std;

class MQTTLinuxClient;

typedef void (*MQTTLinuxClientCallbackSimple)(const char *topic, const char *payload);
typedef void (*MQTTLinuxClientCallbackAdvanced)(MQTTLinuxClient *client, char topic[], char bytes[], int length);
#if MQTT_HAS_FUNCTIONAL
typedef std::function<void(const char *topic, const char *payload)> MQTTLinuxClientCallbackSimpleFunction;
typedef std::function<void(MQTTLinuxClient *client, char topic[], char bytes[], int length)>
    MQTTLinuxClientCallbackAdvancedFunction;
#endif

typedef struct {
  MQTTLinuxClient *client = nullptr;
  MQTTLinuxClientCallbackSimple simple = nullptr;
  MQTTLinuxClientCallbackAdvanced advanced = nullptr;
#if MQTT_HAS_FUNCTIONAL
  MQTTLinuxClientCallbackSimpleFunction functionSimple = nullptr;
  MQTTLinuxClientCallbackAdvancedFunction functionAdvanced = nullptr;
#endif
} MQTTLinuxClientCallback;

class MQTTLinuxClient {
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

  const char *hostname = nullptr;
  int port = 0;
  lwmqtt_will_t *will = nullptr;
  MQTTLinuxClientCallback callback;

  lwmqtt_posix_network_t network = {-1};
  lwmqtt_posix_timer_t timer1 = {0};
  lwmqtt_posix_timer_t timer2 = {0};
  lwmqtt_client_t client = lwmqtt_client_t();

#if defined(ERA_MQTT_SSL)
  lwmqtt_posix_tls_network_t networkTLS = {0};
  bool isTLS = false;
#endif

  bool _connected = false;
  uint16_t nextDupPacketID = 0;
  lwmqtt_return_code_t _returnCode = (lwmqtt_return_code_t)0;
  lwmqtt_err_t _lastError = (lwmqtt_err_t)0;
  uint32_t _droppedMessages = 0;

 public:
  void *ref = nullptr;

  explicit MQTTLinuxClient(int bufSize = 128) : MQTTLinuxClient(bufSize, bufSize) {}
  explicit MQTTLinuxClient(int readBufSize, int writeBufSize);

  ~MQTTLinuxClient();

  void begin();
  void begin(const char _hostname[]) { this->begin(_hostname, 1883); }
  void begin(const char _hostname[], int _port) {
    this->setTLSWithPort(_port);
    this->begin();
    this->setHost(_hostname, _port);
  }

  void init(int readBufSize, int writeBufSize);

  void onMessage(MQTTLinuxClientCallbackSimple cb);
  void onMessageAdvanced(MQTTLinuxClientCallbackAdvanced cb);
#if MQTT_HAS_FUNCTIONAL
  void onMessage(MQTTLinuxClientCallbackSimpleFunction cb);
  void onMessageAdvanced(MQTTLinuxClientCallbackAdvancedFunction cb);
#endif

  void setTLS(bool _isTLS);
  void setTLSWithPort(int _port);
  void setSkipACK(bool skip);
  bool getSkipACK() const {
    return this->skipACK;
  }

  void setHost(const char _hostname[]) { this->setHost(_hostname, 1883); }
  void setHost(const char hostname[], int port);

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

typedef MQTTLinuxClient MQTTClient;

#endif /* INC_MQTT_LINUX_HPP_ */
