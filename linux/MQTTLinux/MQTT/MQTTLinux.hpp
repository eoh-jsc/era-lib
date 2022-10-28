#ifndef INC_MQTT_LINUX_HPP_
#define INC_MQTT_LINUX_HPP_

#include <string>
#include <functional>
#define MQTT_HAS_FUNCTIONAL 1

#include "unix/unix.hpp"

using namespace std;

class MQTTLinuxClient;

typedef void (*MQTTLinuxClientCallbackSimple)(std::string &topic, std::string &payload);
typedef void (*MQTTLinuxClientCallbackAdvanced)(MQTTLinuxClient *client, char topic[], char bytes[], int length);
#if MQTT_HAS_FUNCTIONAL
typedef std::function<void(std::string &topic, std::string &payload)> MQTTLinuxClientCallbackSimpleFunction;
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
  size_t bufSize = 0;
  uint8_t *readBuf = nullptr;
  uint8_t *writeBuf = nullptr;

  uint16_t keepAlive = 10;
  bool cleanSession = true;
  uint32_t timeout = 1000;

  const char *hostname = nullptr;
  int port = 0;
  lwmqtt_will_t *will = nullptr;
  MQTTLinuxClientCallback callback;

  lwmqtt_unix_network_t network = {0};
  lwmqtt_unix_timer_t timer1 = {0};
  lwmqtt_unix_timer_t timer2 = {0};
  lwmqtt_client_t client = lwmqtt_client_t();

  bool _connected = false;
  lwmqtt_return_code_t _returnCode = (lwmqtt_return_code_t)0;
  lwmqtt_err_t _lastError = (lwmqtt_err_t)0;

 public:
  void *ref = nullptr;

  explicit MQTTLinuxClient(int bufSize = 128);

  ~MQTTLinuxClient();

  void begin();
  void begin(const char _hostname[]) { this->begin(_hostname, 1883); }
  void begin(const char _hostname[], int _port) {
    this->begin();
    this->setHost(_hostname, _port);
  }

  void onMessage(MQTTLinuxClientCallbackSimple cb);
  void onMessageAdvanced(MQTTLinuxClientCallbackAdvanced cb);
#if MQTT_HAS_FUNCTIONAL
  void onMessage(MQTTLinuxClientCallbackSimpleFunction cb);
  void onMessageAdvanced(MQTTLinuxClientCallbackAdvancedFunction cb);
#endif

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

  bool connect(const char clientId[], bool skip = false) { return this->connect(clientId, nullptr, nullptr, skip); }
  bool connect(const char clientId[], const char username[], bool skip = false) {
    return this->connect(clientId, username, nullptr, skip);
  }
  bool connect(const char clientID[], const char username[], const char password[], bool skip = false);

  bool publish(const std::string &topic) { return this->publish(topic.c_str(), ""); }
  bool publish(const char topic[]) { return this->publish(topic, ""); }
  bool publish(const std::string &topic, const std::string &payload) { return this->publish(topic.c_str(), payload.c_str()); }
  bool publish(const std::string &topic, const std::string &payload, bool retained, int qos) {
    return this->publish(topic.c_str(), payload.c_str(), retained, qos);
  }
  bool publish(const char topic[], const std::string &payload) { return this->publish(topic, payload.c_str()); }
  bool publish(const char topic[], const std::string &payload, bool retained, int qos) {
    return this->publish(topic, payload.c_str(), retained, qos);
  }
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

  bool subscribe(const std::string &topic) { return this->subscribe(topic.c_str()); }
  bool subscribe(const std::string &topic, int qos) { return this->subscribe(topic.c_str(), qos); }
  bool subscribe(const char topic[]) { return this->subscribe(topic, 0); }
  bool subscribe(const char topic[], int qos);

  bool unsubscribe(const std::string &topic) { return this->unsubscribe(topic.c_str()); }
  bool unsubscribe(const char topic[]);

  bool loop();
  bool connected();

  lwmqtt_err_t lastError() { return this->_lastError; }
  lwmqtt_return_code_t returnCode() { return this->_returnCode; }

  bool disconnect();

 private:
  void close();
};

#endif /* INC_MQTT_LINUX_HPP_ */
