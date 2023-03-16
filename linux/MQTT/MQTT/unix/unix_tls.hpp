#ifndef LWMQTT_UNIX_TLS_HPP
#define LWMQTT_UNIX_TLS_HPP

#include <sys/time.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/error.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/platform.h>
#include <mbedtls/ssl.h>

extern "C" {
#include <MQTT/MQTT/lwmqtt/lwmqtt.h>
}

/**
 * The UNIX timer object.
 */
typedef struct {
  struct timeval end;
} lwmqtt_unix_tls_timer_t;

/**
 * Callback to set the UNIX timer object.
 *
 * @see lwmqtt_timer_set_t.
 */
void lwmqtt_unix_tls_timer_set(void *ref, uint32_t timeout);

/**
 * Callback to read the UNIX timer object.
 *
 * @see lwmqtt_timer_get_t.
 */
int32_t lwmqtt_unix_tls_timer_get(void *ref);

/**
 * The UNIX network object.
 */
typedef struct {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config conf;
  mbedtls_x509_crt cacert;
  mbedtls_net_context socket;
  uint8_t *ca_buf;
  size_t ca_len;
  bool verify;
} lwmqtt_unix_tls_network_t;

/**
 * Function init network cert.
 *
 * @param network - The network object.
 * @param verify - TLS is verify.
 * @param ca_buf - The CA buffer.
 * @param ca_len - The length of the CA buffer.
 */
void lwmqtt_unix_tls_network_init(lwmqtt_unix_tls_network_t *network, bool verify, const uint8_t *ca_buf, size_t ca_len);

/**
 * Function to establish a UNIX network connection.
 *
 * @param network - The network object.
 * @param host - The host.
 * @param port - The port.
 * @return An error value.
 */
lwmqtt_err_t lwmqtt_unix_tls_network_connect(lwmqtt_unix_tls_network_t *network, char *host, int port);

/**
 * Wait until the socket is connected or a timeout has been reached.
 *
 * @param network - The network object.
 * @param connected - The connected status.
 * @param timeout - The timeout.
 * @return An error value.
 */
lwmqtt_err_t lwmqtt_unix_tls_network_wait(lwmqtt_unix_tls_network_t *network, bool *connected, uint32_t timeout);

/**
 * Function to disconnect a UNIX network connection.
 *
 * @param network - The network object.
 */
void lwmqtt_unix_tls_network_disconnect(lwmqtt_unix_tls_network_t *network);

/**
 * Function to peek available bytes on a UNIX network connection.
 *
 * @param network - The network object.
 * @param available - Variables that is set with the available bytes.
 * @return An error value.
 */
lwmqtt_err_t lwmqtt_unix_tls_network_peek(lwmqtt_unix_tls_network_t *network, size_t *available, uint32_t timeout);

/**
 * Function to wait for a socket until data is available or the timeout has been reached.
 *
 * @param network - The network object.
 * @param available - Variables that is set with the available bytes.
 * @param timeout - The timeout.
 * @return An error value.
 */
lwmqtt_err_t lwmqtt_unix_tls_network_select(lwmqtt_unix_tls_network_t *network, bool *available, uint32_t timeout);

/**
 * Callback to read from a UNIX network connection.
 *
 * @see lwmqtt_network_read_t.
 */
lwmqtt_err_t lwmqtt_unix_tls_network_read(void *ref, uint8_t *buf, size_t len, size_t *received, uint32_t timeout);

/**
 * Callback to write to a UNIX network connection.
 *
 * @see lwmqtt_network_write_t.
 */
lwmqtt_err_t lwmqtt_unix_tls_network_write(void *ref, uint8_t *buf, size_t len, size_t *sent, uint32_t timeout);

#endif /* LWMQTT_UNIX_TLS_HPP */
