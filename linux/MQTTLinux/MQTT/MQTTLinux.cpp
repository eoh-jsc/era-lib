#include <string.h>
#include "MQTTLinux.hpp"
#include <ERa/ERaDefine.hpp>

static void MQTTLinuxClientHandler(lwmqtt_client_t * /*client*/, void *ref, lwmqtt_string_t topic,
                              lwmqtt_message_t message) {
  // get callback
  auto cb = (MQTTLinuxClientCallback *)ref;

  // null terminate topic
  char terminated_topic[topic.len + 1];
  memcpy(terminated_topic, topic.data, topic.len);
  terminated_topic[topic.len] = '\0';

  // null terminate payload if available
  if (message.payload != nullptr) {
    message.payload[message.payload_len] = '\0';
  }

  // call the advanced callback and return if available
  if (cb->advanced != nullptr) {
    cb->advanced(cb->client, terminated_topic, (char *)message.payload, (int)message.payload_len);
    return;
  }
#if MQTT_HAS_FUNCTIONAL
  if (cb->functionAdvanced != nullptr) {
    cb->functionAdvanced(cb->client, terminated_topic, (char *)message.payload, (int)message.payload_len);
    return;
  }
#endif

  // return if simple callback is not set
#if MQTT_HAS_FUNCTIONAL
  if (cb->simple == nullptr && cb->functionSimple == nullptr) {
    return;
  }
#else
  if (cb->simple == nullptr) {
    return;
  }
#endif

  // create topic string
  std::string str_topic(terminated_topic);

  // create payload string
  std::string str_payload;
  if (message.payload != nullptr) {
    str_payload.append((const char *)message.payload);
  }

  // call simple callback
#if MQTT_HAS_FUNCTIONAL
  if (cb->functionSimple != nullptr) {
    cb->functionSimple(str_topic, str_payload);
  } else {
    cb->simple(str_topic, str_payload);
  }
#else
  cb->simple(str_topic, str_payload);
#endif
}

MQTTLinuxClient::MQTTLinuxClient(int bufSize) {
  // allocate buffers
  this->bufSize = (size_t)bufSize;
  this->readBuf = (uint8_t *)ERA_MALLOC((size_t)bufSize + 1);
  this->writeBuf = (uint8_t *)ERA_MALLOC((size_t)bufSize);
}

MQTTLinuxClient::~MQTTLinuxClient() {
  // free will
  this->clearWill();

  // free hostname
  if (this->hostname != nullptr) {
    free((void *)this->hostname);
  }

  // free buffers
  free(this->readBuf);
  free(this->writeBuf);
}

void MQTTLinuxClient::begin() {
  // initialize client
  lwmqtt_init(&this->client, this->writeBuf, this->bufSize, this->readBuf, this->bufSize);

  // set timers
  lwmqtt_set_timers(&this->client, &this->timer1, &this->timer2, lwmqtt_unix_timer_set, lwmqtt_unix_timer_get);

  // set network
  lwmqtt_set_network(&this->client, &this->network, lwmqtt_unix_network_read, lwmqtt_unix_network_write);

  // set callback
  lwmqtt_set_callback(&this->client, (void *)&this->callback, MQTTLinuxClientHandler);
}

void MQTTLinuxClient::onMessage(MQTTLinuxClientCallbackSimple cb) {
  // set callback
  this->callback.client = this;
  this->callback.simple = cb;
  this->callback.advanced = nullptr;
#if MQTT_HAS_FUNCTIONAL
  this->callback.functionSimple = nullptr;
  this->callback.functionAdvanced = nullptr;
#endif
}

void MQTTLinuxClient::onMessageAdvanced(MQTTLinuxClientCallbackAdvanced cb) {
  // set callback
  this->callback.client = this;
  this->callback.simple = nullptr;
  this->callback.advanced = cb;
#if MQTT_HAS_FUNCTIONAL
  this->callback.functionSimple = nullptr;
  this->callback.functionAdvanced = nullptr;
#endif
}

#if MQTT_HAS_FUNCTIONAL
void MQTTLinuxClient::onMessage(MQTTLinuxClientCallbackSimpleFunction cb) {
  // set callback
  this->callback.client = this;
  this->callback.simple = nullptr;
  this->callback.functionSimple = cb;
  this->callback.advanced = nullptr;
  this->callback.functionAdvanced = nullptr;
}

void MQTTLinuxClient::onMessageAdvanced(MQTTLinuxClientCallbackAdvancedFunction cb) {
  // set callback
  this->callback.client = this;
  this->callback.simple = nullptr;
  this->callback.functionSimple = nullptr;
  this->callback.advanced = nullptr;
  this->callback.functionAdvanced = cb;
}
#endif

void MQTTLinuxClient::setHost(const char _hostname[], int _port) {
  // free hostname if set
  if (this->hostname != nullptr) {
    free((void *)this->hostname);
  }

  // set hostname and port
  this->hostname = strdup(_hostname);
  this->port = _port;
}

void MQTTLinuxClient::setWill(const char topic[], const char payload[], bool retained, int qos) {
  // return if topic is missing
  if (topic == nullptr || strlen(topic) == 0) {
    return;
  }

  // clear existing will
  this->clearWill();

  // allocate will
  this->will = (lwmqtt_will_t *)ERA_MALLOC(sizeof(lwmqtt_will_t));
  memset(this->will, 0, sizeof(lwmqtt_will_t));

  // set topic
  this->will->topic = lwmqtt_string(strdup(topic));

  // set payload if available
  if (payload != nullptr && strlen(payload) > 0) {
    this->will->payload = lwmqtt_string(strdup(payload));
  }

  // set flags
  this->will->retained = retained;
  this->will->qos = (lwmqtt_qos_t)qos;
}

