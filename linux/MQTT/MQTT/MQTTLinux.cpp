#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "MQTTLinux.hpp"
#include <ERa/ERaDefine.hpp>

inline char* lwmqtt_strdup(const char* str) {
  if (str == nullptr) {
    return nullptr;
  }

  size_t length {0};
  char* copy = nullptr;

  length = strlen(str) + sizeof("");
  copy = (char*)malloc(length);
  if (copy == nullptr) {
    return nullptr;
  }
  memcpy(copy, str, length);

  return copy;
}

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
  const char *str_topic = (const char *)terminated_topic;

  // create payload string
  const char *str_payload = (const char *)message.payload;

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

MQTTLinuxClient::MQTTLinuxClient(int readBufSize, int writeBufSize) {
  this->init(readBufSize, writeBufSize);
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
  lwmqtt_init(&this->client, this->writeBuf, this->writeBufSize, this->readBuf, this->readBufSize);

  // set CA cert
#if defined(ERA_MQTT_SSL)
  if (this->isTLS) {
    lwmqtt_posix_tls_network_init(&this->networkTLS, false, nullptr, 0);
  }
#endif

  // set timers
  lwmqtt_set_timers(&this->client, &this->timer1, &this->timer2, lwmqtt_posix_timer_set, lwmqtt_posix_timer_get);

  // set network
#if defined(ERA_MQTT_SSL)
  if (this->isTLS) {
    lwmqtt_set_network(&this->client, &this->networkTLS, lwmqtt_posix_tls_network_read, lwmqtt_posix_tls_network_write);
  }
  else {
    lwmqtt_set_network(&this->client, &this->network, lwmqtt_posix_network_read, lwmqtt_posix_network_write);
  }
#else
  lwmqtt_set_network(&this->client, &this->network, lwmqtt_posix_network_read, lwmqtt_posix_network_write);
#endif

  // set callback
  lwmqtt_set_callback(&this->client, (void *)&this->callback, MQTTLinuxClientHandler);
}

