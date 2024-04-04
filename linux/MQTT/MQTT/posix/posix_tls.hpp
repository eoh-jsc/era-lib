#ifndef LWMQTT_POSIX_TLS_HPP
#define LWMQTT_POSIX_TLS_HPP

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
 * The POSIX network object.
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
} lwmqtt_posix_tls_network_t;

/**
 * Function init network cert.
 *
 * @param network - The network object.
 * @param verify - TLS is verify.
 * @param ca_buf - The CA buffer.
 * @param ca_len - The length of the CA buffer.
 */
void lwmqtt_posix_tls_network_init(lwmqtt_posix_tls_network_t *network, bool verify, const uint8_t *ca_buf, size_t ca_len);

/**
 * Function to establish a POSIX network connection.
 *
 * @param network - The network object.
 * @param host - The host.
 * @param port - The port.
 * @return An error value.
 */
lwmqtt_err_t lwmqtt_posix_tls_network_connect(lwmqtt_posix_tls_network_t *network, char *host, int port);

/**
 * Wait until the socket is connected or a timeout has been reached.
 *
 * @param network - The network object.
 * @param connected - The connected status.
 * @param timeout - The timeout.
 * @return An error value.
 */
lwmqtt_err_t lwmqtt_posix_tls_network_wait(lwmqtt_posix_tls_network_t *network, bool *connected, uint32_t timeout);

/**
 * Function to disconnect a POSIX network connection.
 *
 * @param network - The network object.
 */
void lwmqtt_posix_tls_network_disconnect(lwmqtt_posix_tls_network_t *network);

/**
 * Function to peek available bytes on a POSIX network connection.
 *
 * @param network - The network object.
 * @param available - Variables that is set with the available bytes.
 * @return An error value.
 */
lwmqtt_err_t lwmqtt_posix_tls_network_peek(lwmqtt_posix_tls_network_t *network, size_t *available, uint32_t timeout);

/**
 * Function to wait for a socket until data is available or the timeout has been reached.
 *
 * @param network - The network object.
 * @param available - Variables that is set with the available bytes.
 * @param timeout - The timeout.
 * @return An error value.
 */
lwmqtt_err_t lwmqtt_posix_tls_network_select(lwmqtt_posix_tls_network_t *network, bool *available, uint32_t timeout);

/**
 * Callback to read from a POSIX network connection.
 *
 * @see lwmqtt_network_read_t.
 */
lwmqtt_err_t lwmqtt_posix_tls_network_read(void *ref, uint8_t *buf, size_t len, size_t *received, uint32_t timeout);

/**
 * Callback to write to a POSIX network connection.
 *
 * @see lwmqtt_network_write_t.
 */
lwmqtt_err_t lwmqtt_posix_tls_network_write(void *ref, uint8_t *buf, size_t len, size_t *sent, uint32_t timeout);

#endif /* LWMQTT_POSIX_TLS_HPP */