void MQTTLinuxClient::clearWill() {
  // return if not set
  if (this->will == nullptr) {
    return;
  }

  // free payload if set
  if (this->will->payload.len > 0) {
    free(this->will->payload.data);
  }

  // free topic if set
  if (this->will->topic.len > 0) {
    free(this->will->topic.data);
  }

  // free will
  free(this->will);
  this->will = nullptr;
}

void MQTTLinuxClient::setKeepAlive(int _keepAlive) { this->keepAlive = _keepAlive; }

void MQTTLinuxClient::setCleanSession(bool _cleanSession) { this->cleanSession = _cleanSession; }

void MQTTLinuxClient::setTimeout(int _timeout) { this->timeout = _timeout; }

bool MQTTLinuxClient::publish(const char topic[], const char payload[], int length, bool retained, int qos) {
  // return immediately if not connected
  if (!this->connected()) {
    return false;
  }

  // prepare message
  lwmqtt_message_t message = lwmqtt_default_message;
  message.payload = (uint8_t *)payload;
  message.payload_len = (size_t)length;
  message.retained = retained;
  message.qos = lwmqtt_qos_t(qos);

  // publish message
  this->_lastError = lwmqtt_publish(&this->client, lwmqtt_string(topic), message, this->timeout);
  if (this->_lastError != LWMQTT_SUCCESS) {
    // close connection
    this->close();

    return false;
  }

  return true;
}

bool MQTTLinuxClient::connect(const char clientID[], const char username[], const char password[], bool skip) {
  // close left open connection if still connected
  if (!skip && this->connected()) {
    this->close();
  }

  // connect to host
  if (!skip) {
    if (this->hostname != nullptr) {
      this->_lastError = lwmqtt_unix_network_connect(&this->network, (char*)this->hostname, (uint16_t)this->port);
    } else {
      this->_lastError = LWMQTT_NETWORK_FAILED_CONNECT;
    }
    if (this->_lastError != LWMQTT_SUCCESS) {
      return false;
    }
  }

  // prepare options
  lwmqtt_options_t options = lwmqtt_default_options;
  options.keep_alive = this->keepAlive;
  options.clean_session = this->cleanSession;
  options.client_id = lwmqtt_string(clientID);

  // set username and password if available
  if (username != nullptr) {
    options.username = lwmqtt_string(username);

    if (password != nullptr) {
      options.password = lwmqtt_string(password);
    }
  }

  // connect to broker
  this->_lastError = lwmqtt_connect(&this->client, options, this->will, &this->_returnCode, this->timeout);
  if (this->_lastError != LWMQTT_SUCCESS) {
    // close connection
    this->close();

    return false;
  }

  // set flag
  this->_connected = true;

  return true;
}

bool MQTTLinuxClient::subscribe(const char topic[], int qos) {
  // return immediately if not connected
  if (!this->connected()) {
    return false;
  }

  // subscribe to topic
  this->_lastError = lwmqtt_subscribe_one(&this->client, lwmqtt_string(topic), (lwmqtt_qos_t)qos, this->timeout);
  if (this->_lastError != LWMQTT_SUCCESS) {
    // close connection
    this->close();

    return false;
  }

  return true;
}

bool MQTTLinuxClient::unsubscribe(const char topic[]) {
  // return immediately if not connected
  if (!this->connected()) {
    return false;
  }

  // unsubscribe from topic
  this->_lastError = lwmqtt_unsubscribe_one(&this->client, lwmqtt_string(topic), this->timeout);
  if (this->_lastError != LWMQTT_SUCCESS) {
    // close connection
    this->close();

    return false;
  }

  return true;
}

bool MQTTLinuxClient::loop() {
  // return immediately if not connected
  if (!this->connected()) {
    return false;
  }

  // get available bytes on the network
  size_t available = 0;
  this->_lastError = lwmqtt_unix_network_peek(&this->network, &available);
  if (this->_lastError != LWMQTT_SUCCESS) {
    // close connection
    this->close();

    return false;
  }

  // yield if data is available
  if (available > 0) {
    this->_lastError = lwmqtt_yield(&this->client, available, this->timeout);
    if (this->_lastError != LWMQTT_SUCCESS) {
      // close connection
      this->close();

      return false;
    }
  }

  // keep the connection alive
  this->_lastError = lwmqtt_keep_alive(&this->client, this->timeout);
  if (this->_lastError != LWMQTT_SUCCESS) {
    // close connection
    this->close();

    return false;
  }

  return true;
}

bool MQTTLinuxClient::connected() {
  // a client is connected if the network is connected, a client is available and
  // the connection has been properly initiated
  return this->_connected;
}

bool MQTTLinuxClient::disconnect() {
  // return immediately if not connected anymore
  if (!this->connected()) {
    return false;
  }

  // cleanly disconnect
  this->_lastError = lwmqtt_disconnect(&this->client, this->timeout);

  // close
  this->close();

  return this->_lastError == LWMQTT_SUCCESS;
}

void MQTTLinuxClient::close() {
  // set flag
  this->_connected = false;

  // close network
  lwmqtt_unix_network_disconnect(&this->network);
}