void MQTTLinuxClient::init(int readBufSize, int writeBufSize) {
  // allocate buffers
  if ((readBufSize != 0) && (this->readBuf == nullptr)) {
    this->readBufSize = (size_t)readBufSize;
    this->readBuf = (uint8_t *)ERA_MALLOC((size_t)readBufSize + 1);
  }
  if ((writeBufSize != 0) && (this->writeBuf == nullptr)) {
    this->writeBufSize = (size_t)writeBufSize;
    this->writeBuf = (uint8_t *)ERA_MALLOC((size_t)writeBufSize);
  }
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

void MQTTLinuxClient::setTLS(bool _isTLS) {
#if defined(ERA_MQTT_SSL)
  this->isTLS = _isTLS;
#else
  (void)_isTLS;
#endif
}

void MQTTLinuxClient::setTLSWithPort(int _port) {
#if defined(ERA_MQTT_SSL)
  if (_port == ERA_DEFAULT_MQTT_PORT_SSL) {
    this->isTLS = true;
  }
  else if (_port == ERA_DEFAULT_MQTT_PORT) {
    this->isTLS = false;
  }
#else
  (void)_port;
#endif
}

void MQTTLinuxClient::setSkipACK(bool skip) {
  this->skipACK = skip;
}

void MQTTLinuxClient::setHost(const char _hostname[], int _port) {
  // free hostname if set
  if (this->hostname != nullptr) {
    free((void *)this->hostname);
  }

  // set hostname and port
  this->hostname = lwmqtt_strdup(_hostname);
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
  this->will->topic = lwmqtt_string(lwmqtt_strdup(topic));

  // set payload if available
  if (payload != nullptr && strlen(payload) > 0) {
    this->will->payload = lwmqtt_string(lwmqtt_strdup(payload));
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

void MQTTLinuxClient::dropOverflow(bool enabled) {
  // configure drop overflow
  lwmqtt_drop_overflow(&this->client, enabled, &this->_droppedMessages);
}

bool MQTTLinuxClient::connect(const char clientID[], const char username[], const char password[], bool skip) {
  // close left open connection if still connected
  if (!skip && this->connected()) {
    this->close();
  }

  // connect to host
  if (!skip) {
    if (this->hostname != nullptr) {
#if defined(ERA_MQTT_SSL)
      if (this->isTLS) {
        this->_lastError = lwmqtt_posix_tls_network_connect(&this->networkTLS, (char*)this->hostname, (uint16_t)this->port);
      }
      else {
        this->_lastError = lwmqtt_posix_network_connect(&this->network, (char*)this->hostname, (uint16_t)this->port);
      }
#else
      this->_lastError = lwmqtt_posix_network_connect(&this->network, (char*)this->hostname, (uint16_t)this->port);
#endif
    } else {
      this->_lastError = LWMQTT_NETWORK_FAILED_CONNECT;
    }
    if (this->_lastError != LWMQTT_SUCCESS) {
      return false;
    }
  }

  // wait for connection
  bool isConnected = false;
#if defined(ERA_MQTT_SSL)
  if (this->isTLS) {
    this->_lastError = lwmqtt_posix_tls_network_wait(&this->networkTLS, &isConnected, this->timeout);
  }
  else {
    this->_lastError = lwmqtt_posix_network_wait(&this->network, &isConnected, this->timeout);
  }
#else
  this->_lastError = lwmqtt_posix_network_wait(&this->network, &isConnected, this->timeout);
#endif
  if (this->_lastError != LWMQTT_SUCCESS || !isConnected) {
    // close connection
    this->close();

    return false;
  }

  // prepare options
  lwmqtt_connect_options_t options = lwmqtt_default_connect_options;
  options.keep_alive = this->keepAlive;
  options.clean_session = this->cleanSession;
  options.client_id = lwmqtt_string(clientID);

  // set username and password if available
  if (username != nullptr) {
    options.username = lwmqtt_string(username);
  }
  if (password != nullptr) {
    options.password = lwmqtt_string(password);
  }

  // connect to broker
  this->_lastError = lwmqtt_connect(&this->client, &options, this->will, this->timeout);

  // copy return code
  this->_returnCode = options.return_code;

  // handle error
  if (this->_lastError != LWMQTT_SUCCESS) {
    // close connection
    this->close();

    return false;
  }

  // copy session present flag
  this->_sessionPresent = options.session_present;

  // set flag
  this->_connected = true;

  return true;
}

bool MQTTLinuxClient::publish(const char topic[], const char payload[], int length, bool retained, int qos) {
  // return immediately if not connected
  if (!this->connected()) {
    return false;
  }

  // wait for connection
  bool isConnected = false;
#if defined(ERA_MQTT_SSL)
  if (this->isTLS) {
    this->_lastError = lwmqtt_posix_tls_network_wait(&this->networkTLS, &isConnected, this->timeout);
  }
  else {
    this->_lastError = lwmqtt_posix_network_wait(&this->network, &isConnected, this->timeout);
  }
#else
  this->_lastError = lwmqtt_posix_network_wait(&this->network, &isConnected, this->timeout);
#endif
  if (this->_lastError != LWMQTT_SUCCESS) {
    // close connection
    this->close();

    return false;
  }

  if (!isConnected) {
    return false;
  }

  // prepare message
  lwmqtt_message_t message = lwmqtt_default_message;
  message.payload = (uint8_t *)payload;
  message.payload_len = (size_t)length;
  message.retained = retained;
  message.qos = lwmqtt_qos_t(qos);

  // prepare options
  lwmqtt_publish_options_t options = lwmqtt_default_publish_options;

  // set duplicate packet id if available
  if (this->nextDupPacketID > 0) {
    options.dup_id = &this->nextDupPacketID;
    this->nextDupPacketID = 0;
  }
  options.skip_ack = this->skipACK;

  // publish message
  this->_lastError = lwmqtt_publish(&this->client, &options, lwmqtt_string(topic), message, this->timeout);
  if (this->_lastError != LWMQTT_SUCCESS) {
    // close connection
    this->close();

    return false;
  }

  return true;
}

uint16_t MQTTLinuxClient::lastPacketID() {
  // get last packet id from client
  return this->client.last_packet_id;
}

void MQTTLinuxClient::prepareDuplicate(uint16_t packetID) {
  // set next duplicate packet id
  this->nextDupPacketID = packetID;
}

bool MQTTLinuxClient::subscribe(const char topic[], int qos) {
  // return immediately if not connected
  if (!this->connected()) {
    return false;
  }

  // wait for connection
  bool isConnected = false;
#if defined(ERA_MQTT_SSL)
  if (this->isTLS) {
    this->_lastError = lwmqtt_posix_tls_network_wait(&this->networkTLS, &isConnected, this->timeout);
  }
  else {
    this->_lastError = lwmqtt_posix_network_wait(&this->network, &isConnected, this->timeout);
  }
#else
  this->_lastError = lwmqtt_posix_network_wait(&this->network, &isConnected, this->timeout);
#endif
  if (this->_lastError != LWMQTT_SUCCESS || !isConnected) {
    // close connection
    this->close();

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

  // wait for connection
  bool isConnected = false;
#if defined(ERA_MQTT_SSL)
  if (this->isTLS) {
    this->_lastError = lwmqtt_posix_tls_network_wait(&this->networkTLS, &isConnected, this->timeout);
  }
  else {
    this->_lastError = lwmqtt_posix_network_wait(&this->network, &isConnected, this->timeout);
  }
#else
  this->_lastError = lwmqtt_posix_network_wait(&this->network, &isConnected, this->timeout);
#endif
  if (this->_lastError != LWMQTT_SUCCESS || !isConnected) {
    // close connection
    this->close();

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

  // TODO Block until data is available
  bool isAvailable = false;
#if defined(ERA_MQTT_SSL)
  if (this->isTLS) {
    this->_lastError = lwmqtt_posix_tls_network_select(&this->networkTLS, &isAvailable, 0);
  }
  else {
    this->_lastError = lwmqtt_posix_network_select(&this->network, &isAvailable, 0);
  }
#else
  this->_lastError = lwmqtt_posix_network_select(&this->network, &isAvailable, 0);
#endif
  if (this->_lastError != LWMQTT_SUCCESS) {
    // close connection
    this->close();

    return false;
  }

  if (isAvailable) {
    // get available bytes on the network
    size_t available = 0;
#if defined(ERA_MQTT_SSL)
    if (this->isTLS) {
      this->_lastError = lwmqtt_posix_tls_network_peek(&this->networkTLS, &available, this->timeout);
    }
    else {
      this->_lastError = lwmqtt_posix_network_peek(&this->network, &available, this->timeout);
    }
#else
    this->_lastError = lwmqtt_posix_network_peek(&this->network, &available, this->timeout);
#endif
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
  // a client is connected if the network is connected and
  // the connection has been properly initiated
#if defined(ERA_MQTT_SSL)
  if (this->isTLS) {
    return this->_connected && (this->networkTLS.socket.fd >= 0);
  }
  else {
    return this->_connected && (this->network.socket >= 0);
  }
#else
  return this->_connected && (this->network.socket >= 0);
#endif
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

  // close network
#if defined(ERA_MQTT_SSL)
  if (this->isTLS) {
    lwmqtt_posix_tls_network_disconnect(&this->networkTLS);
  }
  else {
    lwmqtt_posix_network_disconnect(&this->network);
  }
#else
  lwmqtt_posix_network_disconnect(&this->network);
#endif

  return this->_lastError == LWMQTT_SUCCESS;
}

void MQTTLinuxClient::close() {
  // set flag
  this->_connected = false;
}
